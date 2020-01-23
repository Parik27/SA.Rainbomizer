#include "weaponstats.hh"
#include "functions.hh"
#include <cstdlib>
#include <algorithm>
#include "injector/injector.hpp"
#include "logger.hh"
#include "base.hh"
#include "config.hh"

WeaponStatsRandomizer *WeaponStatsRandomizer::mInstance = nullptr;
int                    hexFlags[]
    = {0x3033, 0x3833, 0x7001, 0x7011,  0x7031,  0x2001, 0x2011, 0x2031, 0x3013,
       0x3033, 0xA008, 0xA014, 0x48214, 0x30238, 0x238,  0x0038, 0x0431};

/*******************************************************/
int __fastcall RandomizeStats (int *address, int weaponid)
{
    CPed *player = FindPlayerPed ();

    if (address < (int *) player || address > (int *) (player + 0x764)
        || weaponid == 41 || weaponid == 42 || weaponid == 43 || weaponid == 37)
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
        {
            properties = 0x100;
        }
    else if (weaponid == 35 || weaponid == 36)
        {
            properties = 0x48214;
        }
    else if (weaponid == 38)
        {
            properties = 0x238;
        }
    else
        {
            properties = hexFlags[random (0, 16)];
        }

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

            aWeaponInfos[id].m_nFlags = properties;

            aWeaponInfos[id].m_nAmmoClipSize = ammoClip;

            aWeaponInfos[id].m_nDamage = damage;

            aWeaponInfos[id].m_fAccuracy = accuracy;

            aWeaponInfos[id].m_fSpeed = moveSpeed;

            aWeaponInfos[id].m_fTargetingRange = range;

            aWeaponInfos[id].m_fFiringRange = range;

            aWeaponInfos[id].m_fAnimLoopStart = animValues[2];

            aWeaponInfos[id].m_fAnimFrameFire = animValues[1];

            aWeaponInfos[id].m_fAnimLoopEnd = animValues[0];

            aWeaponInfos[id].m_fAnim2LoopStart = animValues[2];

            aWeaponInfos[id].m_fAnim2FrameFire = animValues[1];

            aWeaponInfos[id].m_fAnim2LoopEnd = animValues[0];
        }

    return 0;
}

/*******************************************************/
void
WeaponStatsRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().weaponStats;

    if (!config.enabled)
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
