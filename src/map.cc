#include "map.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "util/scrpt.hh"
#include "config.hh"

MapRandomizer *MapRandomizer::mInstance = nullptr;

    std::unordered_map<int, const char *> tags
    = {{1490, "tag_01"},     {1524, "tag_front"},  {1525, "tag_kilo"},
       {1526, "tag_rifa"},   {1527, "tag_rollin"}, {1528, "tag_seville"},
       {1529, "tag_temple"}, {1530, "tag_vagos"},  {1531, "tag_azteca"}};

/*******************************************************/
CEntity *
RandomizeObjectInstance (CFileObjectInstance *inst, const char *modelName)
{
    if ((inst->modelId >= 1524 && inst->modelId <= 1531)
        || inst->modelId == 1490)
        {
            int randomTag = GetRandomElement (tags).first;
            inst->modelId = randomTag;
            modelName     = tags[randomTag];
        }
    return CallAndReturn<CEntity *, 0x538090> (inst, modelName);
}

/*******************************************************/
void
MapRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("MapRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x5386E3, (void *) RandomizeObjectInstance}});

    Logger::GetLogger ()->LogMessage ("Intialised MapRandomizer");
}

/*******************************************************/
void
MapRandomizer::DestroyInstance ()
{
    if (MapRandomizer::mInstance)
        delete MapRandomizer::mInstance;
}

/*******************************************************/
MapRandomizer *
MapRandomizer::GetInstance ()
{
    if (!MapRandomizer::mInstance)
        {
            MapRandomizer::mInstance = new MapRandomizer ();
            atexit (&MapRandomizer::DestroyInstance);
        }
    return MapRandomizer::mInstance;
}
