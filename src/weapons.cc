/*
    Rainbomizer - A (probably fun) Grand Theft Auto San Andreas Mod that
                  randomizes stuff
    Copyright (C) 2019 - Parik

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 */

#include "weapons.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/injector.hpp"
#include "loader.hh"
#include "config.hh"

WeaponRandomizer *WeaponRandomizer::mInstance = nullptr;

/*******************************************************/
int*
GetWeaponInfo (int weaponId, char skill)
{
    int array_index = 47;

	switch (skill)
        {
        case 0: array_index = weaponId + 25; break;
        case 1: array_index = weaponId; break;
        case 2: array_index = weaponId + 36; break;
        case 3: array_index = weaponId + 47;
        }
    return (int*) ((char*) 0xC8AAB8 + 0x70 * array_index);
}

/*******************************************************/
int __fastcall RandomizeGiveWeapon (CPed *thisPed, void *edx, int weapon,
                                    int ammo, int slot)
{

	int original_slot = -1;
    if (weapon != 0)
	{
		int target_slot = GetWeaponInfo(weapon, 1)[5];
		
		weapon = 0;		
		while(weapon == 0 || weapon == 19 || weapon == 20 || weapon == 21 || weapon == 14 || weapon == 40)
			weapon = random(46, 1);

		StreamingManager::AttemptToLoadVehicle (GetWeaponInfo(weapon, 1)[3]);

		printf("%d - %d %d\n", weapon, GetWeaponInfo(weapon, 1)[3], GetWeaponInfo(weapon, 1)[4]);
		if(GetWeaponInfo(weapon, slot)[4] != -1)
			StreamingManager::AttemptToLoadVehicle (GetWeaponInfo(weapon, 1)[4]);

		original_slot = GetWeaponInfo(weapon, 1)[5];
		GetWeaponInfo(weapon, 1)[5] = target_slot;
	}
	
    int ret = thisPed->GiveWeapon (weapon, ammo, slot);

	if(original_slot >= 0)
		GetWeaponInfo(weapon, 1)[5] = original_slot;
	
	return ret;
}

/*******************************************************/
void
WeaponRandomizer::Initialise ()
{

	auto config = ConfigManager::GetInstance ()->GetConfigs ().weapon;
	if(!config.enabled)
		return;

	
    int addresses[]
        = {0x0438647, 0x043865E, 0x0438675, 0x043868C, 0x04386A3, 0x04386BD,
           0x04386D4, 0x04386EB, 0x0438705, 0x043871F, 0x0438748, 0x043875F,
           0x0438776, 0x043878D, 0x04387A4, 0x04387BE, 0x04387D5, 0x04387EC,
           0x0438806, 0x0438820, 0x043891B, 0x0438932, 0x0438949, 0x0438960,
           0x043897A, 0x0438994, 0x04389AB, 0x04389C5, 0x04389DF, 0x0438A08,
           0x0438A1F, 0x0438A36, 0x0438A4D, 0x0438A67, 0x0438A81, 0x0438A98,
           0x0438AB2, 0x0438ACC, 0x0438BAF, 0x0438BC6, 0x0438BDD, 0x0438BF4,
           0x0438C0B, 0x0438C25, 0x0438C3F, 0x0438C68, 0x0438C7F, 0x0438C96,
           0x0438CAD, 0x0438CC4, 0x0438CDE, 0x0438CF8, 0x04395D8, 0x0439F30,
           0x043D577, 0x043D8ED, 0x0441D93, 0x0442936, 0x0444ECE, 0x0448682,
           0x047D335, 0x048D8C7, 0x049C1CF, 0x049C248, 0x04D5CD0, 0x056EC5E,
           0x05B009C, 0x05D459D, 0x05DDCC0, 0x05E7D82, 0x05E7E2D, 0x05E83DE,
           0x05E899A, 0x061390C, 0x062B3BC, 0x062B5C9, 0x068B8DF, 0x068E355,
           0x068E39D, 0x068E3F2, 0x068E418, 0x069082D, 0x06D19E6, 0x06D1A24,
           0x074282C};
    
	for(int i = 0; i < sizeof(addresses)/4; i++)
	{
		injector::MakeCALL(addresses[i], (void*) &RandomizeGiveWeapon);
	}
	
    Logger::GetLogger ()->LogMessage ("Intialised WeaponRandomizer");
}

/*******************************************************/
void
WeaponRandomizer::DestroyInstance ()
{
    if (WeaponRandomizer::mInstance)
        delete WeaponRandomizer::mInstance;
}

/*******************************************************/
WeaponRandomizer *
WeaponRandomizer::GetInstance ()
{
    if (!WeaponRandomizer::mInstance)
        {
            WeaponRandomizer::mInstance = new WeaponRandomizer ();
            atexit (&WeaponRandomizer::DestroyInstance);
        }
    return WeaponRandomizer::mInstance;
}
