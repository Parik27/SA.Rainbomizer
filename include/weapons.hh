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

    static inline struct Config
    {
        bool RandomizePlayerWeapons;
    } m_Config;

    int GetRandomWeapon (CPed *ped, int weapon, bool ignoreBuggy);
    int GetRandomPickup (CPed *ped, int weapon, bool ignoreBuggy);

    /// Initialises Hooks/etc.
    void Initialise ();
};
