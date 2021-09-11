#pragma once

#include <vector>
#include <string>
#include <array>

struct COffset
{
    float x;
    float y;
    float z;
};

class CutsceneRandomizer
{
    static CutsceneRandomizer *mInstance;

    std::vector<std::vector<std::string>> mModels;

    std::string mLastModel;

    CutsceneRandomizer (){};
    static void DestroyInstance ();

public:
    int     originalLevel;
    COffset offset;

    static inline struct Config
    {
        bool RandomizeModels;
        bool NoBrokenJaws;
        bool RandomizeLocation;
        bool RandomizeCutsceneToPlay;
    } m_Config;

    /// Returns the static instance for CutsceneRandomizer.
    static CutsceneRandomizer *GetInstance ();

    char *GetRandomModel (std::string model);

    COffset GetRandomOffset (float x, float y, float z);

    /// Initialises Hooks/etc.
    void Initialise ();
};
