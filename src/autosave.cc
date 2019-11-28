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
    puts ("Should save!");
    AutoSave::GetInstance ()->SetShouldSave (true);
    CStats::IncrementStat (id, val);
}

/*******************************************************/
void
AutoSave::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().general;
    if (config.save_slot < 1 || config.save_slot > 8)
        return;

    this->InstallHooks ();
    RegisterHooks ({{HOOK_CALL, 0x480D11, (void *) &::SetShouldSave}});
    Logger::GetLogger ()->LogMessage ("Intialised AutoSave");
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
