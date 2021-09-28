#include "cutscenes.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include <vector>
#include "functions.hh"
#include <algorithm>
#include "config.hh"
#include "util/scrpt.hh"
#include "injector/calling.hpp"
#include "util/loader.hh"

CutsceneRandomizer *CutsceneRandomizer::mInstance = nullptr;

static std::string model = "";

std::unordered_map<std::string, float> cutsceneOffsetCorrections = {
    {"BCESA4W", -1.00166f},  {"BCESAR5", -1.00166f},  {"BCESA5W", -0.967539f},
    {"BCESAR4", -0.967539f}, {"BCESAR2", -0.656397f}, {"BCRAS1", -1.006541f},
    {"BCRAS2", -1.006541f},  {"BHILL1", -0.99485f},   {"CAS_2", -0.99485f},
    {"CAS_3", -0.99485f},    {"BHILL2", -0.99799f},   {"BHILL3A", -0.99799f},
    {"BHILL5A", -0.99799f},  {"RIOT_1A", -2.62299f},  {"RIOT_1B", -2.62299f},
    {"BHILL3B", -1.07481f},  {"BHILL3C", -0.97375f},  {"BHILL5B", -1.003971f},
    {"CAS_1A", -0.99785f},   {"CAS_7B", -0.99785f},   {"CAS_4A", -0.99831f},
    {"CAS_4B", 0.438f},      {"CAS_4C", -0.997956f},  {"CAS_6A", -0.997956f},
    {"CAS_9A1", -0.997956f}, {"CAS_9A2", -0.997956f}, {"CAS_11A", -0.997956f},
    {"CAS_5A", -0.997956f},  {"CAS6B_1", -1.00998f},  {"CAS6B_2", -1.00998f},
    {"CAT_1", -1.002811f},   {"CAT_2", -0.999608f},   {"CAT_3", -0.999311f},
    {"CAT_4", -0.99931f},    {"CESAR1A", -0.83088f},  {"CRASH1A", -0.997f},
    {"CRASH3A", -0.76656f},  {"CRASHV1", -0.99031f},  {"CRASV2A", -0.98669f},
    {"CRASV2B", -0.98669f},  {"D8_ALT", -1.17456f},   {"D10_ALT", -1.1857f},
    {"DATE1A", 1.012f},      {"DATE2A", 1.008f},      {"DATE4B", 1.008f},
    {"DATE3A", 0.9753f},     {"DATE3B", 0.9753f},     {"DATE4A", 0.9657f},
    {"DATE4B", 0.9657f},     {"DATE5A", 0.9782f},     {"DATE5B", 0.9782f},
    {"DES_10A", -1.03788f},  {"DES_10B", -1.18231f},  {"DESERT1", 0.37718f},
    {"DESERT2", 0.37718f},   {"DESERT3", -1.00381f},  {"DESERT4", -1.00381f},
    {"DESERT6", -1.1129f},   {"DESERT8", -1.08438f},  {"DESERT9", -1.05185f},
    {"DOC_2", -0.99831f},    {"EPILOG", -0.98199f},   {"FARL_2A", -1.002892f},
    {"FARL_3A", -1.002892f}, {"SYND_2B", -1.002892f}, {"SYND_4B", -1.002892f},
    {"FARL_3B", -1.00414f},  {"FARL_4A", -6.213995f}, {"FARL_5A", -6.213995f},
    {"W2_ALT", -6.213995f},  {"WOOZIE2", -6.213995f}, {"WOOZIE4", -6.213995f},
    {"FINAL1A", -0.999f},    {"FINAL2A", -0.999f},    {"GROVE2", -0.999f},
    {"RIOT_2", -0.999f},     {"RIOT_4A", -0.999f},    {"SWEET1B", -0.999f},
    {"SWEET3A", -0.999f},    {"SWEET5A", -0.999f},    {"SWEET6A", -0.999f},
    {"FINAL2B", -0.99908f},  {"GARAG1B", -1.00513f},  {"GARAG1C", -1.004931f},
    {"GARAG3A", -1.004931f}, {"SCRASH1", -1.004931f}, {"STEAL_1", -1.004931f},
    {"STEAL_5", -1.004931f}, {"SYND_2A", -1.004931f}, {"SYND_4A", -1.004931f},
    {"SYND_7", -1.004931f},  {"GROVE1A", -0.98199f},  {"INTRO1A", -0.98199f},
    {"GROVE1B", -1.006973f}, {"SWEET1C", -1.006973f}, {"GROVE1C", -1.029987f},
    {"HEIST1A", -0.898811f}, {"HEIST2A", -0.898811f}, {"HEIST4A", -0.898811f},
    {"HEIST5A", -0.898811f}, {"HEIST6A", -0.898811f}, {"HEIST8A", -0.99831f},
    {"INTRO1B", -0.44917f},  {"INTRO2A", -0.998f},    {"PROLOG3", -0.76281f},
    {"RIOT_4B", -1.414902f}, {"RIOT_4C", -8.705941f}, {"RIOT_4D", -8.77601f},
    {"RIOT4E1", -0.73718f},  {"RIOT4E2", -0.73718f},  {"RYDER1A", -1.00637f},
    {"RYDER2A", -1.00452f},  {"RYDER3A", -0.99799f},  {"SCRASH2", -1.00493f},
    {"STEAL_4", -1.00493f},  {"SMOKE1A", -0.19688f},  {"SMOKE2A", -0.19688f},
    {"SMOKE3A", -0.19688f},  {"SMOKE4A", -0.19688f},  {"SMOKE1B", -0.77281f},
    {"SMOKE2B", -0.999379f}, {"STEAL_2", -1.005131f}, {"STRAP1A", -0.98281f},
    {"STRAP4A", -0.98281f},  {"STRAP2A", -0.998f},    {"STRAP3A", -0.98798f},
    {"STRP4B1", -1.799985f}, {"STRP4B2", -0.95935f},  {"SWEET1A", -1.00713f},
    {"SWEET2A", 0.28891f},   {"SWEET4A", 0.28891f},   {"SWEET2B", -0.83281f},
    {"SWEET3B", -1.01588f},  {"SWEET6B", -1.2257f},   {"SWEET7A", -0.98118f},
    {"SYND_3A", -0.951688f}, {"TRUTH_1", -0.999017f}, {"TRUTH_2", -1.008189f},
    {"WOOZI1A", -0.994995f}, {"WOOZI1B", -6.213995f}, {"ZERO_1", -0.993956f},
    {"ZERO_2", -0.996642f},  {"ZERO_4", -0.996642F},  {"PROLOG1", 0.0000000f}};

/*******************************************************/
char *
RandomizeCutsceneObject (char *dst, char *src)
{
    char *ret = CallAndReturn<char *, 0x82244B> (dst, src);
    return CutsceneRandomizer::GetInstance ()->GetRandomModel (ret);
}

/*******************************************************/
int
LoadModelForCutscene (std::string name)
{
    // Set ms_cutsceneProcessing to 0 before loading.
    // The game doesn't load the models without this for some reason
    injector::WriteMemory<uint8_t> (0xB5F852, 0);
    int ret = 1;

    short modelIndex = 0;

    if (CModelInfo::GetModelInfo (name.c_str (), &modelIndex)
        && StreamingManager::AttemptToLoadVehicle (modelIndex) == ERR_FAILED)
        {
            Logger::GetLogger ()->LogMessage ("Failed to load Cutscene Model: "
                                              + name);
            ret = 0;
        }

    injector::WriteMemory<uint8_t> (0xB5F852, 1);
    return ret;
}

/*******************************************************/
char *
CutsceneRandomizer::GetRandomModel (std::string model)
{
    std::transform (std::begin (model), std::end (model), model.begin (),
                    [] (unsigned char c) { return std::tolower (c); });
    mLastModel = model;

    for (auto i : mModels)
        {
            if (std::find (std::begin (i), std::end (i), model) != std::end (i))
                {
                    auto replaced = i[random (i.size () - 1)];
                    mLastModel    = replaced;
                    break;
                }
        }

    if (!LoadModelForCutscene (mLastModel))
        mLastModel = model;

    return (char *) mLastModel.c_str ();
}

/*******************************************************/
void
SelectCutsceneOffset (const char *name)
{
    auto cutsceneRandomizer = CutsceneRandomizer::GetInstance ();

    COffset offset;
    offset.z = -100.0f;
    while (offset.z < -70.0f)
        {
            offset.x = randomFloat (-3000, 3000);
            offset.y = randomFloat (-3000, 3000);

            Scrpt::CallOpcode (0x4E4, "refresh_game_renderer", offset.x,
                               offset.y);
            Scrpt::CallOpcode (0x3CB, "set_render_origin", offset.x, offset.y,
                               20);
            Scrpt::CallOpcode (0x15f, "set_pos", offset.x, offset.y, 20, 0, 0,
                               0);
            Scrpt::CallOpcode (0x4D7, "freeze_player", GlobalVar (3), 1);

            offset.z = CWorld::FindGroundZedForCoord (offset.x, offset.y);
        }

    offset.z -= cutsceneOffsetCorrections[name];

    cutsceneRandomizer->offset = offset;
}

/*******************************************************/
void
LoadCutsceneHook (const char *name)
{
    auto cutsceneRandomizer = CutsceneRandomizer::GetInstance ();

    if (cutsceneRandomizer->m_Config.RandomizeCutsceneToPlay)
        name = GetRandomElement (cutsceneOffsetCorrections).first.c_str ();

    if (cutsceneRandomizer->m_Config.RandomizeLocation)
        SelectCutsceneOffset (name);

    HookManager::CallOriginal<injector::cstd<void (const char *)>, 0x480714> (
        name);
}

/*******************************************************/
void
RandomizeCutsceneOffset (char *Str, char *format, float *x, float *y, float *z)
{

    auto cutsceneRandomizer = CutsceneRandomizer::GetInstance ();

    sscanf (Str, format, x, y, z);

    cutsceneRandomizer->originalLevel = injector::ReadMemory<int> (0xB72914);
    Scrpt::CallOpcode (0x4BB, "select_interior", 0);

    *x = cutsceneRandomizer->offset.x;
    *y = cutsceneRandomizer->offset.y;
    *z = cutsceneRandomizer->offset.z;
}

/*******************************************************/
bool
RestoreCutsceneInterior ()
{
    bool ret = HookManager::CallOriginalAndReturn<injector::cstd<bool ()>,
                                                  0x480761> (false);
    if (ret)
        {
            auto cutsceneRandomizer = CutsceneRandomizer::GetInstance ();
            Scrpt::CallOpcode (0x4BB, "select_interior",
                               cutsceneRandomizer->originalLevel);
            Scrpt::CallOpcode (0x4D7, "freeze_player", GlobalVar (3), 0);
        }
    return ret;
}

/*******************************************************/
void
CutsceneRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "CutsceneRandomizer",
            std::pair ("RandomizeModels", &m_Config.RandomizeModels),
            std::pair ("UseOnlyNormalCutsceneModels", &m_Config.NoBrokenJaws),
            std::pair ("RandomizeLocations", &m_Config.RandomizeLocation),
            std::pair ("RandomizeCutsceneToPlay",
                       &m_Config.RandomizeCutsceneToPlay)))
        return;

    std::string fileName;
    if (m_Config.NoBrokenJaws)
        fileName = "Cutscene_Models2.txt";
    else
        fileName = "Cutscene_Models.txt";

    FILE *modelFile = OpenRainbomizerFile (fileName, "r", "data/");
    if (modelFile && m_Config.RandomizeModels)
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
            RegisterHooks (
                {{HOOK_CALL, 0x5B0B30, (void *) &RandomizeCutsceneObject}});
        }
    else if (!modelFile)
        {
            // Log a message if file wasn't found
            Logger::GetLogger ()->LogMessage (
                "Failed to read file: rainbomizer/data/" + fileName);
            Logger::GetLogger ()->LogMessage (
                "Cutscene models will not be randomized");
        }

    if (m_Config.RandomizeLocation)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x5B0A1F, (void *) &RandomizeCutsceneOffset},
                 {HOOK_CALL, 0x480761, (void *) &RestoreCutsceneInterior}});
            injector::MakeNOP (0x5B09D2, 5);
        }

    if (m_Config.RandomizeLocation || m_Config.RandomizeCutsceneToPlay)
        RegisterHooks ({{HOOK_CALL, 0x480714, (void *) &LoadCutsceneHook}});

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
