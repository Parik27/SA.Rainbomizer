#include "clothes.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "injector/calling.hpp"
#include "functions.hh"
#include "util/scrpt.hh"
#include <windows.h>
#include "config.hh"
#include "fades.hh"
#include "ped.hh"

ClothesRandomizer *ClothesRandomizer::mInstance = nullptr;

/*******************************************************/
void
SetModel (int modelIndex)
{
    auto ped          = FindPlayerPed ();
    auto associations = RpAnimBlendClumpExtractAssociations (ped->m_pRwClump);
    Scrpt::CallOpcode (0x09C7, "set_player_model", GlobalVar (2), modelIndex);
    RpAnimBlendClumpGiveAssociations (ped->m_pRwClump, associations);
}

/*******************************************************/
void
ClothesRandomizer::RandomizePlayerModel ()
{
    int model = 0;
    while ((model = random (299)), !PedRandomizer::IsModelValidPedModel (model))
        ;

    if (m_Config.ForcedModel >= 0)
        model = m_Config.ForcedModel;

    if (PedRandomizer::IsSpecialModel (model))
        {
            model = 298;
            if (m_Config.ForcedSpecial == "")
                {
                    std::string randomSpecialModel
                        = GetRandomElement (PedRandomizer::specialModels);

                    if (m_Config.RandomizePlayerOnce
                        && !m_RandomizeOnceInfo.Initialised)
                        {
                            m_RandomizeOnceInfo.SpecialModel
                                = randomSpecialModel;
                            m_Config.ForcedSpecial = randomSpecialModel;
                        }

                    CStreaming::RequestSpecialModel (
                        model, randomSpecialModel.c_str (), 0);
                }
            else
                CStreaming::RequestSpecialModel (
                    model, m_Config.ForcedSpecial.c_str (), 0);
        }
    else
        CStreaming::RequestModel (model, 0);

    CStreaming::LoadAllRequestedModels (false);

    if (m_Config.RandomizePlayerOnce && !m_RandomizeOnceInfo.Initialised)
        {
            m_RandomizeOnceInfo.isClothes = false;
            if (m_Config.ForcedModel >= 0)
                m_RandomizeOnceInfo.ChosenModel = m_Config.ForcedModel;
            else
                m_RandomizeOnceInfo.ChosenModel = model;

            m_Config.ForcedModel            = m_RandomizeOnceInfo.ChosenModel;
            m_RandomizeOnceInfo.Initialised = true;
        }

    if (ms_aInfoForModel[model].m_nLoadState != 1)
        model = 0;

    SetModel (model);
}

/*******************************************************/
void
ClothesRandomizer::RandomizePlayerClothes ()
{
    SetModel (0);

    for (int i = 0; i < 17; i++)
        {
            auto cloth
                = ClothesRandomizer::GetInstance ()->GetRandomCRCForComponent (
                    i);

            if (m_Config.RandomizePlayerOnce
                && !m_RandomizeOnceInfo.Initialised)
                m_RandomizeOnceInfo.RandomClothes.push_back (cloth);
            else if (m_Config.RandomizePlayerOnce
                     && m_RandomizeOnceInfo.Initialised)
                cloth = m_RandomizeOnceInfo.RandomClothes.at (i);

            Scrpt::CallOpcode (0x784, "set_player_model_tex_crc", GlobalVar (2),
                               cloth.second, cloth.first, i);
            Scrpt::CallOpcode (0x070D, "rebuild_player", GlobalVar (2));
        }

    if (m_Config.RandomizePlayerOnce && !m_RandomizeOnceInfo.Initialised)
        {
            m_RandomizeOnceInfo.isClothes   = true;
            m_RandomizeOnceInfo.Initialised = true;
        }

    Scrpt::CallOpcode (0x070D, "rebuild_player", GlobalVar (2));
    CStreaming::LoadAllRequestedModels (false);
}

/*******************************************************/
void
ClothesRandomizer::HandleClothesChange ()
{
    if (CGame::bMissionPackGame)
        return;

    if (m_Config.RandomizePlayerOnce && m_RandomizeOnceInfo.Initialised)
        {
            if (m_RandomizeOnceInfo.isClothes)
                RandomizePlayerClothes ();
            else
                RandomizePlayerModel ();
        }
    else if (m_Config.RandomizePlayerClothing && m_Config.RandomizePlayerModel
             && !m_RandomizeOnceInfo.Initialised)
        {
            if (random (100) >= m_Config.OddsOfNewModel
                && m_Config.ForcedModel < 0)
                RandomizePlayerClothes ();
            else
                RandomizePlayerModel ();
        }
    else if (m_Config.RandomizePlayerClothing && !m_Config.RandomizePlayerModel
             && m_Config.ForcedModel < 0)
        RandomizePlayerClothes ();
    else if (m_Config.RandomizePlayerModel && !m_Config.RandomizePlayerClothing)
        RandomizePlayerModel ();
    else
        return;
}

/*******************************************************/
void
ClothesRandomizer::InitialiseClothes ()
{
    std::vector<std::string> shops
        = {"CSchp", "CSsprt",  "LACS1",   "clothgp", "Csdesgn",
           "Csexl", "barbers", "barber2", "barber3"};

    for (auto i : shops)
        {
            CShopping::LoadShop (i.c_str ());
            for (int i = 0; i < CShopping::m_nTotalItems; i++)
                {
                    auto &item = CShopping::m_aShoppingItems[i];
                    if (item.modelType >= 0 && item.modelType <= 17)
                        {
                            mClothes[item.modelType].push_back (
                                std::make_pair (item.modelName,
                                                item.textureName));
                        }
                }
        }

    mInitialised = true;
}

/*******************************************************/
void
ClothesRandomizer::FixChangingClothes (int modelId, uint32_t *newClothes,
                                       uint32_t *oldClothes,
                                       bool      CutscenePlayer)

{
    int model = 0;
    if (CutscenePlayer)
        model = 1;

    Call<0x5A81E0> (model, newClothes, oldClothes, CutscenePlayer);
}
/*******************************************************/
int __fastcall ClothesRandomizer::FixAnimCrash (uint32_t *anim, void *edx,
                                                int arg0, int animGroup)
{
    if (animGroup > 0)
        animGroup = 0;

    return CallMethodAndReturn<int, 0x6E3D10> (anim, arg0, animGroup);
}
/*******************************************************/
std::pair<int, int>
ClothesRandomizer::GetRandomCRCForComponent (int componentId)
{
    if (!mInitialised)
        InitialiseClothes ();

    int randomId = random (mClothes[componentId].size ()) - 1;
    if (randomId < 0)
        return {0, 0};

    return mClothes[componentId][randomId];
}

/*******************************************************/
void
ClothesRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "PlayerRandomizer",
            std::pair ("RandomizePlayerModel", &m_Config.RandomizePlayerModel),
            std::pair ("RandomizePlayerClothing",
                       &m_Config.RandomizePlayerClothing),
            std::pair ("OddsOfNewModel", &m_Config.OddsOfNewModel),
            std::pair ("IncludeNSFWModels", &m_Config.IncludeNSFWModels),
            std::pair ("RandomizePlayerOnce", &m_Config.RandomizePlayerOnce),
            std::pair ("ForcedModel", &m_Config.ForcedModel),
            std::pair ("ForcedSpecialModel", &m_Config.ForcedSpecial)))
        return;

    if (m_Config.OddsOfNewModel < 0 || m_Config.OddsOfNewModel > 100)
        m_Config.OddsOfNewModel = 80;

    mInitialised = false;

    FadesManager::AddFadeCallback (HandleClothesChange);
    injector::MakeCALL (0x5A834D, FixChangingClothes);
    injector::MakeCALL (0x5A82AF, FixChangingClothes);

    for (int addr : {0x64561B, 0x64C3FE, 0x64E7EE, 0x64EA43, 0x64EB0F, 0x64FD1E,
                     0x64FD57, 0x64FE54})
        injector::MakeCALL (addr, FixAnimCrash);

    Logger::GetLogger ()->LogMessage ("Intialised ClothesRandomizer");
}

/*******************************************************/
void
ClothesRandomizer::DestroyInstance ()
{
    if (ClothesRandomizer::mInstance)
        delete ClothesRandomizer::mInstance;
}

/*******************************************************/
ClothesRandomizer *
ClothesRandomizer::GetInstance ()
{
    if (!ClothesRandomizer::mInstance)
        {
            ClothesRandomizer::mInstance = new ClothesRandomizer ();
            atexit (&ClothesRandomizer::DestroyInstance);
        }
    return ClothesRandomizer::mInstance;
}
