#include "clothes.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "injector/calling.hpp"
#include "functions.hh"
#include "util/scrpt.hh"
#include <windows.h>
#include "config.hh"

ClothesRandomizer *ClothesRandomizer::mInstance = nullptr;

/*******************************************************/
void
HandleClothesChange ()
{
    static int prevFadeValue = -1;
    int        fadeValue     = injector::ReadMemory<uint8_t> (0xC3EFAB);

    if (prevFadeValue != fadeValue && fadeValue == 255)
        {
            for (int i = 0; i < 17; i++)
                {
                    auto cloth = ClothesRandomizer::GetInstance ()
                                     ->GetRandomCRCForComponent (i);

                    Scrpt::CallOpcode (0x784, "set_player_model_tex_crc",
                                       GlobalVar (2), cloth.second, cloth.first,
                                       i);
                    Scrpt::CallOpcode (0x070D, "rebuild_player", GlobalVar (2));
                }
        }

    prevFadeValue = fadeValue;

    HookManager::CallOriginal<injector::cstd<void ()>, 0x53EB9D> ();
}

/*******************************************************/
void
ClothesRandomizer::InitialiseClothes ()
{
    std::vector<std::string> shops
        = {"CSchp", "CSsprt",  "LACS1",   "clothgp", "Csdesgn",
           "Csexl", "barbers", "barber2", "barber3"};

    for (auto i : shops)
        {
            CShopping::LoadShop (i.c_str ());
            for (int i = 0; i < CShopping::m_nTotalItems; i++)
                {
                    auto &item = CShopping::m_aShoppingItems[i];
                    if (item.modelType >= 0 && item.modelType <= 17)
                        {
                            mClothes[item.modelType].push_back (
                                std::make_pair (item.modelName,
                                                item.textureName));
                        }
                }
        }

    mInitialised = true;
}

/*******************************************************/
std::pair<int, int>
ClothesRandomizer::GetRandomCRCForComponent (int componentId)
{
    if (!mInitialised)
        InitialiseClothes ();

    int randomId = random (mClothes[componentId].size ()) - 1;
    if (randomId < 0)
        return {0, 0};

    return mClothes[componentId][randomId];
}

/*******************************************************/
void
ClothesRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().clothes;
    if (!config.enabled)
        return;

    mInitialised = false;

    RegisterHooks ({{HOOK_CALL, 0x53EB9D, (void *) HandleClothesChange}});

    Logger::GetLogger ()->LogMessage ("Intialised ClothesRandomizer");
}

/*******************************************************/
void
ClothesRandomizer::DestroyInstance ()
{
    if (ClothesRandomizer::mInstance)
        delete ClothesRandomizer::mInstance;
}

/*******************************************************/
ClothesRandomizer *
ClothesRandomizer::GetInstance ()
{
    if (!ClothesRandomizer::mInstance)
        {
            ClothesRandomizer::mInstance = new ClothesRandomizer ();
            atexit (&ClothesRandomizer::DestroyInstance);
        }
    return ClothesRandomizer::mInstance;
}
