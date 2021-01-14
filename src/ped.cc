#include "ped.hh"
#include "base.hh"
#include <cstdint>
#include <cstdlib>
#include "functions.hh"
#include "injector/calling.hpp"
#include "logger.hh"
#include "injector/injector.hpp"
#include "util/loader.hh"
#include "injector/calling.hpp"
#include "config.hh"

PedRandomizer *PedRandomizer::mInstance = nullptr;

std::vector<std::string> PedRandomizer::specialModels
    = {"TRUTH",  "MACCER", "TENPEN",  "PULASKI", "HERN",    "DWAYNE",
       "SMOKE",  "SWEET",  "RYDER",   "FORELLI", "ROSE",    "PAUL",
       "CESAR",  "OGLOC",  "WUZIMU",  "TORINO",  "JIZZY",   "MADDOGG",
       "CAT",    "CLAUDE", "RYDER2",  "RYDER3",  "EMMET",   "ANDRE",
       "KENDL",  "JETHRO", "ZERO",    "TBONE",   "SINDACO", "JANITOR",
       "BBTHIN", "SMOKEV", "GUNGRL2", "COPGRL2", "NURGRL2", "CROGRL2",
       "BB",     "SUZIE",  "PSYCHO"};

/*******************************************************/
int
PedRandomizer::ChooseRandomPedToLoad ()
{
    int ped = 0;
    while ((ped = random (288)), !IsModelValidPedModel (ped)
                                     || ms_aInfoForModel[ped].m_nLoadState == 1)
        ;

    return ped;
}

/*******************************************************/
uint32_t
PedRandomizer::GetRandomModelIndex (uint32_t originalIdx)
{
    if (originalIdx == 0)
        return originalIdx;

    int newModel = originalIdx;

    if (*ms_numPedsLoaded > 1)
        newModel = GetRandomElement (ms_pedsLoaded, *ms_numPedsLoaded);

    // Final checks before spawning
    if (ms_aInfoForModel[newModel].m_nLoadState != 1
        || IsSpecialModel (originalIdx))
        newModel = originalIdx;

    return newModel;
}

/*******************************************************/
void __fastcall PedRandomizer::RandomizePedModelIndex (CEntity *entity, void *,
                                                       uint32_t index)
{
    HookManager::CallOriginal<injector::thiscall<void (CEntity *, uint32_t)>,
                              0x5E4890> (entity, GetRandomModelIndex (index));
}

/*******************************************************/
void
PedRandomizer::RandomizeSpecialModels (int slot, const char *modelName,
                                       int flags)
{
    const std::string &newModel = GetRandomElement (specialModels);

    CStreaming::RequestSpecialModel (slot, newModel.c_str (), flags);
    CStreaming::LoadAllRequestedModels (false);
}

/*******************************************************/
bool
PedRandomizer::IsModelValidPedModel (int model)
{
    return ms_modelInfoPtrs[model]
           && ms_modelInfoPtrs[model]->GetModelType () == 7;
}

/*******************************************************/
bool
PedRandomizer::IsSpecialModel (int model)
{
    if ((model >= 290 && model <= 299))
        return true;

    return false;
}

/*******************************************************/
void
PedRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("PedRandomizer"))
        return;

    // If NSFW enabled
    for (auto &model : {"GANGRL1", "MECGRL1", "GUNGRL1", "COPGRL1", "NURGRL1",
                        "CROGRL1", "GANGRL2", "COPGRL2"})
        specialModels.push_back (model);

    // If Special Models Enabled
    injector::MakeJMP (0x40B45E, RandomizeSpecialModels);
    injector::MakeJMP (0x60FFD0, ChooseRandomPedToLoad);

    RegisterHooks ({{HOOK_CALL, 0x5E4890, (void *) RandomizePedModelIndex}});

    Logger::GetLogger ()->LogMessage ("Intialised PedRandomizer");
}

/*******************************************************/
void
PedRandomizer::DestroyInstance ()
{
    if (PedRandomizer::mInstance)
        delete PedRandomizer::mInstance;
}

/*******************************************************/
PedRandomizer *
PedRandomizer::GetInstance ()
{
    if (!PedRandomizer::mInstance)
        {
            PedRandomizer::mInstance = new PedRandomizer ();
            atexit (&PedRandomizer::DestroyInstance);
        }
    return PedRandomizer::mInstance;
}
