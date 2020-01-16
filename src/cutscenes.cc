#include "cutscenes.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include <vector>
#include "functions.hh"
#include <algorithm>
#include "config.hh"

CutsceneRandomizer *CutsceneRandomizer::mInstance = nullptr;

/*******************************************************/
char *
RandomizeCutsceneObject_2 (char *dst, char *src)
{
    char *ret = CallAndReturn<char *, 0x82244B> (dst, src);
    return (char *) CutsceneRandomizer::GetInstance ()->GetRandomModel (ret);
}

/*******************************************************/
const char *
CutsceneRandomizer::GetRandomModel (std::string model)
{
    std::transform (std::begin (model), std::end (model), model.begin (),
                    [] (unsigned char c) { return std::tolower (c); });
    std::string logMessage = model;

    for (auto i : mModels)
        {
            if (std::find (std::begin (i), std::end (i), model) != std::end (i))
                {
                    auto replaced = i[random (i.size () - 1)];
                    logMessage += " -> " + replaced;

                    Logger::GetLogger ()->LogMessage (logMessage);
                    return replaced.c_str ();
                }
        }
    return model.c_str ();
}

/*******************************************************/
void
CutsceneRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().cutscenes;
    if (!config.enabled)
        return;

    FILE *modelFile = fopen (config.cutsceneFile.c_str (), "r");
    if (modelFile)
        {
            char line[512] = {0};
            mModels.push_back (std::vector<std::string> ());
            while (fgets (line, 512, modelFile))
                {
                    if (strlen (line) < 2)
                        {
                            mModels.push_back (std::vector<std::string> ());
                            continue;
                        }
                    line[strcspn (line, "\n")] = 0;
                    mModels.back ().push_back (line);
                }
        }
    else
        return;

    RegisterHooks (
        {{HOOK_CALL, 0x5B0B30, (void *) &RandomizeCutsceneObject_2}});
    Logger::GetLogger ()->LogMessage ("Intialised CutsceneRandomizer");
}

/*******************************************************/
void
CutsceneRandomizer::DestroyInstance ()
{
    if (CutsceneRandomizer::mInstance)
        delete CutsceneRandomizer::mInstance;
}

/*******************************************************/
CutsceneRandomizer *
CutsceneRandomizer::GetInstance ()
{
    if (!CutsceneRandomizer::mInstance)
        {
            CutsceneRandomizer::mInstance = new CutsceneRandomizer ();
            atexit (&CutsceneRandomizer::DestroyInstance);
        }
    return CutsceneRandomizer::mInstance;
}
