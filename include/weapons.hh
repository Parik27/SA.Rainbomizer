#pragma once

class WeaponRandomizer
{
    static WeaponRandomizer *    mInstance;

    WeaponRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for WeaponRandomizer.
    static WeaponRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

};
