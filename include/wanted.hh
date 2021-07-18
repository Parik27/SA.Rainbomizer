#pragma once

class WantedLevelRandomizer
{
    static WantedLevelRandomizer *mInstance;

    static inline struct Config
    {
        bool RandomizeMission = true;
        bool RandomizeChaos;

        Config () {}
    } m_Config;

    WantedLevelRandomizer (){};
    static void DestroyInstance ();

    static float mNextChaosPoints;

public:
    /// Returns the static instance for WantedLevelRandomizer.
    static WantedLevelRandomizer *GetInstance ();

    /// Randomizes Chaos Points
    void RandomizeChaosPoints ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
