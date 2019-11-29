#pragma once

#include <cstdio>

const int ORIGINAL_DATA_SIZE = 5;
class AutoSave
{
    static AutoSave *mInstance;

    bool m_shouldSave = false;

    char m_original_data[ORIGINAL_DATA_SIZE];

    AutoSave (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for AutoSave.
    static AutoSave *GetInstance ();

    bool
    ShouldSave ()
    {
        return m_shouldSave;
    };

    void
    SetShouldSave (bool shouldSave)
    {
        puts (shouldSave ? "Save Scheduled" : "Save Successful");
        m_shouldSave = shouldSave;
    };

    /// Initialises Hooks/etc.
    void Initialise ();

    void InstallHooks ();
    void RevertHooks ();

    bool IsMissionGlobalVariable (int global_var);
};
