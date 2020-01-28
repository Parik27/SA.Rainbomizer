#pragma once

#include <unordered_map>
#include <vector>
#include <cstring>
#include <functional>

struct CRunningScript;

struct CitiesInfo
{
    int  citiesUnlocked;
    bool SFBarriers;
    bool LVBarriers;
};

struct MissionStatus
{
    unsigned char data[102];

    unsigned char &operator[] (int index) { return data[index - 11]; }
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

struct MissionCleanup
{
    bool                   onMissionPassed;
    bool                   onMissionFailed;
    std::function<bool ()> condition;
    std::function<void ()> cleanup;
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
    std::vector<MissionCleanup>                   mMissionCleanups;

    void ApplyMissionSpecificFixes (unsigned char *data);
    void TeleportPlayerAfterMission ();
    int  GetCorrectedMissionNo ();
    void StoreCityInfo (CitiesInfo &out);
    void RestoreCityInfo (const CitiesInfo &info);
    int  GetStatusForTwoPartMissions (int index);
    void HandleGoSubAlternativeForMission (int index);

    int GetCorrectedMissionStatusIndex (int index);

public:
    CRunningScript *    mRandomizedScript        = nullptr;
    int                 mRandomizedMissionNumber = -1;
    int                 mOriginalMissionNumber   = -1;
    bool                mStoreNextMission        = false;
    int                 mCurrentCitiesUnlocked   = 0;
    int                 mContinuedMission        = -1;
    int                 mSkipNextMission         = -1;
    std::pair<int, int> mCorrectedMissionStatus  = {-1, -1};

    /// Returns the static instance for MissionRandomizer.
    static MissionRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    /// Checks if the mission script should jump
    bool ShouldJump (CRunningScript *src);

    /// Applies Mission Specific Fixes (start)
    void ApplyMissionStartSpecificFixes (unsigned char *data);

    /// Sets continued mission
    void
    SetContinuedMission (int mission)
    {
        this->mContinuedMission = mission;
    }

    /// Sets the randomizer to skip the next mission with this index
    void
    SetSkippedMission (int mission)
    {
        this->mSkipNextMission = mission;
    }

    /// Sets the randomizer to use a different mission status
    void
    SetCorrectedMissionStatusIndex (int mission, int newStatus)
    {
        this->mCorrectedMissionStatus = {mission, newStatus};
    }

    /// Returns a random mission
    int GetRandomMission (int originalMission);

    /// Jumps the script to the original offset
    void MoveScriptToOriginalOffset (CRunningScript *src);

    /// Unlocks the cities based on the mission number
    void UnlockCitiesBasedOnMissionID (int missionId);
    void SetGangTerritoriesForMission (int index);
    void SetRiotModeForMission (int index);

    void TeleportPlayerBeforeMission ();

    /// Reset save data
    void ResetSaveData ();
    void InitShuffledMissionOrder ();

    /// Add to mission cleanup
    void
    AddToMissionCleanup (MissionCleanup cleanup)
    {
        mMissionCleanups.push_back (cleanup);
    }

    /// Add to mission cleanup
    void
    AddToMissionCleanup (std::function<void ()> function,
                         bool                   onMissionPassed = true,
                         bool                   onMissionFailed = true,
                         std::function<bool ()> condition       = nullptr)
    {
        mMissionCleanups.push_back (
            {onMissionPassed, onMissionFailed, condition, function});
    }

    void
    ClearMissionCleanup ()
    {
        mMissionCleanups.clear ();
    }

    void Load ();
    void Save ();
};
