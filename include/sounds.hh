#include <string>
#include <vector>
#include <unordered_map>

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
	std::unordered_map<std::string, int> mPreviousPairs;
	
	void InitaliseSoundTable();
	
    SoundRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for SoundRandomizer.
    static SoundRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

	/// Returns a random sound pair
	SoundPair GetRandomPair(int &index);

	/// Returns random pair map
	std::unordered_map<std::string, int>& GetPreviousPairs();
	
	/// Returns sound pair by array index
	SoundPair GetPairByIndex(int index);

	/// Returns sound pair by ID
	SoundPair GetPairByID(int id);
};
