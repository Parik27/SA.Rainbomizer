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
ClothesRandomizer::RandomizePlayerModel ()
{
    int model = 0;
    while ((model = random (299)), !PedRandomizer::IsModelValidPedModel (model))
        ;

    if (PedRandomizer::IsSpecialModel (model))
        {
            model = 298;
            CStreaming::RequestSpecialModel (
                model, GetRandomElement (PedRandomizer::specialModels).c_str (),
                1);
        }
    else
        CStreaming::RequestModel (model, 1);

    CStreaming::LoadAllRequestedModels (false);

    if (ms_aInfoForModel[model].m_nLoadState != 1)
        model = 0;

    Logger::GetLogger ()->LogMessage ("Player Model: "
                                      + std::to_string (model));

    Scrpt::CallOpcode (0x09C7, "set_player_model", GlobalVar (2), model);
}

/*******************************************************/
void
ClothesRandomizer::RandomizePlayerClothes ()
{
    Scrpt::CallOpcode (0x09C7, "set_player_model", GlobalVar (2), 0);

    for (int i = 0; i < 17; i++)
        {
            auto cloth
                = ClothesRandomizer::GetInstance ()->GetRandomCRCForComponent (
                    i);

            Scrpt::CallOpcode (0x784, "set_player_model_tex_crc", GlobalVar (2),
                               cloth.second, cloth.first, i);
            Scrpt::CallOpcode (0x070D, "rebuild_player", GlobalVar (2));
        }
}

/*******************************************************/
void
ClothesRandomizer::HandleClothesChange ()
{
    if (CGame::bMissionPackGame)
        return;

    if (random (100) >= 50)
        RandomizePlayerClothes ();
    else
        RandomizePlayerModel ();
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
    auto config = ConfigManager::GetInstance ()->GetConfigs ().clothes;
    if (!config.enabled)
        return;

    mInitialised = false;

    FadesManager::AddFadeCallback (HandleClothesChange);
    injector::MakeCALL (0x5A834D, FixChangingClothes);
    injector::MakeCALL (0x5A82AF, FixChangingClothes);

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
