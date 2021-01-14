#include "ped.hh"
#include "base.hh"
#include <cstdlib>
#include "logger.hh"
#include "injector/injector.hpp"
#include "util/loader.hh"
#include "injector/calling.hpp"
#include "config.hh"

PedRandomizer *PedRandomizer::mInstance = nullptr;

/*******************************************************/
int
PedRandomizer::ChooseRandomPedToLoad ()
{
    int ped = 0;
    while ((ped = random (288)),
           IsModelBlacklisted (ped) || ms_aInfoForModel[ped].m_nLoadState == 1)
        ;

    return ped;
}
/*******************************************************/
void __fastcall PedRandomizer::RandomizeGenericPeds (CCivilianPed *ped,
                                                     void *edx, ePedType type,
                                                     int model)
{
    int newModel = model;

    if (*ms_numPedsLoaded > 1)
        newModel = ms_pedsLoaded[random (*ms_numPedsLoaded - 1)];

    // Final checks before spawning
    if (ms_aInfoForModel[newModel].m_nLoadState != 1 || IsSpecialModel (model))
        newModel = model;

    Logger::GetLogger ()->LogMessage ("Ped: " + std::to_string (model) + ", "
                                      + std::to_string (newModel));

    ped->CivilianPed (type, newModel);
}
/*******************************************************/
void __fastcall PedRandomizer::RandomizeCopPeds (CPed *ped, void *edx,
                                                 int modelIndex)
{
    int newModel = 0;

    if (*ms_numPedsLoaded > 1)
        newModel = ms_pedsLoaded[random (*ms_numPedsLoaded - 1)];

    // Final checks before spawning
    if (ms_aInfoForModel[newModel].m_nLoadState != 1)
        newModel = modelIndex;

    Logger::GetLogger ()->LogMessage ("Cop: " + std::to_string (modelIndex)
                                      + ", " + std::to_string (newModel));

    ped->SetModelIndex (newModel);
}
/*******************************************************/
void
PedRandomizer::RandomizeSpecialModels (int slot, const char *modelName,
                                       int flags)
{
    std::string &newModel = special_models[random (special_models.size () - 1)];

    CStreaming::RequestSpecialModel (slot, newModel.c_str (), flags);
    CStreaming::LoadAllRequestedModels (false);
}
/*******************************************************/
bool
PedRandomizer::IsModelBlacklisted (int model)
{
    if (model > 0 && model < 7)
        return true;

    if (model == 8 || model == 42 || model == 65 || model == 74 || model == 86
        || model == 119 || model == 208 || model == 149 || model == 289)
        return true;

    if (model > 264 && model < 274)
        return true;

    return false;
}
/*******************************************************/
bool
PedRandomizer::IsSpecialModel (int model)
{
    if (model >= 290 && model <= 299)
        return true;

    return false;
}
/*******************************************************/
void __stdcall ForceCopType (int pedType, int *modelIndex)
{
    if (pedType == 6 && *modelIndex == 283)
        *modelIndex = 280;
    
    HookManager::CallOriginal<injector::stdcall<int (int, int *)>,
        0x464F50> (pedType, modelIndex);
}

/*******************************************************/
void
PedRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("PedRandomizer"))
        return;

    // If Generic Peds Enabled
    for (int addr :
         {0x444E02, 0x445638, 0x461580, 0x4675DA, 0x469CDB, 0x47D8E6, 0x4833BC,
          0x491D36, 0x61276E, 0x6128E4, 0x6129B2, 0x45CFD8})
        injector::MakeCALL (addr, RandomizeGenericPeds);

    injector::MakeJMP (0x60FFD0, ChooseRandomPedToLoad);

    // If Special Models Enabled
    injector::MakeJMP (0x40B45E, RandomizeSpecialModels);

    // If random cops enabled
    for (int addr :
         {0x5DDD8A, 0x5DDD96, 0x5DDDD6, 0x5DDE16, 0x5DDE54, 0x5DDCB0})
        injector::MakeCALL (addr, RandomizeCopPeds);

    // If NSFW enabled
    for (std::string &model : nsfw_models)
        special_models.push_back (model);

    RegisterHooks ({{HOOK_CALL, 0x469C64, (void *) &ForceCopType}});

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
