#pragma once

struct CPed;

int *GetWeaponInfo (int weaponId, char skill);

class WeaponRandomizer
{
    static WeaponRandomizer *mInstance;

    WeaponRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for WeaponRandomizer.
    static WeaponRandomizer *GetInstance ();

    int GetRandomWeapon (CPed *ped, int weapon);

    /// Initialises Hooks/etc.
    void Initialise ();
};
