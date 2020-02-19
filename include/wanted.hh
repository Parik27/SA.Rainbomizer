#pragma once

class WantedLevelRandomizer
{
    static WantedLevelRandomizer *mInstance;

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
