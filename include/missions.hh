#pragma once

#include <unordered_map>
#include <vector>
#include <cstring>

struct CRunningScript;

struct CitiesInfo
{
    int  citiesUnlocked;
    bool SFBarriers;
    bool LVBarriers;
};

struct MissionStatus
{
    unsigned char data[101];

    unsigned char &operator[] (int index)
    {
        printf ("Accessing index: %d\n", index);
        return data[index - 11];
    }
};

struct MissionRandomizerSaveStructure
{
    char          signature[12] = "RAINBOMIZER";
    unsigned int  randomSeed;
    MissionStatus missionStatus;

    MissionRandomizerSaveStructure &
    operator= (const MissionRandomizerSaveStructure &rhs)
    {
        // Check for self-assignment!
        if (this == &rhs)
            return *this;

        memcpy (this, &rhs, sizeof (MissionRandomizerSaveStructure));

        return *this;
    }
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

    MissionRandomizerSaveStructure                mSaveInfo;
    std::unordered_map<int, std::vector<uint8_t>> mShuffledOrder;

    void ApplyMissionSpecificFixes (unsigned char *data);
    void TeleportPlayerAfterMission ();
    int  GetCorrectedMissionNo ();
    void StoreCityInfo ();
    void RestoreCityInfo (const CitiesInfo &info);
    int  GetStatusForTwoPartMissions (int index);

public:
    CRunningScript *mRandomizedScript        = nullptr;
    int             mRandomizedMissionNumber = -1;
    int             mOriginalMissionNumber   = -1;
    int             mSkipMissionNumber       = 0;
    bool            mStoreNextMission        = false;
    int             mCurrentCitiesUnlocked   = 0;

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

    /// Reset save data
    void ResetSaveData ();
    void InitShuffledMissionOrder ();

    void Load ();
    void Save ();
};
