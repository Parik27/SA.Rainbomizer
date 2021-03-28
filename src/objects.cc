#include "objects.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "config.hh"

ObjectsRandomizer *ObjectsRandomizer::mInstance = nullptr;

/*******************************************************/
void __fastcall RandomizeObjectIndices (CRunningScript *script, void *edx,
                                        short count)
{
    script->CollectParameters (count);
    //if (ScriptParams[0] == -154)
    //    ScriptParams[0] = -46;
    if (ScriptParams[0] < 0)
        ScriptParams[0] = -random (*((int *) 0xA44B6C));
}

/*******************************************************/
void
ObjectsRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("ObjectRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x469773, (void *) RandomizeObjectIndices}});
    Logger::GetLogger ()->LogMessage ("Intialised ObjectsRandomizer");
}

/*******************************************************/
void
ObjectsRandomizer::DestroyInstance ()
{
    if (ObjectsRandomizer::mInstance)
        delete ObjectsRandomizer::mInstance;
}

/*******************************************************/
ObjectsRandomizer *
ObjectsRandomizer::GetInstance ()
{
    if (!ObjectsRandomizer::mInstance)
        {
            ObjectsRandomizer::mInstance = new ObjectsRandomizer ();
            atexit (&ObjectsRandomizer::DestroyInstance);
        }
    return ObjectsRandomizer::mInstance;
}
