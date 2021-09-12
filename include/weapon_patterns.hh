#pragma once

#include <CMath.hh>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "functions.hh"
#include "logger.hh"

static std::vector<std::vector<int>> weapon_slots
    = {{0, 1},                   // Fist Slot 0
       {2, 3, 4, 5, 6, 7, 8, 9}, // Melee Slot 1
       {22, 23, 24},             // Pistol Slot 2
       {25, 26, 27},             // Shotgun Slot 3
       {28, 29, 32},             // SMG Slot 4
       {30, 31},                 // Assault Rifle Slot 5
       {33, 34},                 // Rifle Slot 6
       {35, 36, 37, 38},         // Heavy Weapons Slot 7
       {16, 17, 18, 39},         // Projectiles Slot 8
       {41, 42, 43},             // Spray / Camera Slot 9
       {10, 11, 12, 13, 14, 15}, // Gift Slot 10
       {44, 45, 46},             // Wearable Slots 11
       {40}};                    // Detonator Slot 12

static const std::vector<int> additional_pickups = {1240, 1242, 1247, 1241};

/*******************************************************/
/* Class to handle a single weapon pattern.    */
/*******************************************************/
class WeaponPattern
{
    struct WeaponTypes
    {
        bool Melee : 1;
        bool Pistol : 1;
        bool Shotgun : 1;
        bool SMG : 1;
        bool Assault : 1;
        bool Rifle : 1;
        bool Heavy : 1;
        bool Projectile : 1;
        bool Spray : 1;
        bool Gadget : 1;

        bool GetValue (uint32_t type);

    } mAllowedTypes;

    struct
    {
        bool Extra : 1;
        bool Explosive : 1;
        bool NoExplode : 1;
        bool Flame : 1;
        bool NoFlame : 1;
        bool NoUselessProj : 1;
        bool LongRange : 1;
        bool Goggles : 1;
        bool DualWield : 1;
        bool CanDriveby : 1;
        bool ProjectileCheck : 1;
        bool NoFPSAiming : 1;
    } mFlags;

    uint32_t m_nPedType        = 0;
    uint32_t m_nOriginalWeapon = 0;

    std::string m_szThread = "";

    bool                  m_bPickup = false;
    bool                  m_bCached = false;
    std::vector<uint32_t> m_aCache;

public:
    std::unordered_map<std::string, int> weaponIDs
        = {{"fist", 0},          {"brassknuckle", 1}, {"golfclub", 2},
           {"nitestick", 3},     {"knifecur", 4},     {"bat", 5},
           {"shovel", 6},        {"poolcue", 7},      {"katana", 8},
           {"chnsaw", 9},        {"gun_dildo1", 10},  {"gun_dildo2", 11},
           {"gun_vibe1", 12},    {"gun_vibe2", 13},   {"flowera", 14},
           {"gun_cane", 15},     {"grenade", 16},     {"teargas", 17},
           {"molotov", 18},      {"colt45", 22},      {"silenced", 23},
           {"desert_eagle", 24}, {"chromegun", 25},   {"sawnoff", 26},
           {"shotgspa", 27},     {"micro_uzi", 28},   {"mp5lng", 29},
           {"ak47", 30},         {"m4", 31},          {"tec9", 32},
           {"cuntgun", 33},      {"sniper", 34},      {"rocketla", 35},
           {"heatseek", 36},     {"flame", 37},       {"minigun", 38},
           {"satchel", 39},      {"bomb", 40},        {"spraycan", 41},
           {"fire_ex", 42},      {"camera", 43},      {"nvgoggles", 44},
           {"irgoggles", 45},    {"gun_para", 46}};

    /*******************************************************/
    void
    SetAllowedTypes (WeaponTypes types)
    {
        m_bCached     = false;
        mAllowedTypes = types;
    }
    void
    SetOriginalWeapon (char *weapon)
    {
        m_bCached         = false;
        m_nOriginalWeapon = weaponIDs[weapon];
    }
    void
    SetPedType (int ped)
    {
        m_bCached  = false;
        m_nPedType = ped;
    }
    uint32_t
    GetPedType () const
    {
        return m_nPedType;
    }
    void
    SetPickup (char pickup)
    {
        m_bCached = false;
        if (pickup == 'Y')
            m_bPickup = true;
        else
            m_bPickup = false;
    }

    bool
    GetPickup () const
    {
        return m_bPickup;
    }

    void
    SetThreadName (char threadName[64])
    {
        m_bCached  = false;
        m_szThread = threadName;
    }

    std::string
    GetThreadName () const
    {
        return m_szThread;
    }

    uint32_t
    GetOriginalWeapon () const
    {
        return m_nOriginalWeapon;
    }

    // Returns if a weapon matches a certain pattern
    bool DoesWeaponMatchPattern (int weaponID);
    void Cache ();

    uint32_t GetRandom ();

    // Reads a flag string in the format "flag=value" or "flag" for just bools
    void ReadFlag (const std::string &flag);

    // Reads a list of flags delimited by a '+'
    void ParseFlags (const std::string &flags);

    bool MatchWeapon (int weaponID, int ped, bool pickup);

    WeaponPattern ()
        : mFlags{false, false, false, false, false, false,
                 false, false, false, false, false}
    {
    }
};
