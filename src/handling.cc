#include "handling.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"

HandlingRandomizer *    HandlingRandomizer::mInstance = nullptr;

/*******************************************************/
template<typename T>
void  RandomizeData(T* data, int len)
{
	for(int i = 0; i < len; i++)
	{
		
		T temp = data[i];
		int swap = random(len - 1);
		
		data[i] = data[swap];
		data[swap] = temp;
	}
}

/*******************************************************/
int __fastcall RandomizeHandling(cHandlingDataMgr *handling, void* edx)
{
	handling->LoadHandlingData();
	RandomizeData(handling->vehicleHandling, 210);
	RandomizeData(handling->bikeHandling, 13);
	RandomizeData(handling->planeHandling, 24);
	RandomizeData(handling->boatHandling, 12);
}

/*******************************************************/
void
HandlingRandomizer::Initialise ()
{
    RegisterHooks ({
			{HOOK_CALL, 0x5BFA9A, (void*) &RandomizeHandling}
		});
	Logger::GetLogger ()->LogMessage (
        "Intialised HandlingRandomizer");
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
		 
