
#pragma once
#include "functions.hh"
#include <string>
#include <vector>

class PedRandomizer
{
    static PedRandomizer *mInstance;

    PedRandomizer (){};
    static void DestroyInstance ();

	inline static std::vector<std::string> nsfw_models
            = {"GANGRL1", "MECGRL1", "GUNGRL1", "COPGRL1",
               "NURGRL1", "CROGRL1", "GANGRL2", "COPGRL2"};

    static void __fastcall RandomizeGenericPeds (CCivilianPed *ped, void *edx,
                                                 ePedType type, int model);
    static void __fastcall RandomizeCopPeds (CPed *ped, void *edx,
                                             int modelIndex);
    static int  ChooseRandomPedToLoad ();
    static void RandomizeSpecialModels (int slot, const char *modelName,
                                        int flags);

public:
    /// Returns the static instance for PedRandomizer.
    static PedRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

		inline static std::vector<std::string> special_models
        = {"TRUTH",  "MACCER", "TENPEN",  "PULASKI", "HERN",    "DWAYNE",
           "SMOKE",  "SWEET",  "RYDER",   "FORELLI", "ROSE",    "PAUL",
           "CESAR",  "OGLOC",  "WUZIMU",  "TORINO",  "JIZZY",   "MADDOGG",
           "CAT",    "CLAUDE", "RYDER2",  "RYDER3",  "EMMET",   "ANDRE",
           "KENDL",  "JETHRO", "ZERO",    "TBONE",   "SINDACO", "JANITOR",
           "BBTHIN", "SMOKEV", "GUNGRL2", "NURGRL2", "CROGRL2", "BB",
           "SUZIE",  "PSYCHO"};
    static bool                     IsSpecialModel (int model);
    static bool                     IsModelBlacklisted (int model);
};
