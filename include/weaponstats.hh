#pragma once

class WeaponStatsRandomizer
{
    static WeaponStatsRandomizer *mInstance;

    WeaponStatsRandomizer () {}

    static void DestroyInstance ();

public:
    static WeaponStatsRandomizer *GetInstance ();

    void Initialise ();
};
