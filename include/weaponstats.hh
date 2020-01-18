#pragma once

class WeaponStatsRandomizer
{
	static WeaponStatsRandomizer *mInstance;

	WeaponStatsRandomizer(){}

	static void DestroyInstance();

public:
	static WeaponStatsRandomizer *GetInstance();

	int RandomizeWeaponStats(int weaponId);

	void Initialise();
};

