#pragma once

#include <vector>

class ClothesRandomizer
{
    static ClothesRandomizer *mInstance;
    std::vector<std::pair<int, int>> mClothes[18];

    bool mInitialised = false;
    
    ClothesRandomizer (){};
    static void DestroyInstance ();

    void InitialiseClothes();
    
public:
    /// Returns the static instance for ClothesRandomizer.
    static ClothesRandomizer *GetInstance ();

    std::pair<int, int> GetRandomCRCForComponent(int componentId);
    
    /// Initialises Hooks/etc.
    void Initialise ();
};
