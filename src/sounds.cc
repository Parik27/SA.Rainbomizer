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

SoundRandomizer *SoundRandomizer::mInstance = nullptr;

/*******************************************************/
void __fastcall RandomizeAudioLoad (CAudioEngine *audio, void *edx,
                                    unsigned char slot, int id)
{
    auto soundsR = SoundRandomizer::GetInstance ();
    try
        {
            int  newIndex;
            auto newSoundPair  = soundsR->GetRandomPair (newIndex);
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
char *__fastcall CorrectSubtitles (CText *text, void *edx, char *key)
{
    auto soundsR = SoundRandomizer::GetInstance ();

    try
        {
            auto prevIndex = soundsR->GetPreviousPairs ().at (key);
            soundsR->GetPreviousPairs ().erase (key);

            auto soundPair
                = SoundRandomizer::GetInstance ()->GetPairByIndex (prevIndex);
            return (char *) GxtManager::GetText (soundPair.name);
        }
    catch (std::out_of_range e)
        {
            return text->Get (key);
        }
}

/*******************************************************/
std::unordered_map<std::string, int> &
SoundRandomizer::GetPreviousPairs ()
{
    return mPreviousPairs;
}

/*******************************************************/
SoundPair
SoundRandomizer::GetRandomPair (int &index)
{
    int id = random (mSoundTable.size () - 1);

    index = id;
    return mSoundTable[id];
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
                              [id](const SoundPair &a) { return a.id == id; });
    return *iter;
}

/*******************************************************/
void
SoundRandomizer::InitaliseSoundTable ()
{
    FILE *audioEventsTable = fopen ("data/AudioEvents.txt", "r");
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
    Logger::GetLogger ()->LogMessage ("Intialised SoundRandomizer");
    RegisterHooks ({{HOOK_CALL, 0x4851BB, (void *) &RandomizeAudioLoad},
                    {HOOK_CALL, 0x468173, (void *) &CorrectSubtitles}});
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
