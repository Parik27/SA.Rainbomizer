#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <functional>
#include <cstdint>
#include <string>

struct CRunningScript;

const int OPCODE_REPLACE_MISSION = 0x1096; // custom opcode used in race scripts

struct CitiesInfo
{
    int  citiesUnlocked;
    bool SFBarriers;
    bool LVBarriers;
    int  maxWanted;
};

struct MissionStatus
{
    unsigned char data[102];

    unsigned char &
    operator[] (int index)
    {
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
        if (this == &rhs)
            return *this;

        memcpy (this, &rhs, sizeof (MissionRandomizerSaveStructure));

        return *this;
    }
};

class MissionRandomizer
{
    static MissionRandomizer *mInstance;

    static inline struct Config
    {
        int ForcedMissionID;

        bool        RandomizeOnce = true;
        std::string RandomizeOnceSeed;
        int         MissionSeedHash         = 0;
        bool        ForcedRandomizeOnceSeed = false;

        bool PreserveMomentum    = true;
        bool DisableMainSCMCheck = false;

        Config () {}
    } m_Config;

    MissionRandomizer (){};
    static void DestroyInstance ();

    int            mPrevOffset      = 0;
    unsigned char *mOriginalBaseIP  = nullptr;
    bool           mScriptReplaced  = false;
    unsigned char *mTempMissionData = nullptr;
    int *          mLocalVariables  = nullptr;
    CitiesInfo     mCityInfo;
    bool           mScriptByPass = false;

    MissionRandomizerSaveStructure                mSaveInfo;
    std::unordered_map<int, std::vector<uint8_t>> mShuffledOrder;

    void ApplyMissionSpecificFixes (unsigned char *data);
    void ApplyMissionFailFixes ();
    void TeleportPlayerAfterMission ();
    int  GetCorrectedMissionNo ();
    void StoreCityInfo (CitiesInfo &out);
    void RestoreCityInfo (const CitiesInfo &info);
    int  GetStatusForTwoPartMissions (int index);
    void HandleGoSubAlternativeForMission (int index);

    int GetCorrectedMissionStatusIndex (int index);

    void HandleReturnOpcode (CRunningScript *scr, short opcode);
    void HandleGoSubOpcode (CRunningScript *scr, short &opcode);
    void HandleStoreCarOpcode (CRunningScript *scr, short opcode);
    void HandleReplaceMissionOpcode (CRunningScript *scr, short opcode);
    void HandleEndThreadOpcode (CRunningScript *scr, short opcode);
    void HandleOverrideRestartOpcode (CRunningScript *scr, short opcode);

    bool VerifyMainSCM ();

    void InstallCheat (void *func, uint32_t hash);

public:
    CRunningScript *    mRandomizedScript        = nullptr;
    int                 mRandomizedMissionNumber = -1;
    int                 mOriginalMissionNumber   = -1;
    bool                mStoreNextMission        = false;
    int                 mCurrentCitiesUnlocked   = 0;
    int                 mContinuedMission        = -1;
    int                 mSkipNextMission         = -1;
    std::pair<int, int> mCorrectedMissionStatus  = {-1, -1};
    static inline bool  mKeyPressOpcode          = false;

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

    void
    SetScriptByPass (bool status = true)
    {
        mScriptByPass = status;
    }

    void Load ();
    void Save ();
};
