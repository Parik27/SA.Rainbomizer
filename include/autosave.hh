#pragma once

#include <cstdio>

const int ORIGINAL_DATA_SIZE = 5;
class AutoSave
{
    static AutoSave *mInstance;

    bool m_shouldSave = false;

    char m_original_data[ORIGINAL_DATA_SIZE];

    float mDrawPosX = -1;
    float mDrawPosY = -1;

    float mDisplayDrawPosX = 0;
    float mDisplayDrawPosY = 0;

    int mLastSave = 0;

    void DrawMessage (const char *text);
    void HandleTransitions (float &counter, const float &target);

    AutoSave (){};
    static void DestroyInstance ();

public:
    bool mSaveVehicleCoords = false;

    /// Returns the static instance for AutoSave.
    static AutoSave *GetInstance ();

    /// Draws the autosave message
    void DrawAutosaveMessage ();

    bool
    ShouldSave ()
    {
        return m_shouldSave;
    };

    void SetShouldSave (bool shouldSave);

    /// Initialises Hooks/etc.
    void Initialise ();

    void
    SetDrawXY (float x, float y)
    {
        x = (x == -1) ? mDrawPosX : x;
        y = (y == -1) ? mDrawPosY : y;

        if (this->mDrawPosX == -1 || this->mDrawPosY == -1)
            {
                this->mDisplayDrawPosX = x;
                this->mDisplayDrawPosY = y;
            }

        this->mDrawPosX = x;
        this->mDrawPosY = y;
    }

    void InstallHooks ();
    void RevertHooks ();

    bool IsMissionGlobalVariable (int global_var);
};
