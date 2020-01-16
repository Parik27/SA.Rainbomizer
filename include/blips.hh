#pragma once

class BlipRandomizer
{
    static BlipRandomizer *mInstance;

    BlipRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for BlipRandomizer.
    static BlipRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();
};
