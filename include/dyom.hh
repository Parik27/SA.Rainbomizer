#pragma once
#include <vector>
#include <windows.h>
#include <string>
#include "util/dyom/Internet.hh"

struct CRunningScript;

class DyomRandomizer
{
    static DyomRandomizer *mInstance;

    DyomRandomizer (){};
    static void DestroyInstance ();

    InternetUtils internet;

    void        SaveMission (const std::vector<uint8_t> &data);
    bool        ParseMission (const std::string &url);
    
    std::string GetRandomEntryFromPage (std::string page);
    int         GetTotalNumberOfDYOMMissionPages (std::string list);
    void        DownloadRandomMission ();

    int         prevObjectiveForSubtitles = -1;
    bool        enableExternalSubtitles = false;
    std::string storedObjectives[100];
    std::string originalName;

    bool inhibitEditorScript = false;

public:
    static inline struct Config
    {
        bool        EnglishOnly;
        bool        AutoTranslateToEnglish;
        bool        EnableTextToSpeech;
        bool        RandomSpawn;
        std::string TranslationChain;
        std::string CharactersMap;
        double      OverrideTTSVolume;
    } m_Config;

    CRunningScript *mDyomScript = nullptr;

    void HandleExternalSubtitles ();
    void HandleAutoplay (CRunningScript* scr);

    void HandleScript (CRunningScript *scr);
    void HandleDyomScript (CRunningScript *scr);

    static bool mEnabled;

    /// Returns the static instance for DyomRandomizer.
    static DyomRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
