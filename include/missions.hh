#pragma once

struct CRunningScript;

struct CitiesInfo
{
    int  citiesUnlocked;
    bool SFBarriers;
    bool LVBarriers;
};

class MissionRandomizer
{
    static MissionRandomizer *mInstance;

    MissionRandomizer (){};
    static void DestroyInstance ();

    int            mPrevOffset      = 0;
    unsigned char *mOriginalBaseIP  = nullptr;
    bool           mScriptReplaced  = false;
    unsigned char *mTempMissionData = nullptr;
    int *          mLocalVariables  = nullptr;
    CitiesInfo     mCityInfo;

    void ApplyMissionSpecificFixes (unsigned char *data);
    void TeleportPlayerAfterMission ();
    int  GetCorrectedMissionNo ();
    void StoreCityInfo ();
    void RestoreCityInfo (const CitiesInfo &info);

public:
    CRunningScript *mRandomizedScript        = nullptr;
    int             mRandomizedMissionNumber = -1;
    int             mOriginalMissionNumber   = -1;
    bool            mStoreNextMission        = false;

    /// Returns the static instance for MissionRandomizer.
    static MissionRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    /// Checks if the mission script should jump
    bool ShouldJump (CRunningScript *src);

    /// Applies Mission Specific Fixes (start)
    void ApplyMissionStartSpecificFixes (unsigned char *data);

    /// Returns a random mission
    int GetRandomMission (int originalMission);

    /// Jumps the script to the original offset
    void MoveScriptToOriginalOffset (CRunningScript *src);

    /// Unlocks the cities based on the mission number
    /// Why do I bother writing these documentations when all they do it restate
    /// the function name
    void UnlockCitiesBasedOnMissionID (int missionId);
};
