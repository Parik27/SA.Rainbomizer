#pragma once

struct CRunningScript;

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

    /// Jumps the script to the original offset
    void MoveScriptToOriginalOffset (CRunningScript *src);
};
