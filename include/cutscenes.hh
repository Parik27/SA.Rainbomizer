#pragma once

#include <vector>
#include <string>

class CutsceneRandomizer
{
    static CutsceneRandomizer *mInstance;

    std::vector<std::vector<std::string>> mModels;

    CutsceneRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for CutsceneRandomizer.
    static CutsceneRandomizer *GetInstance ();

    const char *GetRandomModel (std::string model);

    /// Initialises Hooks/etc.
    void Initialise ();
};
