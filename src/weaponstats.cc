#include "weaponstats.hh"
#include "functions.hh"
#include <cstdlib>
#include <algorithm>
#include "injector/injector.hpp"
#include "logger.hh"
#include "base.hh"
#include <iostream>
#include "config.hh"

int                    statsStart                       = 0xC8AAB8;
int                    itemSize                         = 0x70;
WeaponStatsRandomizer *WeaponStatsRandomizer::mInstance = nullptr;

/*******************************************************/
int __fastcall RandomizeStats (int *address, int weaponid)
{
    CPed *player = FindPlayerPed ();

    if (address < (int *) player || address > (int *) (player + 0x764)
        || weaponid == 41 || weaponid == 42)
        return 0;

    int weaponids[] = {weaponid, 0, 0, 0};

    if (weaponid + 25 >= 47 && weaponid + 25 <= 57)
        {
            weaponids[1] = weaponid + 25;
            weaponids[2] = weaponid + 36;
            weaponids[3] = weaponid + 47;
        }

    short quality = random (100);

    short ammoClip;
    short damage;
    float accuracy;
    float moveSpeed;
    float range;

    if (quality < 10)
        {
            ammoClip  = random (1, 15);
            damage    = random (1, 25);
            accuracy  = randomFloat (1, 10);
            moveSpeed = randomFloat (0, 1.5);
            range     = randomFloat (10, 30);
        }
    else if (quality < 85)
        {
            ammoClip  = random (1, 30);
            damage    = random (1, 50);
            accuracy  = randomFloat (5, 15);
            moveSpeed = randomFloat (0.5, 2);
            range     = randomFloat (20, 50);
        }
    else if (quality < 95)
        {
            ammoClip  = random (1, 50);
            damage    = random (1, 75);
            accuracy  = randomFloat (10, 20);
            moveSpeed = randomFloat (1, 2.5);
            range     = randomFloat (20, 75);
        }
    else
        {
            ammoClip  = random (1, 100);
            damage    = random (1, 150);
            accuracy  = randomFloat (15, 30);
            moveSpeed = randomFloat (1.5, 3);
            range     = randomFloat (20, 150);
        }

    float values[] = {randomFloat (0, 0.70f), randomFloat (0, 0.70f),
                      randomFloat (0, 1.2f)};

    std::sort (values, values + 3, std::greater<float> ());

    for (int id : weaponids)
        {
            if (id == 0)
                break;

            int itemAddress = statsStart + itemSize * id;

            injector::WriteMemory<short> (itemAddress + 0x20, ammoClip);

            injector::WriteMemory<short> (itemAddress + 0x22, damage);

            injector::WriteMemory<float> (itemAddress + 0x38, accuracy);

            injector::WriteMemory<float> (itemAddress + 0x3C, moveSpeed);

            injector::WriteMemory<float> (itemAddress + 0x04, range);

            injector::WriteMemory<float> (itemAddress + 0x08, range);

            injector::WriteMemory<float> (itemAddress + 0x40, values[2]);

            injector::WriteMemory<float> (itemAddress + 0x44, values[0]);

            injector::WriteMemory<float> (itemAddress + 0x48, values[1]);

            injector::WriteMemory<float> (itemAddress + 0x4C, values[2]);

            injector::WriteMemory<float> (itemAddress + 0x50, values[0]);

            injector::WriteMemory<float> (itemAddress + 0x54, values[1]);
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
