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
    {"bcesa4w", -1.00166f},  {"bcesar5", -1.00166f},  {"bcesa5w", -0.967539f},
    {"bcesar4", -0.967539f}, {"bcesar2", -0.656397f}, {"bcras1", -1.006541f},
    {"bcras2", -1.006541f},  {"bhill1", -0.99485f},   {"cas_2", -0.99485f},
    {"cas_3", -0.99485f},    {"bhill2", -0.99799f},   {"bhill3a", -0.99799f},
    {"bhill5a", -0.99799f},  {"riot_1a", -2.62299f},  {"riot_1b", -2.62299f},
    {"bhill3b", -1.07481f},  {"bhill3c", -0.97375f},  {"bhill5b", -1.003971f},
    {"cas_1a", -0.99785f},   {"cas_7b", -0.99785f},   {"cas_4a", -0.99831f},
    {"cas_4b", 0.438f},      {"cas_4c", -0.997956f},  {"cas_6a", -0.997956f},
    {"cas_9a1", -0.997956f}, {"cas_9a2", -0.997956f}, {"cas_11a", -0.997956f},
    {"cas_5a", -0.997956f},  {"cas6b_1", -1.00998f},  {"cas6b_2", -1.00998f},
    {"cat_1", -1.002811f},   {"cat_2", -0.999608f},   {"cat_3", -0.999311f},
    {"cat_4", -0.99931f},    {"cesar1a", -0.83088f},  {"crash1a", -0.997f},
    {"crash3a", -0.76656f},  {"crashv1", -0.99031f},  {"crasv2a", -0.98669f},
    {"crasv2b", -0.98669f},  {"d8_alt", -1.17456f},   {"d10_alt", -1.1857f},
    {"date1a", 1.012f},      {"date2a", 1.008f},      {"date4b", 1.008f},
    {"date3a", 0.9753f},     {"date3b", 0.9753f},     {"date4a", 0.9657f},
    {"date4b", 0.9657f},     {"date5a", 0.9782f},     {"date5b", 0.9782f},
    {"des_10a", -1.03788f},  {"des_10b", -1.18231f},  {"desert1", 0.37718f},
    {"desert2", 0.37718f},   {"desert3", -1.00381f},  {"desert4", -1.00381f},
    {"desert6", -1.1129f},   {"desert8", -1.08438f},  {"desert9", -1.05185f},
    {"doc_2", -0.99831f},    {"epilog", -0.98199f},   {"farl_2a", -1.002892f},
    {"farl_3a", -1.002892f}, {"synd_2b", -1.002892f}, {"synd_4b", -1.002892f},
    {"farl_3b", -1.00414f},  {"farl_4a", -6.213995f}, {"farl_5a", -6.213995f},
    {"w2_alt", -6.213995f},  {"woozie2", -6.213995f}, {"woozie4", -6.213995f},
    {"final1a", -0.999f},    {"final2a", -0.999f},    {"grove2", -0.999f},
    {"riot_2", -0.999f},     {"riot_4a", -0.999f},    {"sweet1b", -0.999f},
    {"sweet3a", -0.999f},    {"sweet5a", -0.999f},    {"sweet6a", -0.999f},
    {"final2b", -0.99908f},  {"garag1b", -1.00513f},  {"garag1c", -1.004931f},
    {"garag3a", -1.004931f}, {"scrash1", -1.004931f}, {"steal_1", -1.004931f},
    {"steal_5", -1.004931f}, {"synd_2a", -1.004931f}, {"synd_4a", -1.004931f},
    {"synd_7", -1.004931f},  {"grove1a", -0.98199f},  {"intro1a", -0.98199f},
    {"grove1b", -1.006973f}, {"sweet1c", -1.006973f}, {"grove1c", -1.029987f},
    {"heist1a", -0.898811f}, {"heist2a", -0.898811f}, {"heist4a", -0.898811f},
    {"heist5a", -0.898811f}, {"heist6a", -0.898811f}, {"heist8a", -0.99831f},
    {"intro1b", -0.44917f},  {"intro2a", -0.998f},    {"prolog3", -0.76281f},
    {"riot_4b", -1.414902f}, {"riot_4c", -8.705941f},  {"riot_4d", -8.77601f},
    {"riot4e1", -0.73718f},  {"riot4e2", -0.73718f},  {"ryder1a", -1.00637f},
    {"ryder2a", -1.00452f},  {"ryder3a", -0.99799f},  {"scrash2", -1.00493f},
    {"steal_4", -1.00493f},  {"smoke1a", -0.19688f},  {"smoke2a", -0.19688f},
    {"smoke3a", -0.19688f},  {"smoke4a", -0.19688f},  {"smoke1b", -0.77281f},
    {"smoke2b", -0.999379f}, {"steal_2", -1.005131f}, {"strap1a", -0.98281f},
    {"strap4a", -0.98281f},  {"strap2a", -0.998f},    {"strap3a", -0.98798f},
    {"strp4b1", -1.799985f}, {"strp4b2", -0.95935f},  {"sweet1a", -1.00713f},
    {"sweet2a", 0.28891f},   {"sweet4a", 0.28891f},   {"sweet2b", -0.83281f},
    {"sweet3b", -1.01588f},  {"sweet6b", -1.2257f},   {"sweet7a", -0.98118f},
    {"synd_3a", -0.951688f}, {"truth_1", -0.999017f}, {"truth_2", -1.008189f},
    {"woozi1a", -0.994995f}, {"woozi1b", -6.213995f}, {"zero_1", -0.993956f},
    {"zero_2", -0.996642f},  {"zero_4", -0.996642f}};

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
SelectCutsceneOffset (char *name)
{
    auto    cutsceneRandomizer = CutsceneRandomizer::GetInstance ();
    COffset offset;

    offset.x = randomFloat (-3000, 3000);
    offset.y = randomFloat (-3000, 3000);

    Scrpt::CallOpcode (0x4E4, "refresh_game_renderer", offset.x, offset.y);
    Scrpt::CallOpcode (0x3CB, "set_render_origin", offset.x, offset.y, 20);
    Scrpt::CallOpcode (0x15f, "set_pos", offset.x, offset.y, 20, 0, 0, 0);
    Scrpt::CallOpcode (0x4D7, "freeze_player", GlobalVar (3), 1);

    offset.z = CWorld::FindGroundZedForCoord (offset.x, offset.y);

    for (auto &it : cutsceneOffsetCorrections)
    {
        std::string cutsceneName = it.first;
        transform (cutsceneName.begin (), cutsceneName.end (), cutsceneName.begin (), ::toupper);
        if (cutsceneName == std::string(name))
        {
            offset.z -= it.second;
        }
    }

    cutsceneRandomizer->offset = offset;

    HookManager::CallOriginal<injector::cstd<void (char *)>, 0x480714> (name);
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
    if (!ConfigManager::ReadConfig ("CutsceneRandomizer",
            std::pair ("RandomizeModels", &m_Config.RandomizeModels),
            std::pair ("RandomizeLocations", &m_Config.RandomizeLocation)))
        return;

    FILE *modelFile = OpenRainbomizerFile ("Cutscene_Models.txt", "r", "data/");
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
                "Failed to read file: rainbomizer/data/Cutscene_Models.txt");
            Logger::GetLogger ()->LogMessage (
                "Cutscene models will not be randomized");
        }

    if (m_Config.RandomizeLocation)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x5B0A1F, (void *) &RandomizeCutsceneOffset},
                 {HOOK_CALL, 0x480761, (void *) &RestoreCutsceneInterior},
                 {HOOK_CALL, 0x480714, (void *) &SelectCutsceneOffset}});
            injector::MakeNOP (0x5B09D2, 5);
        }

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
