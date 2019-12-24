#include <string>
#include <vector>
#include <unordered_map>

#pragma once

struct SoundPair
{
    int         id;
    std::string name;
};

struct SFXPair
{
    int         bank;
    int         sfx;
    std::string subtitle;
};

struct SlotSoundPair
{
    int oldSound = -1;
    int newSound = -1;
};

struct CAudioEngine;
struct CText;
struct CAEMp3BankLoader;

void __fastcall RandomizeAudioLoad (CAudioEngine *audio, void *edx,
                                    unsigned char slot, int id);
char *__fastcall RemoveSubtitlesHook (CText *TheText, void *edx, char *key);
char __fastcall InitialiseLoopedSoundList (CAEMp3BankLoader *thisLoader);

class SoundRandomizer
{
    static SoundRandomizer *mInstance;

    std::unordered_map<int, std::string> mSubtitles;
    std::unordered_map<std::string, int> mPreviousPairs;
    SlotSoundPair                        slots[5];
    std::string                          mPrevOverridenText;
    std::string                          mPrevOverridenKey;
    int                                  mCurrentReplacedSubIndex = 0;

    void InitaliseSoundTable ();

    SoundRandomizer (){};
    static void DestroyInstance ();

    std::string GetNewSubtitleForEvent (int event, const SFXPair &pair);

public:
    std::vector<SFXPair> mBankLkups;
    std::string          mReplacedSubtitles[10];

    /// Returns the static instance for SoundRandomizer.
    static SoundRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    /// Returns random pair map
    std::unordered_map<std::string, int> &GetPreviousPairs ();

    /// Returns subtitle by audio event id
    std::string GetSubtitleByID (int id);

    /// Calculates the sound event id from slot and bank id's
    static int CalculateEventFromSFX (int bank, int sfx);

    const std::string &GetPreviousOverridenText ();
    const std::string &GetPreviousOverridenKey ();

    /// Updates the last randomized sound subtitle
    void UpdatePreviousPairs (int event, const SFXPair &pair);

    void SetPreviousOverridenText (const std::string &key,
                                   const std::string &text);

    friend char __fastcall InitialiseLoopedSoundList (
        CAEMp3BankLoader *thisLoader);

    friend void CorrectSubtitles (char *string, int time, int16_t flags,
                                  char bAddToPreviousBrief);
};
