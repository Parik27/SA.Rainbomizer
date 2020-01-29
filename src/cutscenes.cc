#include "cutscenes.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include <vector>
#include "functions.hh"
#include <algorithm>
#include "config.hh"
#include "scrpt.hh"
#include "injector/calling.hpp"

CutsceneRandomizer *CutsceneRandomizer::mInstance = nullptr;

static std::string model = "";

/*******************************************************/
char *
RandomizeCutsceneObject (char *dst, char *src)
{
    char *ret = CallAndReturn<char *, 0x82244B> (dst, src);
    return CutsceneRandomizer::GetInstance ()->GetRandomModel (ret);
}

/*******************************************************/
char *
CutsceneRandomizer::GetRandomModel (std::string model)
{
    std::transform (std::begin (model), std::end (model), model.begin (),
                    [] (unsigned char c) { return std::tolower (c); });
    std::string logMessage = model;
    mLastModel             = model;

    for (auto i : mModels)
        {
            if (std::find (std::begin (i), std::end (i), model) != std::end (i))
                {
                    auto replaced = i[random (i.size () - 1)];
                    logMessage += " -> " + replaced;

                    Logger::GetLogger ()->LogMessage (logMessage);
                    mLastModel = replaced;
                }
        }
    return (char *) mLastModel.c_str ();
}

/*******************************************************/
void
RandomizeCutsceneOffset (char *Str, char *format, float *x, float *y, float *z)
{

    auto cutsceneRandomizer = CutsceneRandomizer::GetInstance ();

    sscanf (Str, format, x, y, z);

    *x = randomFloat(-3000, 3000);
    *y = randomFloat(-3000, 3000);
    *z = CWorld::FindGroundZedForCoord(*x, *y) - 17.582066;
    
    cutsceneRandomizer->originalLevel = injector::ReadMemory<int> (0x48B99C);

    Scrpt::CallOpcode (0x4BB, "select_interior", 0);
}

/*******************************************************/
void
RestoreCutsceneInterior ()
{
    auto cutsceneRandomizer = CutsceneRandomizer::GetInstance ();
    Scrpt::CallOpcode (0x4BB, "select_interior",
                       cutsceneRandomizer->originalLevel);
    HookManager::CallOriginal<injector::cstd<void ()>, 0x48078A> ();
}

/*******************************************************/
void
CutsceneRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().cutscenes;
    if (!config.enabled)
        return;

    FILE *modelFile = OpenRainbomizerFile (config.cutsceneFile, "r");
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

    RegisterHooks ({
        {HOOK_CALL, 0x5B0B30, (void *) &RandomizeCutsceneObject},
        {HOOK_CALL, 0x5B0A1F, (void *) &RandomizeCutsceneOffset},
        //{HOOK_CALL, 0x48078A, (void *) &RestoreCutsceneInterior}
    });
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
