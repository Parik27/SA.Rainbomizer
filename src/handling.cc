#include "handling.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"

HandlingRandomizer *HandlingRandomizer::mInstance = nullptr;

/*******************************************************/
int __fastcall RandomizeHandling (cHandlingDataMgr *handling, void *edx)
{
    handling->LoadHandlingData ();
    for (int i = 0; i < 210; i++)
        {
            auto handl                    = handling->vehicleHandling[i];
            int  id                       = random (210 - 1);
            handling->vehicleHandling[i]  = handling->vehicleHandling[id];
            handling->vehicleHandling[id] = handl;
        }
    // RandomizeData(handling->vehicleHandling, 210);
    // RandomizeData(handling->bikeHandling, 13);
    // RandomizeData(handling->planeHandling, 24);
    // RandomizeData(handling->boatHandling, 12);
    return 0;
}

/*******************************************************/
void
HandlingRandomizer::Initialise ()
{
    RegisterHooks ({{HOOK_CALL, 0x5BFA9A, (void *) &RandomizeHandling}});
    Logger::GetLogger ()->LogMessage ("Intialised HandlingRandomizer");
}

/*******************************************************/
void
HandlingRandomizer::DestroyInstance ()
{
    if (HandlingRandomizer::mInstance)
        delete HandlingRandomizer::mInstance;
}

/*******************************************************/
HandlingRandomizer *
HandlingRandomizer::GetInstance ()
{
    if (!HandlingRandomizer::mInstance)
        {
            HandlingRandomizer::mInstance = new HandlingRandomizer ();
            atexit (&HandlingRandomizer::DestroyInstance);
        }
    return HandlingRandomizer::mInstance;
}
