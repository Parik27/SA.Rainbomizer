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
#include <vector>

PedRandomizer *PedRandomizer::mInstance = nullptr;

std::vector<std::string> PedRandomizer::specialModels;

/*******************************************************/
int
PedRandomizer::ChooseRandomPedToLoad ()
{
    int ped = 0;
    while ((ped = random (288)), !IsModelValidPedModel (ped)
                                     || ms_aInfoForModel[ped].m_nLoadState == 1)
        ;

    if (m_Config.ForcedPed >= 0)
        ped = m_Config.ForcedPed;

    return ped;
}

/*******************************************************/
uint32_t
PedRandomizer::GetRandomModelIndex (uint32_t originalIdx)
{
    if (originalIdx == 0)
        return originalIdx;

    if (!m_Config.RandomizeCops && originalIdx >= 280 && originalIdx <= 288)
        return originalIdx;

    if (m_Config.RandomizeCops && !m_Config.RandomizeGenericModels && !m_Config.RandomizeGangMembers 
        && (originalIdx < 280 || originalIdx > 288))
        return originalIdx;

    if (!m_Config.RandomizeGangMembers && originalIdx >= 102
        && originalIdx <= 127)
        return originalIdx;

    if (m_Config.RandomizeGangMembers && !m_Config.RandomizeGenericModels && !m_Config.RandomizeCops
        && (originalIdx < 102 || originalIdx > 127))
        return originalIdx;

    if (m_Config.RandomizeGangMembers && !m_Config.RandomizeGenericModels && m_Config.RandomizeCops 
        && ((!(originalIdx >= 102) && !(originalIdx <= 127)) || (!(originalIdx >= 280) && !(originalIdx <= 288))))
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
    std::string &newModel = GetRandomElement (specialModels);

    if (m_Config.ForcedSpecial != "")
        newModel = m_Config.ForcedSpecial;

    CStreaming::RequestSpecialModel (slot, newModel.c_str (), 0);
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
CheckDataAboutKilledPeds (CPed *pedKilled, int typeOfWeapon, char wasHeadShot,
                          int vehicle)
{
    char canTriggerGangWar = injector::ReadMemory<char> (0x96AB93);
    int  numSpecificZones  = injector::ReadMemory<int> (0x96AB94);
    float gangAnger         = injector::ReadMemory<float> (0x96AB5C);
    Logger::GetLogger()->LogMessage("Dead Ped Type: "
                       + std::to_string (pedKilled->m_nPedType));
    Logger::GetLogger ()->LogMessage (
        "Gang Info (can trigger, num specific, anger): "
        + std::to_string ((int) canTriggerGangWar) + ", "
        + std::to_string (numSpecificZones) + ", "
        + std::to_string (gangAnger));
    Call<0x43DCD0>(pedKilled, typeOfWeapon, wasHeadShot, vehicle);
}

/*******************************************************/
char
MakeGangWarsConsistent (int pedType)
{
    if (pedType >= 7 && pedType <= 16)
        pedType = 7;
    return CallAndReturn<char, 0x443950> (pedType);
}

/*******************************************************/
void
PedRandomizer::Initialise ()
{
    if (ConfigManager::ReadConfig ("PedRandomizer",
        std::pair ("IncludeNSFWModels", &m_Config.IncludeNSFWModels))
        || ConfigManager::ReadConfig ("PlayerRandomizer", 
            std::pair ("IncludeNSFWModels", &m_Config.IncludeNSFWModels)))
    {
        for (auto &model :
                 {"TRUTH",   "MACCER", "TENPEN",  "PULASKI", "HERN",
                  "DWAYNE",  "SMOKE",  "SWEET",   "RYDER",   "FORELLI",
                  "ROSE",    "PAUL",   "CESAR",   "OGLOC",   "WUZIMU",
                  "TORINO",  "JIZZY",  "MADDOGG", "CAT",     "CLAUDE",
                  "RYDER2",  "RYDER3", "EMMET",   "ANDRE",   "KENDL",
                  "JETHRO",  "ZERO",   "TBONE",   "SINDACO", "JANITOR",
                  "BBTHIN",  "SMOKEV", "GUNGRL2", "NURGRL2",
                  "CROGRL2", "BB",     "SUZIE",   "PSYCHO"})
                specialModels.push_back (model);

        // If NSFW enabled
        if (m_Config.IncludeNSFWModels)
        {
            for (auto &model : {"GANGRL1", "MECGRL1", "GUNGRL1", "COPGRL1",
                                    "NURGRL1", "CROGRL1", "GANGRL2", "COPGRL2"})
                specialModels.push_back (model);
        }
    }

    if (!ConfigManager::ReadConfig ("PedRandomizer", 
        std::pair("RandomizeGenericModels", &m_Config.RandomizeGenericModels), 
        std::pair("RandomizeCops", &m_Config.RandomizeCops),
        std::pair("RandomizeGangMembers", &m_Config.RandomizeGangMembers),
        std::pair("RandomizeSpecialModels", &m_Config.RandomizeSpecialModels), 
        std::pair("IncludeNSFWModels", &m_Config.IncludeNSFWModels),
        std::pair("ForcedPedModel", &m_Config.ForcedPed),
        std::pair("ForcedSpecialModel", &m_Config.ForcedSpecial)))
        return;

    // If Special Models Enabled
    if (m_Config.RandomizeSpecialModels)
        injector::MakeJMP (0x40B45E, RandomizeSpecialModels);
    
    if (m_Config.RandomizeGenericModels || m_Config.RandomizeCops || m_Config.RandomizeGangMembers)
    {
        injector::MakeJMP (0x60FFD0, ChooseRandomPedToLoad);

        RegisterHooks (
                {{HOOK_CALL, 0x5E4890, (void *) RandomizePedModelIndex}});
    }

    if (m_Config.RandomizeGangMembers)
        injector::MakeCALL (0x43DEE9, (void *) MakeGangWarsConsistent);

    //RegisterHooks ({{HOOK_CALL, 0x4B93AA, (void *) &CheckDataAboutKilledPeds},
    //                {HOOK_CALL, 0x6D1CBC, (void *) &CheckDataAboutKilledPeds},
    //                {HOOK_CALL, 0x6D1D27, (void *) &CheckDataAboutKilledPeds},
    //                {HOOK_CALL, 0x6D2276, (void *) &CheckDataAboutKilledPeds},
    //                {HOOK_CALL, 0x6D22B7, (void *) &CheckDataAboutKilledPeds}});

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
