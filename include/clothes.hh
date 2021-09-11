#pragma once

#include <vector>
#include <cstdint>
#include <string>

class ClothesRandomizer
{
    static ClothesRandomizer *       mInstance;
    std::vector<std::pair<int, int>> mClothes[18];

    bool mInitialised = false;

    ClothesRandomizer (){};
    static void DestroyInstance ();

    void InitialiseClothes ();

    static void FixChangingClothes (int modelId, uint32_t *newClothes,
                                    uint32_t *oldClothes, bool CutscenePlayer);

    static int __fastcall FixAnimCrash (uint32_t *anim, void *edx, int arg0,
                                        int animGroup);

    static void RandomizePlayerClothes ();
    static void RandomizePlayerModel ();
    static void HandleClothesChange ();

public:
    /// Returns the static instance for ClothesRandomizer.
    static ClothesRandomizer *GetInstance ();

    static inline struct Config
    {
        bool RandomizePlayerModel;
        bool RandomizePlayerClothing;

        int OddsOfNewModel;

        bool IncludeNSFWModels;
        bool RandomizePlayerOnce;

        int         ForcedModel;
        std::string ForcedSpecial;
    } m_Config;

    std::pair<int, int> GetRandomCRCForComponent (int componentId);

    static inline struct RandomizeOnceData
    {
        bool Initialised = false;

        bool                             isClothes;
        std::vector<std::pair<int, int>> RandomClothes;
        int                              ChosenModel;
        std::string                      SpecialModel;

        RandomizeOnceData (){};

    } m_RandomizeOnceInfo;

    /// Initialises Hooks/etc.
    void Initialise ();
};
