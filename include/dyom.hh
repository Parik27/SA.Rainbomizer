#pragma once
#include <windows.h>
#include <string>

struct CRunningScript;

class DyomRandomizer
{
    static DyomRandomizer *mInstance;
    
    DyomRandomizer (){};
    static void DestroyInstance ();

    bool ParseMission(HANDLE session, const std::string &url);
    std::string GetRandomEntryFromPage(HANDLE session, std::string page);
    int GetTotalNumberOfDYOMMissionPages(HANDLE session, std::string list);
    void DownloadRandomMission();
    
public:
    
    CRunningScript *       mDyomScript = nullptr;

    void HandleDyomScript(CRunningScript *scr);
    
    static bool mEnabled;
    
    /// Returns the static instance for DyomRandomizer.
    static DyomRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

};
