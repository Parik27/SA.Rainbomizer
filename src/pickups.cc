#include "pickups.hh"
#include <cstdint>
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "config.hh"
#include "weapons.hh"
#include <injector/injector.hpp>
#include <algorithm>
#include <vector>
#include "util/scrpt.hh"
#include "hooks.hh"

using namespace Rainbomizer;

PickupsRandomizer *PickupsRandomizer::mInstance = nullptr;

static std::unordered_map<int, int> weaponToModel
    = {{331, 1},  {333, 2},  {334, 3},  {335, 4},  {336, 5},  {337, 6},
       {338, 7},  {339, 8},  {341, 9},  {321, 10}, {322, 11}, {323, 12},
       {324, 13}, {325, 14}, {326, 15}, {342, 16}, {343, 17}, {344, 18},
       {346, 22}, {347, 23}, {348, 24}, {349, 25}, {350, 26}, {351, 27},
       {352, 28}, {353, 29}, {355, 30}, {356, 31}, {372, 32}, {357, 33},
       {358, 34}, {359, 35}, {360, 36}, {361, 37}, {362, 38}, {363, 39},
       {364, 40}, {365, 41}, {366, 42}, {367, 43}, {368, 44}, {369, 45},
       {371, 46}};

/*******************************************************/
void
PlayPickupSound (int soundType)
{
    CallMethod<0x506EA0> (0xB6BC90, soundType, 0.0f, 1.0f); // Play sound
}

/*******************************************************/
unsigned int
SelectRandomPickup (unsigned int modelId, bool isDeadPed)
{
    std::vector<int> additional_pickups
        = {370,  1240, 1240, 1240, 1242, 1242, 1242,
           1247, 1247, 1241, 1241, 1581, 2894, 1550};

    if (isDeadPed)
        additional_pickups.push_back (1210);

    if (modelId != 1212 && modelId != 953 && modelId != 954 && modelId != 1253
        && modelId != 370 && modelId != 1277)
        {
            if (weaponToModel.find (modelId) != weaponToModel.end ())
                {
                    modelId = weaponToModel[modelId];
                }
            modelId
                = WeaponRandomizer::GetInstance ()->GetRandomWeapon (nullptr,
                                                                     modelId,
                                                                     true);
            if (modelId >= 47 || modelId == 20 || modelId == 21)
                {
                    modelId = additional_pickups[random (
                        additional_pickups.size () - 1)];
                }
            else if (find (additional_pickups.begin (),
                           additional_pickups.end (), modelId)
                     == additional_pickups.end ())
                modelId = GetWeaponInfo (modelId, 1)[3];
        }
    return modelId;
}

enum ePickupType {
    PICKUP_NONE = 0,
    PICKUP_IN_SHOP = 1,
    PICKUP_ON_STREET = 2,
    PICKUP_ONCE = 3,
    PICKUP_ONCE_TIMEOUT = 4,
    PICKUP_ONCE_TIMEOUT_SLOW = 5,
    PICKUP_COLLECTABLE1 = 6,
    PICKUP_IN_SHOP_OUT_OF_STOCK = 7,
    PICKUP_MONEY = 8,
    PICKUP_MINE_INACTIVE = 9,
    PICKUP_MINE_ARMED = 10,
    PICKUP_NAUTICAL_MINE_INACTIVE = 11,
    PICKUP_NAUTICAL_MINE_ARMED = 12,
    PICKUP_FLOATINGPACKAGE = 13,
    PICKUP_FLOATINGPACKAGE_FLOATING = 14,
    PICKUP_ON_STREET_SLOW = 15,
    PICKUP_ASSET_REVENUE = 16,
    PICKUP_PROPERTY_LOCKED = 17,
    PICKUP_PROPERTY_FORSALE = 18,
    PICKUP_MONEY_DOESNTDISAPPEAR = 19,
    PICKUP_SNAPSHOT = 20,
    PICKUP_2P = 21,
    PICKUP_ONCE_FOR_MISSION = 22
};

/*******************************************************/
bool
ShouldRandomize (char pickupType, int model)
{
    if (model == 1277)
        return false;
    
    switch (pickupType)
        {
        case PICKUP_ON_STREET:
        case PICKUP_ONCE:
        case PICKUP_COLLECTABLE1:
        case PICKUP_ON_STREET_SLOW:
        case PICKUP_ASSET_REVENUE:
        case PICKUP_PROPERTY_FORSALE:
        case PICKUP_SNAPSHOT:
            return true;
        }

    return false;
}

/*******************************************************/
bool
Equal (CCompressedVector vec, float x, float y, float z)
{
    return int(vec.GetX ()) == int(x) && int(vec.GetY ()) == int(y) && int(vec.GetZ ()) == z;                
}

/*******************************************************/
void
ShuffleAllCollectables ()
{
    static uint16_t &miCjOyster     = *reinterpret_cast<uint16_t *> (0x8CD750);
    static uint16_t &miCjHorseShoe  = *reinterpret_cast<uint16_t *> (0x8CD754);
    static uint16_t &miCameraPickup = *reinterpret_cast<uint16_t *> (0x8CD5D8);

    std::vector<CPickup *> collectablePickups;

    const int NUM_PICKUPS = 0x26C;
    for (int i = 0; i < NUM_PICKUPS; i++)
        {
            auto &pickup = aPickups[i];

            if (ShouldRandomize (pickup.m_PickupType, pickup.m_wModelId) &&
                pickup.m_vPos.GetZ () < 95.0f && !Equal (pickup.m_vPos, -2120.0, 96.39, 39.0) &&
                !Equal (pickup.m_vPos, -2094.0, -488.0, 36.0))
                collectablePickups.push_back (&pickup);

            continue;
            
            if (pickup.m_wModelId == miCjOyster
                || pickup.m_wModelId == miCjHorseShoe)
                collectablePickups.push_back (&pickup);

            // Don't randomize camera pickups over height limit.
            if (pickup.m_wModelId == miCameraPickup)
                if (pickup.m_vPos.GetZ () < 95.0f)
                    collectablePickups.push_back (&pickup);
        }

    for (auto pickupA : collectablePickups)
        {
            auto pickupB = GetRandomElement (collectablePickups);
            std::swap (pickupA->m_vPos, pickupB->m_vPos);
        }
}

/*******************************************************/
int
RandomizeCollectable (FunctionCb<int> CPickups_GenerateNewOne, float x, float y,
                      float z, unsigned int modelId, char pickupType, int ammo,
                      unsigned int moneyPerDay, char isEmpty, char *message)
{
    int ret = CPickups_GenerateNewOne (x, y, z, modelId, pickupType, ammo,
                                       moneyPerDay, isEmpty, message);

    ShuffleAllCollectables ();

    return ret;
}

/*******************************************************/
int
RandomizePickup (FunctionCb<int> CPickups_GenerateNewOne, float x, float y,
                 float z, unsigned int modelId, char pickupType, int ammo,
                 unsigned int moneyPerDay, char isEmpty, char *message)
{
    if (PickupsRandomizer::m_Config.ReplaceWithWeaponsOnly)
        {
            if (weaponToModel.find (modelId) != weaponToModel.end ())
                modelId = SelectRandomPickup (modelId, false);
        }
    else
        modelId = SelectRandomPickup (modelId, false);

    int ret = CPickups_GenerateNewOne (x, y, z, modelId, pickupType, ammo,
                                    moneyPerDay, isEmpty, message);

    if (ShouldRandomize (pickupType, modelId))
        ShuffleAllCollectables();

    return ret;
}

/*******************************************************/
int
InitialiseCacheForPickupRandomization (void *fileName)
{
    WeaponRandomizer::GetInstance ()->CachePatterns ();
    return CGame::Init2 (fileName);
}

/*******************************************************/
int
RandomizeWeaponPickup (float x, float y, float z, unsigned int weaponType,
                       char pickupType, int ammo, char isEmpty, char *message)
{
    weaponType = SelectRandomPickup (weaponType, true);
    return CPickups::GenerateNewOne (x, y, z, weaponType, pickupType, ammo, 0,
                                     isEmpty, message);
}

/*******************************************************/
bool
GiveMoneyForBriefcase (unsigned short model, int plrIndex)
{
    if (model == 1210)
        {
            if (FindPlayerPed ())
                {
                    PlayPickupSound (7);
                    Scrpt::CallOpcode (0x109, "add_score", GlobalVar (2),
                                       random (1, 500));
                }
        }
    else if (model == 1550)
        {
            if (FindPlayerPed ())
                {
                    PlayPickupSound (7);
                    Scrpt::CallOpcode (0x109, "add_score", GlobalVar (2),
                                       random (1000, 10000));
                }
        }
    return CallAndReturn<bool, 0x4564F0> (
        model, plrIndex); // GivePlayerGoodiesWithPickupMI
}

/*******************************************************/
void
PickupsRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "PickupsRandomizer",
            std::pair ("RandomizePedWeaponDrops", &m_Config.RandomizeDeadPed),
            std::pair ("ReplaceWithWeaponsOnly",
                       &m_Config.ReplaceWithWeaponsOnly),
            std::pair ("RandomizeCollectables", &m_Config.RandomizeCollectables),
            std::pair ("MoneyFromRandomPickups", &m_Config.MoneyFromPickups),
            std::pair ("SkipChecks", &m_Config.SkipChecks)))
        return;

    HooksManager::Add<&RandomizePickup, 0x00445098, 0x00445AFD, 0x00458A58,
                      0x0047E636, 0x480658, 0x00481744, 0x0048B243, 0x48CEB9,
                      0x00592103, 0x005B49C6, 0x0067B6DE> ();

    if (m_Config.RandomizeCollectables)
        HooksManager::Add<&RandomizeCollectable, 0x477983> ();

    if (m_Config.RandomizeDeadPed)
        injector::MakeCALL (0x4592F7, RandomizeWeaponPickup);

    injector::MakeCALL (0x5921B5, RandomizeWeaponPickup);

    if (m_Config.MoneyFromPickups)
        {
            for (int address : {0x43989F, 0x457DF2, 0x457F91})
                injector::MakeCALL (address, GiveMoneyForBriefcase);
        }

    if (!m_Config.SkipChecks)
        injector::MakeCALL (0x53BCA6, InitialiseCacheForPickupRandomization);

    Logger::GetLogger ()->LogMessage ("Intialised PickupsRandomizer");
}

/*******************************************************/
void
PickupsRandomizer::DestroyInstance ()
{
    if (PickupsRandomizer::mInstance)
        delete PickupsRandomizer::mInstance;
}

/*******************************************************/
PickupsRandomizer *
PickupsRandomizer::GetInstance ()
{
    if (!PickupsRandomizer::mInstance)
        {
            PickupsRandomizer::mInstance = new PickupsRandomizer ();
            atexit (&PickupsRandomizer::DestroyInstance);
        }
    return PickupsRandomizer::mInstance;
}
