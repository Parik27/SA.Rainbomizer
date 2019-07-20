#include <string>
#include <vector>

struct SoundPair
{
	int id;
	std::string name;
};

#pragma once

struct CAudioEngine;
struct CText;

void __fastcall RandomizeAudioLoad (CAudioEngine *audio, void *edx,
                                    unsigned char slot, int id);
char* __fastcall CorrectSubtitles(CText* TheText, void* edx, char* key);

class SoundRandomizer
{
    static SoundRandomizer *mInstance;

	std::vector<SoundPair> mSoundTable;
	std::vector<int> mPreviousPairs;
	
	void InitaliseSoundTable();
	
    SoundRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for SoundRandomizer.
    static SoundRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

	/// Returns a random sound pair
	SoundPair GetRandomPair();

	/// Returns previous sound pair
	int GetPreviousPairID();

	/// Returns previous sound pair
	SoundPair GetPairByID(int id);
};
