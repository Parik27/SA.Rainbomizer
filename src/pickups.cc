#include "pickups.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "config.hh"
#include "weapons.hh"
#include <injector/injector.hpp>
#include <algorithm>
#include <vector>

// MODIFY PICKUPS TO SHUFFLE USING CPickups:aPickups ARRAY. COORDS CAN ONLY BE COMPARED / CHANGED THROUGH THE COBJECT POINTERS

PickupsRandomizer *PickupsRandomizer::mInstance = nullptr;

static std::unordered_map<int, int> weaponToModel = {
       {331, 1},  {333, 2},  {334, 3},  {335, 4},  {336, 5},  {337, 6},
       {338, 7},  {339, 8},  {341, 9},  {321, 10}, {322, 11}, {323, 12},
       {324, 13}, {325, 14}, {326, 15}, {342, 16}, {343, 17}, {344, 18},
       {346, 22}, {347, 23}, {348, 24}, {349, 25}, {350, 26}, {351, 27},
       {352, 28}, {353, 29}, {355, 30}, {356, 31}, {372, 32}, {357, 33},
       {358, 34}, {359, 35}, {360, 36}, {361, 37}, {362, 38}, {363, 39},
       {364, 40}, {365, 41}, {366, 42}, {367, 43}, {368, 44}, {369, 45},
       {371, 46}};

struct Pickup
{
    int ID;
    CPickup pickup;
};

/*******************************************************/
std::vector<int> PickupsRandomizer::additional_pickups
    = {1240, 1242, 1247, 1241};
/*******************************************************/
int
RandomizePickup (float x, float y, float z, unsigned int modelId,
                 char pickupType, int ammo, unsigned int moneyPerDay,
                 char isEmpty, char *message)
{
    //std::vector<Pickup> pickups;

    //for (int i = 0; i < 620; i++)
    //{
    //    CPickup *pickup = aPickups + (i * 32);
    //    if (!pickup->m_PickupType || pickup->m_PickupType == 4
    //        || pickup->m_PickupType == 5 || pickup->m_PickupType == 8)
    //        continue;
    //    pickups.push_back ({i, *pickup});
    //}

    //if (!pickups.empty())
    //{
    //        Pickup  otherPickup = GetRandomElement (pickups);
    //        CVector pos1
    //            = otherPickup.pickup.m_pObject->m_SimpleTransform.m_vPosn;
    //        CVector pos2                                            = {x, y, z};
    //        otherPickup.pickup.m_pObject->m_SimpleTransform.m_vPosn = pos2;
    //        x                                                       = pos1.x;
    //        y                                                       = pos1.y;
    //        z                                                       = pos1.z;
    //}

    if (modelId != 1212 && modelId != 953 && modelId != 954 && modelId != 1253
        && modelId != 370 && modelId != 1277)
        {
            if (weaponToModel.find (modelId) != weaponToModel.end ())
            {
                modelId = weaponToModel[modelId];
            }
             modelId = WeaponRandomizer::GetInstance ()
                ->GetRandomWeapon (nullptr, modelId, true);
            if (modelId == 47 || modelId == 20 || modelId == 21)
                {
                    modelId = PickupsRandomizer::additional_pickups[random (
                        PickupsRandomizer::additional_pickups.size () - 1)];
                }
            else if (find (PickupsRandomizer::additional_pickups.begin (),
                           PickupsRandomizer::additional_pickups.end (),
                           modelId)
                     == PickupsRandomizer::additional_pickups.end ())
                modelId = GetWeaponInfo (modelId, 1)[3];
        }

    return CPickups::GenerateNewOne (x, y, z, modelId, pickupType, ammo,
                                     moneyPerDay, isEmpty, message);
}

/*******************************************************/
int
InitialiseCacheForPickupRandomization (void *fileName)
{
    Logger::GetLogger ()->LogMessage ("Pickup Cache Hook");
     WeaponRandomizer::GetInstance()->CachePatterns ();
    return CGame::Init2 (fileName);
}

/*******************************************************/
void
PickupsRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("PickupsRandomizer"))
        return;

    for (int address :
         {0x00445098, 0x00445AFD, 0x00458A58, 0x00477983, 0x0047E636, 0x480658,
          0x00481744, 0x0048B243, 0x48CEB9, 0x00592103, 0x005B49C6, 0x0067B6DE})
        {
            injector::MakeCALL (address, (void *) &RandomizePickup);
        }

    // Dead peds
    injector::MakeCALL (0x4573C2, &RandomizePickup);
    //injector::MakeCALL (0x156DF52, &RandomizePickup);

    injector::MakeCALL (0x53BCA6, (void *) &InitialiseCacheForPickupRandomization);

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
