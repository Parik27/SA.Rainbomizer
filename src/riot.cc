#include "riot.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/calling.hpp"
#include "config.hh"
#include <chrono>

RiotRandomizer *RiotRandomizer::mInstance = nullptr;

static bool  greenLightCheat             = false;
static char  lightResult                 = 0;
static int   randomTimeInterval          = 0;
static float currentTimeSinceLightChange = 0.0f;
static float timeOfLastChange            = 0.0f;

/*******************************************************/
char
TimeToSwitchLight ()
{
    if (!greenLightCheat && injector::ReadMemory<bool> (0x969130 + 30))
        {
            greenLightCheat             = true;
            lightResult                 = random (0, 2);
            currentTimeSinceLightChange = 0.0f;
            timeOfLastChange            = 0.0f;
            randomTimeInterval          = 0.0f;
        }
    else if (greenLightCheat && !injector::ReadMemory<bool> (0x969130 + 30))
        greenLightCheat = false;
    if (greenLightCheat)
        return lightResult;

    if ((int) currentTimeSinceLightChange >= randomTimeInterval)
        {
            lightResult        = random (0, 2);
            randomTimeInterval = random (100, 6000);
            timeOfLastChange   = clock ();
        }

    currentTimeSinceLightChange = clock () - timeOfLastChange;

    return lightResult;
}

/*******************************************************/
CZone *
CheckNewZone (CVector *point, char checkType)
{
    CZone * currentZone = CallAndReturn<CZone *, 0x572360> (point, checkType);
    CVector playerPos   = FindPlayerCoors ();
    if ((int) playerPos.x != (int) point->x
        && (int) playerPos.y != (int) point->y
        && (int) playerPos.z != (int) point->z)
        return currentZone;

    if (std::string (currentZone->m_szTextKey)
        != std::string (RiotRandomizer::previousZone))
        {
            if (ConfigManager::ReadConfig ("RiotRandomizer")
                && RiotRandomizer::m_Config.RandomizeRiots)
                {
                    // Memory address storing if end-game riots are enabled
                    int gbLARiots = 0xB72958;

                    // Memory address storing if riot cheat is enabled
                    // Checked before randomizing riot and written to in order
                    // to enable it
                    int riotCheatActive = 0x969130 + 69;

                    // Check if riots already enabled separate from randomizer
                    // Doesn't take effect in these cases for simplicity
                    if (!injector::ReadMemory<bool> (gbLARiots)
                        && !injector::ReadMemory<bool> (riotCheatActive))
                        RiotRandomizer::riotModeRandomized = false;

                    if (!injector::ReadMemory<bool> (gbLARiots)
                        && !injector::ReadMemory<bool> (riotCheatActive)
                        && !RiotRandomizer::riotModeRandomized
                        && random (1000) > 985
                        && !ScriptSpace[1870])
                        {
                            RiotRandomizer::riotModeRandomized = true;
                            injector::WriteMemory (riotCheatActive, 1);
                        }
                    else if (!injector::ReadMemory<bool> (gbLARiots)
                             && RiotRandomizer::riotModeRandomized
                             && random (1000) > 600)
                        {
                            RiotRandomizer::riotModeRandomized = false;
                            injector::WriteMemory (riotCheatActive, 0);
                        }
                }
        }

    for (int i = 0; i < 8; i++)
        {
            RiotRandomizer::previousZone[i] = currentZone->m_szTextKey[i];
        }
    return currentZone;
}

/*******************************************************/
void
RiotRandomizer::Initialise ()
{
    RegisterHooks ({{HOOK_CALL, 0x572407, (void *) &CheckNewZone}});

    if (!ConfigManager::ReadConfig (
            "RiotRandomizer",
            std::pair ("RandomizeRiots", &m_Config.RandomizeRiots),
            std::pair ("RandomizeTrafficLights",
                       &m_Config.RandomizeTrafficLights)))
        return;

    if (m_Config.RandomizeTrafficLights)
        {
            for (int address : {0x49DB6D, 0x49DB5F, 0x49D6EC, 0x49D862,
                                0x49D9E3, 0x49D6D4, 0x49D84A, 0x49D9CD})
                {
                    injector::MakeCALL (address, (void *) &TimeToSwitchLight);
                }
        }

    Logger::GetLogger ()->LogMessage ("Intialised RiotRandomizer");
}

/*******************************************************/
void
RiotRandomizer::DestroyInstance ()
{
    if (RiotRandomizer::mInstance)
        delete RiotRandomizer::mInstance;
}

/*******************************************************/
RiotRandomizer *
RiotRandomizer::GetInstance ()
{
    if (!RiotRandomizer::mInstance)
        {
            RiotRandomizer::mInstance = new RiotRandomizer ();
            atexit (&RiotRandomizer::DestroyInstance);
        }
    return RiotRandomizer::mInstance;
}
