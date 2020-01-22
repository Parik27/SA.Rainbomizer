#pragma once

#include <vector>
#include <string>
#include <array>

struct COffset
{
    float x;
    float y;
    float z;
    int   interior;
    int   extraCol;
};

class CutsceneRandomizer
{
    static CutsceneRandomizer *mInstance;

    std::vector<std::vector<std::string>> mModels;
    std::vector<COffset>                  mOffsets;

    CutsceneRandomizer (){};
    static void DestroyInstance ();

public:
    int originalLevel;

    /// Returns the static instance for CutsceneRandomizer.
    static CutsceneRandomizer *GetInstance ();

    const char *GetRandomModel (std::string model);

    COffset GetRandomOffset (float x, float y, float z);

    /// Initialises Hooks/etc.
    void Initialise ();
};
