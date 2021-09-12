#include "weapon_patterns.hh"
#include <sstream>
#include <cstdint>
#include "injector/calling.hpp"
#include "util/loader.hh"
#include "logger.hh"
#include "base.hh"
#include <algorithm>
#include <vector>

bool
WeaponPattern::WeaponTypes::GetValue (uint32_t weapon)
{
    if (std::find (weapon_slots[0].begin (), weapon_slots[0].end (), weapon)
            != weapon_slots[0].end ()
        || std::find (weapon_slots[1].begin (), weapon_slots[1].end (), weapon)
               != weapon_slots[1].end ()
        || std::find (weapon_slots[10].begin (), weapon_slots[10].end (),
                      weapon)
               != weapon_slots[10].end ())
        {
            return Melee;
        }
    else if (std::find (weapon_slots[2].begin (), weapon_slots[2].end (),
                        weapon)
             != weapon_slots[2].end ())
        {
            return Pistol;
        }
    else if (std::find (weapon_slots[3].begin (), weapon_slots[3].end (),
                        weapon)
             != weapon_slots[3].end ())
        {
            return Shotgun;
        }
    else if (std::find (weapon_slots[4].begin (), weapon_slots[4].end (),
                        weapon)
             != weapon_slots[4].end ())
        {
            return SMG;
        }
    else if (std::find (weapon_slots[5].begin (), weapon_slots[5].end (),
                        weapon)
             != weapon_slots[5].end ())
        {
            return Assault;
        }
    else if (std::find (weapon_slots[6].begin (), weapon_slots[6].end (),
                        weapon)
             != weapon_slots[6].end ())
        {
            return Rifle;
        }
    else if (std::find (weapon_slots[7].begin (), weapon_slots[7].end (),
                        weapon)
             != weapon_slots[7].end ())
        {
            return Heavy;
        }
    else if (std::find (weapon_slots[8].begin (), weapon_slots[8].end (),
                        weapon)
             != weapon_slots[8].end ())
        {
            return Projectile;
        }
    else if (std::find (weapon_slots[9].begin (), weapon_slots[9].end (),
                        weapon)
             != weapon_slots[9].end ())
        {
            return Spray;
        }
    else
        return Gadget;
}

/*******************************************************/
bool
WeaponPattern::DoesWeaponMatchPattern (int weaponID)
{
    if (weaponID == 19 || weaponID == 20 || weaponID == 21)
        return false;

    if (!GetPickup () && GetPedType () != 0
        && (weaponID == 14 || weaponID == 39))
        return false;

    if (!GetPickup () && weaponID == 40)
        return false;

    if (mFlags.Flame && weaponID != 16 && weaponID != 18 && weaponID != 35
        && weaponID != 36 && weaponID != 37 && weaponID != 39)
        return false;

    if (mFlags.NoFlame && (weaponID == 18 || weaponID == 37))
        return false;

    if (mFlags.NoUselessProj && (weaponID == 17 || weaponID == 39))
        return false;

    if (mFlags.Explosive && weaponID != 16 && weaponID != 18 && weaponID != 35
        && weaponID != 36 && weaponID != 39)
        return false;

    if (mFlags.LongRange && weaponID != 27 && weaponID != 30 && weaponID != 31
        && weaponID != 33 && weaponID != 34 && weaponID != 35 && weaponID != 36
        && weaponID != 38)
        return false;

    if (mFlags.Goggles && weaponID != 44 && weaponID != 45)
        return false;

    if (mFlags.DualWield && weaponID != 22 && weaponID != 32 && weaponID != 28)
        return false;

    if (mFlags.CanDriveby
        && (weaponID < 22 || weaponID > 38 || weaponID == 34 || weaponID == 35
            || weaponID == 36 || weaponID == 37))
        return false;

    if (mFlags.ProjectileCheck && weaponID != 16 && weaponID != 17
        && weaponID != 18 && weaponID != 35 && weaponID != 36)
        return false;

    if (mFlags.NoExplode
        && (weaponID == 16 || weaponID == 35 || weaponID == 36 || weaponID == 37
            || weaponID == 39))
        return false;

    // Excludes weapons that go into first person aiming on use
    if (mFlags.NoFPSAiming
        && (weaponID == 34 || weaponID == 35 || weaponID == 36
            || weaponID == 43))
        return false;

    // Type check (it has to be allowed)
    if (!mAllowedTypes.GetValue (weaponID))
        return false;

    return true;
}

/*******************************************************/
void
WeaponPattern::Cache ()
{
    m_aCache.clear ();

    for (int i = 1; i < 47; i++)
        {
            if (i == m_nOriginalWeapon || DoesWeaponMatchPattern (i))
                {
                    m_aCache.push_back (i);
                }
        }

    if (GetPickup () && mFlags.Extra)
        {
            for (int i = 0; i < additional_pickups.size (); i++)
                {
                    m_aCache.push_back (additional_pickups[i]);
                }
        }

    m_bCached = true;
}

/*******************************************************/
uint32_t
WeaponPattern::GetRandom ()
{
    if (!m_bCached)
        Cache ();

    int newWeaponID = GetRandomElement (m_aCache);

    return newWeaponID;
}

/*******************************************************/
bool
WeaponPattern::MatchWeapon (int weaponID, int ped, bool pickup)
{
    if (weaponID != GetOriginalWeapon ()
        || (!CRunningScripts::CheckForRunningScript (GetThreadName ().c_str ())
            && GetThreadName () != "any")
        || (ped != GetPedType () && GetPedType () != -1)
        || pickup != GetPickup ())
        return false;

    return true;
}

/*******************************************************/
void
WeaponPattern::ReadFlag (const std::string &flag)
{
    m_bCached = false;

    if (flag == "extra")
        mFlags.Extra = true;
    else if (flag == "explosive")
        mFlags.Explosive = true;
    else if (flag == "noexplosives")
        mFlags.NoExplode = true;
    else if (flag == "flame")
        mFlags.Flame = true;
    else if (flag == "noflame")
        mFlags.NoFlame = true;
    else if (flag == "uselessproj")
        mFlags.NoUselessProj = true;
    else if (flag == "longrange")
        mFlags.LongRange = true;
    else if (flag == "goggles")
        mFlags.Goggles = true;
    else if (flag == "dualwield")
        mFlags.DualWield = true;
    else if (flag == "candriveby")
        mFlags.CanDriveby = true;
    else if (flag == "nofps")
        mFlags.NoFPSAiming = true;
    else if (flag == "projectilecheck")
        mFlags.ProjectileCheck = true;
}

/*******************************************************/
void
WeaponPattern::ParseFlags (const std::string &flags)
{
    std::istringstream flagStream (flags);
    std::string        flag = "";

    while (std::getline (flagStream, flag, '+'))
        ReadFlag (flag);
}