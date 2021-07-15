#pragma once

class GeneralSettings
{
    static GeneralSettings *mInstance;

    GeneralSettings (){};
    static void DestroyInstance ();

public:
    static GeneralSettings *GetInstance ();

    static inline struct Config
    {
        int  Seed          = -1;
        bool Unprotect     = true;
        int  AutoSaveSlot  = 8;
        bool ModifyCredits = true;

        Config () {}
    } m_Config;

    /// Initialises Hooks/etc.
    void Initialise ();
};
