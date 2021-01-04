
#pragma once
#include "functions.hh"
#include <string>
#include <vector>

class PedRandomizer
{
	static PedRandomizer* mInstance;

	PedRandomizer() {};
	static void DestroyInstance();

	static void __fastcall RandomizeGenericPeds(CCivilianPed* ped, void* edx, ePedType type, int model);
    static void __fastcall RandomizeCopPeds (CPed* ped, void* edx, int modelIndex);
    static int ChooseRandomPedToLoad ();
    static void RandomizeSpecialModels (int slot, const char *modelName, int flags);

public:
	/// Returns the static instance for PedRandomizer.
	static PedRandomizer* GetInstance();

	/// Initialises Hooks/etc.
	void Initialise();

    static std::vector<std::string> special_models;
    static bool IsSpecialModel (int model);
    static bool IsModelBlacklisted (int model);
};
