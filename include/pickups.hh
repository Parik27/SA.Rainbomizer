
#pragma once

class PickupsRandomizer
{
    static PickupsRandomizer *mInstance;

    PickupsRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for PickupsRandomizer.
    static PickupsRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
