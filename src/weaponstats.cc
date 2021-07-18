#include "weaponstats.hh"
#include "functions.hh"
#include <cstdlib>
#include <algorithm>
#include "injector/injector.hpp"
#include "logger.hh"
#include "base.hh"
#include "config.hh"

WeaponStatsRandomizer *WeaponStatsRandomizer::mInstance = nullptr;
// Hex flags used to define weapon properties. Values provided in comments
// define single property. Every other value is combination of that properties.

// 0x1- aim anywhere, 0x2 - aim with arm, 0x4 - 1st person, 0x8 - no auto aim
int aimFlag[] = {0x1, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};

// 0x0 - none, 0x10 - can only aim while moving, 0x20 - can shoot while moving
int movementFlag[] = {0x0, 0x10, 0x20, 0x30};

// 0x0 -regular, 0x100 -throw, 0x200 - cant jump with weapon, 0x400 - continous
// fire, 0x800 - dual-wield
int otherFlag[] = {0x0, 0x200, 0x400, 0x600, 0x800, 0xA00, 0xC00, 0xE00};

// 0x0 - none, 0x1000 - reload, 0x2000 - can shoot while crouching, 0x4000 -
// reload to start, 0x8000 - long reload
int reloadFlag[] = {0x0,    0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x8000,
                    0x9000, 0xA000, 0xB000, 0xC000, 0xD000, 0xE000, 0xF000};

/*******************************************************/
int __fastcall RandomizeStats (int *address, int weaponid)
{
    CPed *player = FindPlayerPed ();

    if (address < (int *) player || address > (int *) (player + 0x764)
        || (weaponid <= 46 && weaponid >= 40) || weaponid == 36
        || weaponid == 37 || weaponid < 22)
        return 0;

    int weaponids[] = {weaponid, 0, 0, 0};

    /* Get weapon ids for all skill levels. */
    if (weaponid + 25 >= 47 && weaponid + 25 <= 57)
        {
            weaponids[1] = weaponid + 25;
            weaponids[2] = weaponid + 36;
            weaponids[3] = weaponid + 47;
        }

    short quality = random (100);

    short        ammoClip;
    short        damage;
    float        accuracy;
    float        moveSpeed;
    float        range;
    unsigned int properties;

    if (weaponid == 18 || weaponid == 16 || weaponid == 39 || weaponid == 17)
        properties = 0x100;

    else if (weaponid == 35 || weaponid == 36)
        properties = 0x48214;

    else if (weaponid == 38)
        properties = 0x238;
    // randomize SMG properties in the way which they can be used with jetpack
    else if (weaponid == 28 || weaponid == 29 || weaponid == 32)
        properties = aimFlag[1] + movementFlag[random (3)]
                     + otherFlag[random (7)] + reloadFlag[2];
    else
        properties = aimFlag[random (6)] + movementFlag[random (3)]
                     + otherFlag[random (7)] + reloadFlag[random (13)];

    if (quality < 10)
        {
            ammoClip  = random (1, 15);
            damage    = random (1, 25);
            accuracy  = randomFloat (0.1, 0.5);
            moveSpeed = randomFloat (0, 1.5);
            range     = randomFloat (10, 30);
        }
    else if (quality < 85)
        {
            ammoClip  = random (1, 30);
            damage    = random (1, 50);
            accuracy  = randomFloat (0.1, 0.75);
            moveSpeed = randomFloat (0.5, 2);
            range     = randomFloat (20, 50);
        }
    else if (quality < 95)
        {
            ammoClip  = random (30, 50);
            damage    = random (10, 75);
            accuracy  = randomFloat (0.25, 1);
            moveSpeed = randomFloat (1, 2.5);
            range     = randomFloat (20, 75);
        }
    else
        {
            ammoClip  = random (50, 100);
            damage    = random (20, 150);
            accuracy  = randomFloat (1, 2);
            moveSpeed = randomFloat (1.5, 3);
            range     = randomFloat (20, 150);
        }

    float animValues[] = {randomFloat (0, 0.70f), randomFloat (0, 0.70f),
                          randomFloat (0, 1.2f)};

    std::sort (animValues, animValues + 3, std::greater<float> ());

    for (int id : weaponids)
        {
            if (id == 0)
                break;

            aWeaponInfos[id].m_nFlags          = properties;
            aWeaponInfos[id].m_nAmmoClipSize   = ammoClip;
            aWeaponInfos[id].m_nDamage         = damage;
            aWeaponInfos[id].m_fAccuracy       = accuracy;
            aWeaponInfos[id].m_fSpeed          = moveSpeed;
            aWeaponInfos[id].m_fTargetingRange = range;
            aWeaponInfos[id].m_fFiringRange    = range;
            // aWeaponInfos[id].m_fAnimLoopStart  = animValues[2];
            // aWeaponInfos[id].m_fAnimFrameFire  = animValues[1];
            // aWeaponInfos[id].m_fAnimLoopEnd    = animValues[0];
            // aWeaponInfos[id].m_fAnim2LoopStart = animValues[2];
            // aWeaponInfos[id].m_fAnim2FrameFire = animValues[1];
            // aWeaponInfos[id].m_fAnim2LoopEnd   = animValues[0];
        }

    return 0;
}

/*******************************************************/
void
WeaponStatsRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("WeaponStatsRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x73B4C8, (void *) &RandomizeStats}});

    Logger::GetLogger ()->LogMessage ("Intialised WeaponStatsRandomizer");
}

/*******************************************************/
WeaponStatsRandomizer *
WeaponStatsRandomizer::GetInstance ()
{

    if (!WeaponStatsRandomizer::mInstance)
        {
            WeaponStatsRandomizer::mInstance = new WeaponStatsRandomizer ();
            atexit (&WeaponStatsRandomizer::DestroyInstance);
        }

    return WeaponStatsRandomizer::mInstance;
}

/*******************************************************/
void
WeaponStatsRandomizer::DestroyInstance ()
{
    if (WeaponStatsRandomizer::mInstance)
        delete WeaponStatsRandomizer::mInstance;
}
