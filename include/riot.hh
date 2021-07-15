#pragma once

class RiotRandomizer
{
    static RiotRandomizer *mInstance;

    RiotRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for RiotRandomizer.
    static RiotRandomizer *GetInstance ();

    static inline struct Config
    {
        bool RandomizeRiots;
        bool RandomizeTrafficLights;
    } m_Config;

    inline static char previousZone[8];
    inline static bool riotModeRandomized = false;

    /// Initialises Hooks/etc.
    void Initialise ();
};
