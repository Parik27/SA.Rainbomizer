#include "blips.hh"
#include <cstdlib>
#include "logger.hh"
#include "injector/calling.hpp"
#include "base.hh"
#include "functions.hh"
#include "config.hh"

BlipRandomizer *BlipRandomizer::mInstance = nullptr;

/*******************************************************/
void
RandomizeBlipSprite (int blipHandle, char spriteId)
{
    spriteId = random (2, 63);
    injector::cstd<void (int, char)>::call (0x583D70, blipHandle, spriteId);
}

/*******************************************************/
void
RandomizeBlipsOnStart ()
{
    injector::cstd<void ()>::call (0x5D53C0);

    int radarTrace = 0xBA86F0 + 0x24;
    for (int i = 0; i < 175; i++)
        {
            if (injector::ReadMemory<uint8_t> (radarTrace) > 0)
                injector::WriteMemory<uint8_t> (radarTrace, random (2, 63));
            radarTrace += 40;
        }
}

/*******************************************************/
void
BlipRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().blips;
    if (!config.enabled)
        return;

    Logger::GetLogger ()->LogMessage ("Intialised BlipRandomizer");
    RegisterHooks ({{HOOK_CALL, 0x5D1948, (void *) RandomizeBlipsOnStart}});
    for (auto i : {0x444403, 0x47F7FE, 0x48BCA8, 0x48DBE1, 0x5775DD})
        injector::MakeCALL (i, (void *) RandomizeBlipSprite);
}

/*******************************************************/
void
BlipRandomizer::DestroyInstance ()
{
    if (BlipRandomizer::mInstance)
        delete BlipRandomizer::mInstance;
}

/*******************************************************/
BlipRandomizer *
BlipRandomizer::GetInstance ()
{
    if (!BlipRandomizer::mInstance)
        {
            BlipRandomizer::mInstance = new BlipRandomizer ();
            atexit (&BlipRandomizer::DestroyInstance);
        }
    return BlipRandomizer::mInstance;
}
