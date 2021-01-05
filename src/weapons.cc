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
#include "util/loader.hh"
#include "config.hh"
#include <algorithm>

WeaponRandomizer *WeaponRandomizer::mInstance = nullptr;

const int WEAPON_MELEE = 100;

std::vector<std::vector<int>> weapon_slots = {{0, 1},
                                              {2, 3, 4, 5, 6, 7, 8, 9},
                                              {22, 23, 24},
                                              {25, 26, 27},
                                              {28, 29, 32},
                                              {30, 31},
                                              {33, 34},
                                              {35, 36, 37, 38},
                                              {16, 17, 18, 39},
                                              {41, 42, 43},
                                              {10, 11, 12, 14, 15},
                                              {44, 45, 46}};

/*******************************************************/
int *
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
    return (int *) ((char *) 0xC8AAB8 + 0x70 * array_index);
}

/*******************************************************/
int __fastcall RandomizeGiveWeapon (CPed *thisPed, void *edx, int weapon,
                                    int ammo, int slot)
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().weapon;

    int original_slot = -1;
    if (weapon != 0
        && !(FindPlayerPed () == thisPed && !config.playerRandomization))
        {
            int target_slot = GetWeaponInfo (weapon, 1)[5];

            weapon = WeaponRandomizer::GetInstance ()->GetRandomWeapon (thisPed,
                                                                        weapon, false);

            StreamingManager::AttemptToLoadVehicle (
                GetWeaponInfo (weapon, 1)[3]);

            if (GetWeaponInfo (weapon, slot)[4] != -1)
                StreamingManager::AttemptToLoadVehicle (
                    GetWeaponInfo (weapon, 1)[4]);

            if (thisPed != FindPlayerPed ())
                {
                    original_slot                = GetWeaponInfo (weapon, 1)[5];
                    GetWeaponInfo (weapon, 1)[5] = target_slot;
                }
        }

    int ret = thisPed->GiveWeapon (weapon, ammo, slot);

    if (original_slot >= 0)
        GetWeaponInfo (weapon, 1)[5] = original_slot;

    return ret;
}

/*******************************************************/
int __fastcall RandomizeGiveWeaponDelayed (CPed *thisPed, void *edx, int weapon,
                                           int ammo, int slot)
{
    if (thisPed == FindPlayerPed ())
        return thisPed->GiveWeapon (weapon, ammo, slot);

    return RandomizeGiveWeapon (thisPed, edx, weapon, ammo, slot);
}

/*******************************************************/
void __fastcall Opcode1B9Fix (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    CPed *ped = reinterpret_cast<CPed *> (ms_pPedPool->m_pObjects
                                          + 0x7C4 * (ScriptParams[0] >> 8));
    ped->SetCurrentWeapon (GetWeaponInfo (ScriptParams[1], 1)[5]);
}

/*******************************************************/
void
WeaponRandomizer::Initialise ()
{

    auto config = ConfigManager::GetInstance ()->GetConfigs ().weapon;
    if (!config.enabled)
        return;

    // CPed::GiveWeapon
    for (int address :
         {0x0438647, 0x043865E, 0x0438675, 0x043868C, 0x04386A3, 0x04386BD,
          0x04386D4, 0x04386EB, 0x0438705, 0x043871F, 0x0438748, 0x043875F,
          0x0438776, 0x043878D, 0x04387A4, 0x04387BE, 0x04387D5, 0x04387EC,
          0x0438806, 0x0438820, 0x043891B, 0x0438932, 0x0438949, 0x0438960,
          0x043897A, 0x0438994, 0x04389AB, 0x04389C5, 0x04389DF, 0x0438A08,
          0x0438A1F, 0x0438A36, 0x0438A4D, 0x0438A67, 0x0438A81, 0x0438A98,
          0x0438AB2, 0x0438ACC, 0x0438BAF, 0x0438BC6, 0x0438BDD, 0x0438BF4,
          0x0438C0B, 0x0438C25, 0x0438C3F, 0x0438C68, 0x0438C7F, 0x0438C96,
          0x0438CAD, 0x0438CC4, 0x0438CDE, 0x0438CF8, 0x04395D8, 0x0439F30,
          0x043D577, 0x043D8ED, 0x0442936, 0x0444ECE, 0x0448682, 0x047D335,
          0x048D8C7, 0x049C1CF, 0x049C248, 0x056EC5E, 0x05B009C, 0x05DDCC0,
          0x05E7D82, 0x05E7E2D, 0x061390C, 0x062B3BC, 0x062B5C9, 0x068B8DF,
          0x068E355, 0x068E39D, 0x068E3F2, 0x068E418, 0x069082D, 0x06D19E6,
          0x06D1A24})
        {
            injector::MakeCALL (address, (void *) &RandomizeGiveWeapon);
        }

    // CPed::GiveWeapon with Player exception
    injector::MakeCALL (0x5E899A, (void *) &RandomizeGiveWeaponDelayed);
    injector::MakeCALL (0x47D4AC, (void *) &Opcode1B9Fix);
    injector::MakeNOP (0x5E62D8, 4);

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
int
WeaponRandomizer::GetRandomWeapon (CPed *ped, int weapon, bool ignoreBuggy)
{
    auto &config = ConfigManager::GetInstance ()->GetConfigs ().weapon;

    int slot = GetWeaponInfo (weapon, 1)[5];
    for (auto pattern : config.patterns)
        {
            if ((pattern.weapon == -1 || pattern.weapon == weapon)
                && (pattern.ped == -1
                    || (pattern.ped == 0 && ped == FindPlayerPed ()))
                && (pattern.thread == ""
                    || CRunningScripts::CheckForRunningScript (
                        pattern.thread.c_str ())))
                {
                    if (pattern.allowed.size () != 0)
                        {
                            int weapon = pattern.allowed[random (
                                pattern.allowed.size () - 1)];

                            if (weapon == WEAPON_SLOT)
                                {
                                    auto slot_weapons = weapon_slots[slot];
                                    weapon            = slot_weapons[random (
                                        slot_weapons.size () - 1)];
                                }

                            return weapon;
                        }
                    int              weapon;
                    std::vector<int> buggy_weapons;
                    if (ignoreBuggy)
                    {
                            buggy_weapons = {19, 20, 21};
                    }
                    else
                    {
                            buggy_weapons = {19, 20, 21, 14, 40, 39};
                            if (ped->m_nPedType == ePedType::PED_TYPE_DEALER)
                            {
                                    buggy_weapons.push_back (22);
                            }
                    }

                    while ((weapon = random (1, 46),

                            std::find (buggy_weapons.begin (),
                                       buggy_weapons.end (), weapon)
                                    != buggy_weapons.end ()

                                || std::find (pattern.denied.begin (),
                                              pattern.denied.end (), weapon)
                                       != pattern.denied.end ()))
                        ;

                    return weapon;
                }
        }
    return weapon;
}

/*******************************************************/
// Currently irrelevant pls ignore
int
WeaponRandomizer::GetRandomPickup (CPed *ped, int weapon, bool ignoreBuggy)
{
    return weapon;
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
