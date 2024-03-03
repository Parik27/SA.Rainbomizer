
#pragma once
#include "functions.hh"
#include <cstdint>
#include <string>
#include <vector>

class PedRandomizer
{
    static PedRandomizer *mInstance;

    PedRandomizer (){};
    static void DestroyInstance ();

    static uint32_t GetRandomModelIndex (uint32_t originalIdx);

    static void __fastcall RandomizePedModelIndex (CEntity *entity, void *,
                                                   uint32_t index);
    static int  ChooseRandomPedToLoad ();
    static void RandomizeSpecialModels (int slot, const char *modelName,
                                        int flags);

public:
    /// Returns the static instance for PedRandomizer.
    static PedRandomizer *GetInstance ();

    static inline struct Config
    {
        bool RandomizeGenericModels;
        bool RandomizeCops;
        bool RandomizeGangMembers;
        bool RandomizeSpecialModels;
        bool IncludeNSFWModels;

        int         ForcedPed;
        std::string ForcedSpecial;
    } m_Config;

    /// Initialises Hooks/etc.
    void Initialise ();

    inline static std::vector<std::string> specialModels{};
    static bool                     IsSpecialModel (int model);
    static bool                     IsModelValidPedModel (int model);
    static inline bool              isSkinSelect = false;
};
