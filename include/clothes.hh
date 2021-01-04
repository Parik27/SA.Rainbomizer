#pragma once

#include <vector>

class ClothesRandomizer
{
    static ClothesRandomizer *       mInstance;
    std::vector<std::pair<int, int>> mClothes[18];

    bool mInitialised = false;

    ClothesRandomizer (){};
    static void DestroyInstance ();

    void InitialiseClothes ();

    static void FixChangingClothes (int modelId, uint32_t* newClothes, uint32_t* oldClothes, bool CutscenePlayer);

public:
    /// Returns the static instance for ClothesRandomizer.
    static ClothesRandomizer *GetInstance ();

    std::pair<int, int> GetRandomCRCForComponent (int componentId);

    /// Initialises Hooks/etc.
    void Initialise ();
};
