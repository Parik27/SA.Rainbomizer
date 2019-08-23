#include <string>
#include <vector>
#include <unordered_map>

struct SoundPair
{
    int         id;
    std::string name;
};

struct SlotSoundPair
{
    int oldSound = -1;
    int newSound = -1;
};

#pragma once

struct CAudioEngine;
struct CText;

void __fastcall RandomizeAudioLoad (CAudioEngine *audio, void *edx,
                                    unsigned char slot, int id);
char *__fastcall CorrectSubtitles (CText *TheText, void *edx, char *key);
char *__fastcall RemoveSubtitlesHook (CText *TheText, void *edx, char *key);

class SoundRandomizer
{
    static SoundRandomizer *mInstance;

    std::vector<SoundPair>               mSoundTable;
    std::unordered_map<std::string, int> mPreviousPairs;
    SlotSoundPair                        slots[5];
    std::string                          mPrevOverridenText;
    std::string                          mPrevOverridenKey;

    void InitaliseSoundTable ();

    SoundRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for SoundRandomizer.
    static SoundRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    /// Returns a random sound pair
    SoundPair GetRandomPair (int &index, int slot, int id);

    /// Returns random pair map
    std::unordered_map<std::string, int> &GetPreviousPairs ();

    /// Returns sound pair by array index
    SoundPair GetPairByIndex (int index);

    /// Returns sound pair by ID
    SoundPair GetPairByID (int id);

    const std::string &GetPreviousOverridenText ();
    const std::string &GetPreviousOverridenKey ();

    void SetPreviousOverridenText (const std::string &key,
                                   const std::string &text);
};
