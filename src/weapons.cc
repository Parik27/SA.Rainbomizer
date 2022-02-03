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
#include "weapon_patterns.hh"
#include "util/scrpt.hh"
#include "pickups.hh"

WeaponRandomizer *WeaponRandomizer::mInstance = nullptr;

const int WEAPON_MELEE = 100;

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

    int original_slot = -1;
    if (weapon != 0
        && !(FindPlayerPed () == thisPed
             && !WeaponRandomizer::m_Config.RandomizePlayerWeapons))
        {
            int target_slot = GetWeaponInfo (weapon, 1)[5];

            weapon = WeaponRandomizer::GetInstance ()->GetRandomWeapon (thisPed,
                                                                        weapon,
                                                                        false);

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
int
InitialiseCacheForWeaponRandomization (void *fileName)
{
    WeaponRandomizer::GetInstance ()->InitialiseCache ();
    return CGame::Init2 (fileName);
}

/*******************************************************/
void
WeaponRandomizer::InitialiseCache ()
{
    this->CachePatterns ();
}

/*******************************************************/
CVehicle *__fastcall ChangeLockedPlayerWeaponForTurrets (
    CPed *attachedPed, void *edx, CVehicle *attachedCar, float x, float y,
    float z, int posType, int angle, int weaponID)
{
    if (weaponID != 0
        && !(FindPlayerPed () == attachedPed
             && !WeaponRandomizer::m_Config.RandomizePlayerWeapons))
        {
            weaponID = WeaponRandomizer::GetInstance ()->GetRandomWeapon (
                attachedPed, weaponID, false);

            StreamingManager::AttemptToLoadVehicle (
                GetWeaponInfo (weaponID, 1)[3]);

            if (GetWeaponInfo (weaponID, 1)[4] != -1)
                StreamingManager::AttemptToLoadVehicle (
                    GetWeaponInfo (weaponID, 1)[4]);
        }

    return CallMethodAndReturn<CVehicle *, 0x5E7CB0> (attachedPed, attachedCar,
                                                      x, y, z, posType, angle,
                                                      weaponID);
}

/*******************************************************/
void __fastcall OverrideWeaponCheck (CRunningScript *scr, void *edx,
                                     short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[0] == ScriptSpace[3] && ScriptParams[1] > 0
        && ScriptParams[1] < 44 && WeaponRandomizer::playerWeaponID != -1)
        ScriptParams[1] = WeaponRandomizer::playerWeaponID;
    else if (scr->CheckName ("dealer"))
        {
            ScriptParams[1] = WeaponRandomizer::dealerWeaponID;
        }
}

/*******************************************************/
void __fastcall IgnoreWeaponAmmoCheck (CRunningScript *scr, void *edx,
                                       short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[0] == ScriptSpace[3]
        && WeaponRandomizer::playerWeaponID != -1)
        {
            ScriptParams[1] = WeaponRandomizer::playerWeaponID;
        }
}

/*******************************************************/
void __fastcall AllowMoreExplosionTypes (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    ScriptParams[0] = -1;
    if (scr->CheckName ("cat4"))
        {
            float &corner1X = ((float *) ScriptParams)[1];
            float &corner1Y = ((float *) ScriptParams)[2];
            float &corner1Z = ((float *) ScriptParams)[3];
            float &corner2X = ((float *) ScriptParams)[4];
            float &corner2Z = ((float *) ScriptParams)[6];
            if (int (corner1X) == 823 && int (corner1Y) == 8
                && int (corner1Z) == 1000)
                {
                    corner1X = 822.0f;
                    corner1Y = 8.0f;
                    corner2X = 829.0f;
                }
            else if (int (corner1X) == 819 && int (corner1Y) == 8
                     && int (corner1Z) == 1003)
                {
                    corner1Z = 1001.0f;
                    corner2Z = 1008.0f;
                }
        }
}

/*******************************************************/
void __fastcall ChangeLockedPlayerWeapon (CRunningScript *scr, void *edx,
                                          short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[0] == ScriptSpace[3] && ScriptParams[1] != 0
        && WeaponRandomizer::playerWeaponID != -1)
        ScriptParams[1] = WeaponRandomizer::playerWeaponID;
    else if (ScriptParams[0] != ScriptSpace[3])
        {
            CPed *ped = reinterpret_cast<CPed *> (
                ms_pPedPool->m_pObjects + 0x7C4 * (ScriptParams[0] >> 8));
            ped->SetCurrentWeapon (GetWeaponInfo (ScriptParams[1], 1)[5]);
        }
}

/*******************************************************/
void __fastcall ChangeGiveAmmoWeapon (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[0] == ScriptSpace[3]
        && WeaponRandomizer::playerWeaponID != -1)
        ScriptParams[1] = WeaponRandomizer::playerWeaponID;
}

/*******************************************************/
void __fastcall OverrideWeaponRemoval (CRunningScript *scr, void *edx,
                                       short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[0] == ScriptSpace[3]
        && WeaponRandomizer::playerWeaponID != -1 && ScriptParams[1] < 44)
        ScriptParams[1] = WeaponRandomizer::playerWeaponID;
}

/*******************************************************/
void __fastcall OverrideEntityHitByWeapon (CRunningScript *scr, void *edx,
                                           short count)
{
    scr->CollectParameters (count);
    if (WeaponRandomizer::playerWeaponID != -1 && ScriptParams[1] > 0
        && ScriptParams[1] < 44 && scr->CheckName ("zero1"))
        ScriptParams[1] = 57;
}

/*******************************************************/
void
ResetPlayerWeaponID ()
{
    WeaponRandomizer::playerWeaponID         = -1;
    WeaponRandomizer::forceWeapon            = false;
    WeaponRandomizer::firstPartFinaleCActive = false;
    Call<0x6F6B60> (); // ReleaseMissionTrains, always called on mission cleanup
}

/*******************************************************/
void __fastcall ResetPlayerWeaponIDShrange (CRunningScript *scr, void *edx,
                                            short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("shrange") && ScriptSpace[8070] < 1)
        {
            WeaponRandomizer::playerWeaponID = -1;
        }
}

/*******************************************************/
int *
SetWeaponUsableInJetpack (int weaponId, char skill)
{
    int *weaponInfo = GetWeaponInfo (weaponId, skill);
    if (weaponId >= 22 && weaponId <= 32)
        weaponInfo[6] = 7833;
    return weaponInfo;
}

/*******************************************************/
int
ResetPlayerWeaponIDOnStart ()
{
    WeaponRandomizer::playerWeaponID         = -1;
    WeaponRandomizer::forceWeapon            = false;
    WeaponRandomizer::firstPartFinaleCActive = false;
    return CallAndReturn<int, 0x464BB0> ();
}

/*******************************************************/
void __fastcall CheckIfPlayerDriveBy (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[0] == ScriptSpace[3])
        {
            WeaponRandomizer::forceWeapon = true;
            if (scr->CheckName ("finalec")
                && !WeaponRandomizer::firstPartFinaleCActive)
                {
                    Scrpt::CallOpcode (0x2A3, "enable_widescreen", 1);
                    Scrpt::CallOpcode (0x2A3, "enable_widescreen", 0);
                    Scrpt::CallOpcode (0x15a, "restore_camera");
                    WeaponRandomizer::firstPartFinaleCActive = true;
                }
        }
}

/*******************************************************/
void __fastcall CheckIfPlayerJetpack (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[0] == ScriptSpace[3])
        WeaponRandomizer::forceWeapon = true;
}

/*******************************************************/
void
ForceWeapon ()
{
    if (!ScriptSpace[409])
        WeaponRandomizer::forceWeapon = false;
    if (WeaponRandomizer::forceWeapon && WeaponRandomizer::playerWeaponID != -1)
        FindPlayerPed ()->SetCurrentWeapon (
            GetWeaponInfo (WeaponRandomizer::playerWeaponID, 1)[5]);
    Call<0x52CF10> ();
}

/*******************************************************/
void __fastcall RemoveExtraJBAmmo (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[1] == 2000)
        Scrpt::CallOpcode (0x555, "remove_weapon", GlobalVar (3),
                           WeaponRandomizer::playerWeaponID);
}

/*******************************************************/
void __fastcall GiveHeist9Goggles (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    float x = ((float *) ScriptParams)[1];
    float y = ((float *) ScriptParams)[1];
    float z = ((float *) ScriptParams)[1];
    if (scr->CheckName ("heist9") && int (x) == 2149 && int (1605) && int (1005))
        Scrpt::CallOpcode (0x1b2, "give_weapon_and_ammo", GlobalVar (3), 44,
                           300);
}

/*******************************************************/
void
WeaponRandomizer::Initialise ()
{
    if (ConfigManager::ReadConfig ("WeaponRandomizer")
        || ConfigManager::ReadConfig ("PickupsRandomizer"))
        {
            injector::MakeCALL (0x48AE9E, (void *) &OverrideWeaponCheck);
            injector::MakeCALL (0x489AFA, (void *) &IgnoreWeaponAmmoCheck);
            injector::MakeCALL (0x48D677, (void *) &OverrideWeaponRemoval);
            injector::MakeCALL (0x482B8D, (void *) &AllowMoreExplosionTypes);
            injector::MakeCALL (0x4685AD, (void *) &ResetPlayerWeaponID);
            injector::MakeCALL (0x489A70, (void *) &ResetPlayerWeaponIDOnStart);
            injector::MakeCALL (0x4680F2, (void *) &ResetPlayerWeaponIDShrange);
        }

    if (!ConfigManager::ReadConfig (
            "WeaponRandomizer",
            std::pair ("RandomizePlayerWeapons",
                       &m_Config.RandomizePlayerWeapons),
            std::pair ("SkipChecks", &m_Config.SkipChecks)))
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
          0x061390C, 0x062B3BC, 0x062B5C9, 0x068B8DF, 0x068E355, 0x068E39D,
          0x068E3F2, 0x068E418, 0x069082D, 0x06D19E6, 0x06D1A24})
        {
            injector::MakeCALL (address, (void *) &RandomizeGiveWeapon);
        }

    // CPed::AttachPedToEntity
    for (int address : {0x041C34E, 0x046FB17, 0x048A84D, 0x048C523, 0x0496A77,
                        0x05E7E93, 0x0665A64, 0x0673844, 0x06F3D67})
        {
            injector::MakeCALL (address,
                                (void *) &ChangeLockedPlayerWeaponForTurrets);
        }

    // CPed::GiveWeapon with Player exception
    injector::MakeCALL (0x5E899A, (void *) &RandomizeGiveWeaponDelayed);
    injector::MakeNOP (0x5E62D8, 4);

    if (!m_Config.SkipChecks)
        injector::MakeCALL (0x53BCA6,
                            (void *) &InitialiseCacheForWeaponRandomization);

    injector::MakeCALL (0x47D4AC, (void *) &ChangeLockedPlayerWeapon);
    injector::MakeCALL (0x469AAC, (void *) &ChangeGiveAmmoWeapon);
    injector::MakeCALL (0x47CDCB, (void *) &ChangeGiveAmmoWeapon);
    injector::MakeCALL (0x480E18, (void *) &OverrideEntityHitByWeapon);
    injector::MakeCALL (0x480E8E, (void *) &OverrideEntityHitByWeapon);
    injector::MakeCALL (0x46D3AB, (void *) &CheckIfPlayerDriveBy);
    injector::MakeCALL (0x53BFBD, (void *) &ForceWeapon);
    injector::MakeCALL (0x47E9E4, (void *) &RemoveExtraJBAmmo);
    injector::MakeCALL (0x475373, (void *) &GiveHeist9Goggles);

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
WeaponRandomizer::GetRandomWeapon (CPed *ped, int weapon, bool isPickup)
{
    std::vector<int> buggy_weapons;

    if (!isPickup && weapon == 46)
        return weapon;

    if (isPickup && PickupsRandomizer::m_Config.ReplaceWithWeaponsOnly)
        {
            buggy_weapons = {19, 20, 21, 47, 48, 49};
        }
    else if (isPickup && !PickupsRandomizer::m_Config.ReplaceWithWeaponsOnly)
        {
            buggy_weapons = {19};
        }
    else
        {
            if (ped->m_nPedType == ePedType::PED_TYPE_PLAYER1)
                {
                    buggy_weapons = {19, 20, 21, 40, 47, 48, 49};
                }
            else
                {
                    buggy_weapons = {19, 20, 21, 14, 40, 39, 47, 48, 49};
                }
        }

    for (auto &pattern : mWeaponPatterns)
        {
            int pedType = -1;
            if (!isPickup && ped != nullptr)
                pedType = ped->m_nPedType;
            if (pattern.MatchWeapon (weapon, pedType, isPickup))
                {
                    int newWeaponID = pattern.GetRandom ();
                    if ((pedType == 0 || pedType == -1) && newWeaponID < 44)
                        {
                            if (!CRunningScripts::CheckForRunningScript (
                                    "heist9")
                                || (weapon != 29 && weapon != 4))
                                {
                                    playerWeaponID = newWeaponID;
                                }
                        }
                    return newWeaponID;
                }
        }

    while ((weapon = random (1, 49),
            std::find (buggy_weapons.begin (), buggy_weapons.end (), weapon)
                != buggy_weapons.end ()))
        ;

    if (!isPickup && ped != nullptr)
        if (ped->m_nPedType == 17)
            dealerWeaponID = weapon;

    return weapon;
}

/*******************************************************/
void
WeaponRandomizer::CachePatterns ()
{
    FILE *weaponPatternsFile
        = OpenRainbomizerFile ("Weapon_Patterns.txt", "r", "data/");
    if (weaponPatternsFile)
        {
            char line[2048] = {0};
            while (fgets (line, 2048, weaponPatternsFile))
                {
                    if (line[0] == '#' || strlen (line) < 10)
                        continue;

                    char threadName[64] = {0};
                    char weaponName[64] = {0};
                    char flags[256]     = {0};
                    int  ped            = 0;
                    char melee          = 'N';
                    char pistol         = 'N';
                    char shotgun        = 'N';
                    char smg            = 'N';
                    char assault        = 'N';
                    char rifle          = 'N';
                    char heavy          = 'N';
                    char projectile     = 'N';
                    char spray          = 'N';
                    char gadget         = 'N';
                    char pickup         = 'N';

                    sscanf (line,
                            "%s %s %d %c %c %c %c %c %c %c %c %c %c %c %s",
                            threadName, weaponName, &ped, &melee, &pistol,
                            &shotgun, &smg, &assault, &rifle, &heavy,
                            &projectile, &spray, &gadget, &pickup, flags);

                    for (int i = 0; i < 64; i++)
                        {
                            threadName[i] = NormaliseChar (threadName[i]);
                            weaponName[i] = NormaliseChar (weaponName[i]);
                        }

                    WeaponPattern pattern;
                    pattern.SetOriginalWeapon (weaponName);
                    pattern.SetThreadName (threadName);

                    pattern.SetAllowedTypes (
                        {melee == 'Y', pistol == 'Y', shotgun == 'Y',
                         smg == 'Y', assault == 'Y', rifle == 'Y', heavy == 'Y',
                         projectile == 'Y', spray == 'Y', gadget == 'Y'});

                    pattern.SetPedType (ped);
                    pattern.SetPickup (pickup);
                    pattern.ParseFlags (flags);

                    pattern.Cache ();

                    mWeaponPatterns.push_back (pattern);
                }
            Logger::GetLogger ()->LogMessage ("Cached Weapon Patterns.");
        }
    else if (!weaponPatternsFile)
        {
            // Log a message if file wasn't found
            Logger::GetLogger ()->LogMessage (
                "Failed to read file: rainbomizer/data/Weapon_Patterns.txt");
            return;
        }
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
