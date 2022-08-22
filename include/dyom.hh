#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct CRunningScript;

class DyomRandomizer
{
    static DyomRandomizer *mInstance;

    DyomRandomizer (){};
    static void DestroyInstance ();

    bool        ParseMission (HANDLE session, const std::string &url);
    bool        TranslateMission (HANDLE session);
    std::string TranslateText (HANDLE session, const std::string &text);
    std::string GetRandomEntryFromPage (HANDLE session, std::string page);
    int  GetTotalNumberOfDYOMMissionPages (HANDLE session, std::string list);
    void DownloadRandomMission ();

    std::vector<std::string> mTranslationChain;

public:
    static inline struct Config
    {
        bool EnglishOnly;
        bool Translate;
        std::string TranslationChain;
    } m_Config;

    CRunningScript *mDyomScript = nullptr;

    void HandleAutoplay (CRunningScript* scr);
    void HandleDyomScript (CRunningScript *scr);

    static bool mEnabled;

    /// Returns the static instance for DyomRandomizer.
    static DyomRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
