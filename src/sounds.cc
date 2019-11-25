#include "sounds.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include <cctype>
#include "text.hh"
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include "config.hh"

SoundRandomizer *SoundRandomizer::mInstance = nullptr;

/*******************************************************/
bool __fastcall AudioHasFinishedHook (CAudioEngine *audio, void *edx,
                                      uint8_t id)
{
    bool status = audio->IsMissionAudioSampleFinished (id);
    printf ("%s\n", status ? "True" : "False");
    return status;
}

/*******************************************************/
void __fastcall RandomizeAudioLoad (CAudioEngine *audio, void *edx,
                                    unsigned char slot, int id)
{
    auto soundsR = SoundRandomizer::GetInstance ();
    try
        {
            int  newIndex;
            auto newSoundPair  = soundsR->GetRandomPair (newIndex, slot, id);
            auto prevSoundPair = soundsR->GetPairByID (id);

            audio->PreloadMissionAudio (slot, newSoundPair.id);
            soundsR->GetPreviousPairs ()[prevSoundPair.name] = newIndex;
        }
    catch (std::exception e)
        {
            return audio->PreloadMissionAudio (slot, id);
        }
}

/*******************************************************/
void __fastcall InitialiseTexts (CText *text, void *edx, char a2)
{
    text->Load (a2);
    GxtManager::Initialise (text);
}

/*******************************************************/
char *__fastcall RemoveSubtitlesHook (CText *TheText, void *edx, char *key)
{
    auto soundsR = SoundRandomizer::GetInstance ();
    if (soundsR->GetPreviousOverridenKey () == key)
        return (char *) soundsR->GetPreviousOverridenText ().c_str ();
    return TheText->Get (key);
}

/*******************************************************/
char *__fastcall CorrectSubtitles (CText *text, void *edx, char *key)
{
    auto soundsR = SoundRandomizer::GetInstance ();
    auto config  = ConfigManager::GetInstance ()->GetConfigs ().sounds;

    if (!config.matchSubtitles)
        return text->Get (key);

    try
        {
            auto prevIndex = soundsR->GetPreviousPairs ().at (key);
            soundsR->GetPreviousPairs ().erase (key);

            auto soundPair
                = SoundRandomizer::GetInstance ()->GetPairByIndex (prevIndex);

            auto text = (char *) GxtManager::GetText (soundPair.name);
            soundsR->SetPreviousOverridenText (key, text);

            return text;
        }
    catch (std::out_of_range e)
        {
            return text->Get (key);
        }
}

/*******************************************************/
const std::string &
SoundRandomizer::GetPreviousOverridenText ()
{
    return this->mPrevOverridenText;
}

/*******************************************************/
const std::string &
SoundRandomizer::GetPreviousOverridenKey ()
{
    return this->mPrevOverridenKey;
}

/*******************************************************/
void
SoundRandomizer::SetPreviousOverridenText (const std::string &key,
                                           const std::string &text)
{
    this->mPrevOverridenKey  = key;
    this->mPrevOverridenText = text;
}

/*******************************************************/
std::unordered_map<std::string, int> &
SoundRandomizer::GetPreviousPairs ()
{
    return mPreviousPairs;
}

/*******************************************************/
SoundPair
SoundRandomizer::GetRandomPair (int &index, int slot, int id)
{
    if (slots[slot].oldSound == id)
        {
            index = slots[slot].newSound;
            return mSoundTable[index];
        }

    int randomID = random (mSoundTable.size () - 1);

    index                = randomID;
    slots[slot].oldSound = id;
    slots[slot].newSound = randomID;

    return mSoundTable[randomID];
}

/*******************************************************/
SoundPair
SoundRandomizer::GetPairByIndex (int id)
{
    return mSoundTable[id];
}

/*******************************************************/
SoundPair
SoundRandomizer::GetPairByID (int id)
{
    auto iter = std::find_if (mSoundTable.begin (), mSoundTable.end (),
                              [id] (const SoundPair &a) { return a.id == id; });
    return *iter;
}

/*******************************************************/
void
SoundRandomizer::InitaliseSoundTable ()
{
    FILE *audioEventsTable
        = fopen (GetGameDirRelativePathA ("data/AudioEvents.txt"), "r");

    if (!audioEventsTable)
        {
            Logger::GetLogger ()->LogMessage ("Unable to load AudioEvents.txt");
            return;
        }

    char line[256] = {0};
    while (fgets (line, 256, audioEventsTable))
        {
            if (!isalpha (line[0]))
                continue;

            char name[128] = {0};
            int  id        = -1;

            sscanf (line, " %s %d ", name, &id);
            if (id >= 2000)
                mSoundTable.push_back ({id, name + 6});
        }
}

/*******************************************************/
void
SoundRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().sounds;
    if (!config.enabled)
        return;

    Logger::GetLogger ()->LogMessage ("Intialised SoundRandomizer");
    RegisterHooks ({{HOOK_CALL, 0x4851BB, (void *) &RandomizeAudioLoad},
                    {HOOK_CALL, 0x468173, (void *) &CorrectSubtitles},
                    {HOOK_CALL, 0x4680E7, (void *) &CorrectSubtitles},
                    {HOOK_CALL, 0x485397, (void *) &RemoveSubtitlesHook},
                    {HOOK_CALL, 0x468E9A, (void *) &InitialiseTexts},
                    {HOOK_CALL, 0x618E97, (void *) &InitialiseTexts},
                    {HOOK_CALL, 0x5BA167, (void *) &InitialiseTexts}});
    InitaliseSoundTable ();
}

/*******************************************************/
void
SoundRandomizer::DestroyInstance ()
{
    if (SoundRandomizer::mInstance)
        delete SoundRandomizer::mInstance;
}

/*******************************************************/
SoundRandomizer *
SoundRandomizer::GetInstance ()
{
    if (!SoundRandomizer::mInstance)
        {
            SoundRandomizer::mInstance = new SoundRandomizer ();
            atexit (&SoundRandomizer::DestroyInstance);
        }
    return SoundRandomizer::mInstance;
}
