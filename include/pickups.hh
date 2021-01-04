
#pragma once
#include <vector>

class PickupsRandomizer
{
    static PickupsRandomizer *mInstance;

    PickupsRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for PickupsRandomizer.
    static PickupsRandomizer *GetInstance ();

	static std::vector<int> additional_pickups;

    /// Initialises Hooks/etc.
    void Initialise ();
};
