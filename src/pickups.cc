#include "pickups.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "config.hh"
#include "weapons.hh"
#include <injector/injector.hpp>

PickupsRandomizer *PickupsRandomizer::mInstance = nullptr;

/*******************************************************/
int
RandomizePickup (float x, float y, float z, unsigned int modelId,
                 char pickupType, int ammo, unsigned int moneyPerDay,
                 char isEmpty, char *message)
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().pickups;

    if (ammo && modelId != 1212 && modelId != 367 && modelId != 344
        && modelId != 366)
        {
            modelId
                = WeaponRandomizer::GetInstance ()->GetRandomWeapon (nullptr,
                                                                     0);
            modelId = GetWeaponInfo (modelId, 1)[3];
        }

    return CPickups::GenerateNewOne (x, y, z, modelId, pickupType, ammo,
                                     moneyPerDay, isEmpty, message);
}

/*******************************************************/
void
PickupsRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().pickups;

    if (!config.enabled)
        return;

    for (int address :
         {0x00445098, 0x00445AFD, 0x004573C2, 0x00458A58, 0x00477983,
          0x0047E636, 0x480658, 0x00481744, 0x0048B243, 0x48CEB9, 0x00592103,
          0x005B49C6, 0x0067B6DE})
        {
            injector::MakeCALL (address, (void *) &RandomizePickup);
        }

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
