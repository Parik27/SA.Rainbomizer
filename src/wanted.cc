#include "wanted.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "config.hh"
#include "functions.hh"
#include "injector/calling.hpp"
#include <random>

WantedLevelRandomizer *WantedLevelRandomizer::mInstance        = nullptr;
float                  WantedLevelRandomizer::mNextChaosPoints = 0;

/*******************************************************/
bool
RandomizeChaosIncrement ()
{
    WantedLevelRandomizer::GetInstance ()->RandomizeChaosPoints ();
    return HookManager::CallOriginalAndReturn<injector::cstd<bool ()>,
                                              0x56218E> (false);
}

/*******************************************************/
void __fastcall RandomizeMissionWantedLevel (CRunningScript *scr, void *edx,
                                             short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[1] != 0)
    {
        bool greater = (bool) random (1);
        if (!greater)
            ScriptParams[1] = random (1, ScriptParams[1]);
        else
            ScriptParams[1] = random (ScriptParams[1], 6);
    }
}

/*******************************************************/
void
WantedLevelRandomizer::RandomizeChaosPoints ()
{
    mNextChaosPoints = int (randomNormal (0.016, 0.1) * 3000);
}

/*******************************************************/
void
WantedLevelRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("WantedLevelRandomizer",
                                    std::pair ("RandomizeMissionWantedLevels",
                                               &m_Config.RandomizeMission),
                                    std::pair ("RandomizeChaosPoints",
                                               &m_Config.RandomizeChaos)))
        return;

    for (auto ref :
         {0x5621BB, 0x5621CA, 0x5621D9, 0x5621E5, 0x5621F1, 0x5621FD, 0x562209,
          0x562215, 0x562221, 0x56222D, 0x562239, 0x562245, 0x562251})
        {
            injector::WriteMemory (ref + 2, &mNextChaosPoints);
        }

    if (m_Config.RandomizeChaos)
        RegisterHooks (
            {{HOOK_CALL, 0x56218E, (void *) RandomizeChaosIncrement}});

    if (m_Config.RandomizeMission)
        RegisterHooks (
            {{HOOK_CALL, 0x4699BF, (void *) RandomizeMissionWantedLevel},
             {HOOK_CALL, 0x4699EC, (void *) RandomizeMissionWantedLevel}});
    Logger::GetLogger ()->LogMessage ("Intialised WantedLevelRandomizer");
}

/*******************************************************/

/*******************************************************/
void
WantedLevelRandomizer::DestroyInstance ()
{
    if (WantedLevelRandomizer::mInstance)
        delete WantedLevelRandomizer::mInstance;
}

/*******************************************************/
WantedLevelRandomizer *
WantedLevelRandomizer::GetInstance ()
{
    if (!WantedLevelRandomizer::mInstance)
        {
            WantedLevelRandomizer::mInstance = new WantedLevelRandomizer ();
            atexit (&WantedLevelRandomizer::DestroyInstance);
        }
    return WantedLevelRandomizer::mInstance;
}
