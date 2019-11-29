#include "autosave.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "config.hh"
#include "injector/injector.hpp"

AutoSave *AutoSave::mInstance = nullptr;

/*******************************************************/
int __fastcall HandleAutosave (CRunningScript *scr, void *edx)
{
    AutoSave::GetInstance ()->RevertHooks ();
    scr->EndThread ();
    AutoSave::GetInstance ()->InstallHooks ();

    if (scr->m_bIsMission && AutoSave::GetInstance ()->ShouldSave ())
        {
            scr->m_bIsActive = false;

            auto config = ConfigManager::GetInstance ()->GetConfigs ().general;

            // Save in a vehicle
            int *playerFlags   = (int *) (((char *) FindPlayerPed (0)) + 0x46C);
            int  originalFlags = playerFlags[0];
            playerFlags[0] &= ~0x100;

            // Save
            CGenericGameStorage::MakeValidSaveFileName (config.save_slot - 1);
            CGenericGameStorage::GenericSave ();

            AutoSave::GetInstance ()->SetShouldSave (false);

            playerFlags[0] = originalFlags;
        }
    return 0;
}

/*******************************************************/
void
SetShouldSave (uint16_t id, float val)
{
    AutoSave::GetInstance ()->SetShouldSave (true);
    CStats::IncrementStat (id, val);
}

/*******************************************************/
char *__fastcall SetShouldSaveGlobalVars (CRunningScript *scr, void *edx,
                                          int a2)
{
    char *var        = scr->GetPointerToScriptVariable (a2);
    int   global_var = (var - (char *) ScriptSpace) / 4;

    printf ("%d\n", global_var);
    if (AutoSave::GetInstance ()->IsMissionGlobalVariable (global_var))
        AutoSave::GetInstance ()->SetShouldSave (true);

    return var;
}

/*******************************************************/
void
AutoSave::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().general;
    if (config.save_slot < 1 || config.save_slot > 8)
        return;

    this->InstallHooks ();
    RegisterHooks (
        {{HOOK_CALL, 0x480D11, (void *) &::SetShouldSave},
         {HOOK_CALL, 0x465FC3, (void *) &::SetShouldSaveGlobalVars}});

    Logger::GetLogger ()->LogMessage ("Intialised AutoSave");
}

/*******************************************************/
bool
AutoSave::IsMissionGlobalVariable (int global_var)
{
    // $629 => $RIOT_TOTAL_PASSED_MISSIONS
    // Note: This is for adding more mission global variables for fine tuning
    //       when saves happen
    for (int variable : {629})
        {
            if (variable == global_var)
                return true;
        }

    return false;
}

/*******************************************************/
void
AutoSave::InstallHooks ()
{
    memcpy (this->m_original_data, (void *) 0x465AA0, ORIGINAL_DATA_SIZE);
    injector::MakeJMP (0x465AA0, (void *) &HandleAutosave);
}

/*******************************************************/
void
AutoSave::RevertHooks ()
{
    memcpy ((void *) 0x465AA0, this->m_original_data, ORIGINAL_DATA_SIZE);
}

/*******************************************************/
void
AutoSave::DestroyInstance ()
{
    if (AutoSave::mInstance)
        delete AutoSave::mInstance;
}

/*******************************************************/
AutoSave *
AutoSave::GetInstance ()
{
    if (!AutoSave::mInstance)
        {
            AutoSave::mInstance = new AutoSave ();
            atexit (&AutoSave::DestroyInstance);
        }
    return AutoSave::mInstance;
}
