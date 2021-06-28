
#pragma once
#include <vector>
#include <string>

class PickupsRandomizer
{
    static PickupsRandomizer *mInstance;

    PickupsRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for PickupsRandomizer.
    static PickupsRandomizer *GetInstance ();

    static inline struct Config
    {
        bool RandomizeDeadPed;
        bool ReplaceWithWeaponsOnly;
        bool MoneyFromPickups;

        std::string PickupsSeed;
        int         PickupsSeedHash = 0;

        bool SkipChecks;
    } m_Config;

    /// Initialises Hooks/etc.
    void Initialise ();
};
