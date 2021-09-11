#include "blips.hh"
#include <cstdlib>
#include "logger.hh"
#include "injector/calling.hpp"
#include "base.hh"
#include "functions.hh"
#include "config.hh"
#include "fades.hh"

BlipRandomizer *BlipRandomizer::mInstance = nullptr;
static int      northIcon                 = random (2, 63);

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
    northIcon = random (2, 63);
}

/*******************************************************/
void
FixLegendCrash (float x, float y, char *text)
{
    if (int (text) < 0x2000)
        text = (char *) "Boohoo";

    HookManager::CallOriginal<injector::cstd<void (float, float, char *)>,
                              0x582DEE> (x, y, text);
}

/*******************************************************/
void
RandomizeNorthIcon (int iconID, float x, float y, uint8_t alpha)
{
    iconID = northIcon;
    injector::cstd<void (int, float, float, uint8_t)>::call (0x585FF0, iconID,
                                                             x, y, alpha);
}

/*******************************************************/
int
DisplayBlipsInInteriors (int a1, char a2)
{
    return 1;
}

/*******************************************************/
void
BlipRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("BlipRandomizer"))
        return;

    Logger::GetLogger ()->LogMessage ("Intialised BlipRandomizer");
    RegisterHooks ({
        {HOOK_CALL, 0x5D1948, (void *) RandomizeBlipsOnStart},
        {HOOK_CALL, 0x582DEE, (void *) FixLegendCrash},
        {HOOK_CALL, 0x588188, (void *) RandomizeNorthIcon},
        {HOOK_JUMP, 0x583B40, (void *) DisplayBlipsInInteriors},
    });
    for (auto i : {0x444403, 0x47F7FE, 0x48BCA8, 0x48DBE1, 0x5775DD})
        injector::MakeCALL (i, (void *) RandomizeBlipSprite);

    FadesManager::AddFadeCallback ([] { northIcon = random (2, 63); });
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
