#pragma once

#include <vector>
#include <cstdint>

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

    static void RandomizePlayerClothes ();
    static void RandomizePlayerModel ();
    static void HandleClothesChange ();

public:
    /// Returns the static instance for ClothesRandomizer.
    static ClothesRandomizer *GetInstance ();

    std::pair<int, int> GetRandomCRCForComponent (int componentId);

    /// Initialises Hooks/etc.
    void Initialise ();
};
