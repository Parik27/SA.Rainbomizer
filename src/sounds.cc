#include "sounds.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include <cctype>
#include "util/text.hh"
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include "config.hh"
#include "injector/injector.hpp"

SoundRandomizer *SoundRandomizer::mInstance = nullptr;

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

    if (!SoundRandomizer::m_Config.MatchSubtitles)
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
    catch (std::out_of_range &e)
        {
            return text->Get (key);
        }
}

/*******************************************************/
char __fastcall InitialiseLoopedSoundList (CAEMp3BankLoader *thisLoader)
{
    int  ret             = thisLoader->Initialise ();
    auto soundRandomizer = SoundRandomizer::GetInstance ();

    FILE *scriptSFX
        = fopen (GetGameDirRelativePathA ("audio/SFX/SCRIPT"), "rb");

    for (auto it = soundRandomizer->mSoundTable.begin ();
         it != soundRandomizer->mSoundTable.end ();)
        {

            int sfx_id;
            int bank;
            CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent (&it->id,
                                                                      &bank,
                                                                      &sfx_id,
                                                                      1);

            auto bankLkup = thisLoader->m_pBankLkups[bank];
            if (bankLkup.sfxIndex == 3)
                {
                    PakFile sfx;
                    fseek (scriptSFX,
                           bankLkup.m_dwOffset + sizeof (PakFile) * sfx_id,
                           SEEK_SET);

                    fread (&sfx, sizeof (PakFile), 1, scriptSFX);

                    // Looping sound
                    if (sfx.loopOffset != -1)
                        {
                            soundRandomizer->mLoopedSounds.push_back (*it);
                            it = soundRandomizer->mSoundTable.erase (it);
                            continue;
                        }
                }
            ++it;
        }

    fclose (scriptSFX);
    return ret;
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
bool
SoundRandomizer::IsSoundLooped (int id)
{
    for (auto i : mLoopedSounds)
        {
            if (i.id == id)
                return true;
        }

    return false;
}

/*******************************************************/
SoundPair
SoundRandomizer::GetRandomPair (int &index, int slot, int id)
{

    if (mSoundTable.size () < 1)
        throw;

    if (this->IsSoundLooped (id))
        {
            auto pair = mLoopedSounds.at (random (mLoopedSounds.size () - 1));
            puts (pair.name.c_str ());
            return pair;
        }

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
    if (iter != mSoundTable.end ())
        return *iter;

    return {-1, "default"};
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
signed short __fastcall RandomizeSayEvent (CPed *ped, void *edx, int phraseID,
                                           int a3, float a4, int a5, char a6,
                                           char a7)
{
    signed short result;
    if (phraseID)
        result
            = CallMethodAndReturn<signed short, 0x4E6550> (&ped->m_pedSpeech,
                                                           52, random (358), a3,
                                                           a4, a5, a6, a7);
    else
        result = -1;
    return result;
}

/*******************************************************/
void __fastcall RandomizeFrontendAudio (CAudioEngine *audioEngine, void *edx,
                                        int eventId, float volumeChange,
                                        float speed)
{
    static std::vector<int> validSounds
        = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
           17, 18, 19, 20, 27, 28, 29, 30, 32, 40, 41, 42, 43, 44, 45, 46, 49};

    std::vector<int> soundsToPickFrom
        = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
           13, 14, 15, 17, 18, 19, 20, 32, 40, 43, 44, 45, 46};

    std::vector<int> onlyInGameSounds = {16, 27, 28, 29, 30, 49};

    int randomEvent;
    if (find (validSounds.begin (), validSounds.end (), eventId)
        == validSounds.end ())
        {
            randomEvent = eventId;
        }
    else
        {
            if (eventId > 5)
                {
                    soundsToPickFrom.insert (soundsToPickFrom.end (),
                                             onlyInGameSounds.begin (),
                                             onlyInGameSounds.end ());
                }
            randomEvent
                = soundsToPickFrom[random (soundsToPickFrom.size () - 1)];
        }
    CallMethod<0x4DD4A0> (&audioEngine->m_FrontendAudio, randomEvent,
                          volumeChange, speed);
}

/*******************************************************/
void
SoundRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "SoundRandomizer",
            std::pair ("RandomizeScriptVoiceLines",
                       &m_Config.RandomizeMissionLines),
            std::pair ("MatchSubtitles", &m_Config.MatchSubtitles),
            std::pair ("RandomizeGenericPedSpeech",
                       &m_Config.RandomizePedSpeech),
            std::pair ("RandomizeGenericSfx", &m_Config.RandomizeGenericSfx),
            std::pair ("ForcedAudioLine", &m_Config.ForcedAudioLine)))
        return;

    Logger::GetLogger ()->LogMessage ("Intialised SoundRandomizer");

    if (m_Config.RandomizeMissionLines)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x4851BB, (void *) &RandomizeAudioLoad},
                 {HOOK_CALL, 0x468173, (void *) &CorrectSubtitles},
                 {HOOK_CALL, 0x4680E7, (void *) &CorrectSubtitles},
                 {HOOK_CALL, 0x485397, (void *) &RemoveSubtitlesHook},
                 {HOOK_CALL, 0x468E9A, (void *) &InitialiseTexts},
                 {HOOK_CALL, 0x618E97, (void *) &InitialiseTexts},
                 {HOOK_CALL, 0x5BA167, (void *) &InitialiseTexts},
                 {HOOK_CALL, 0x4D99B3, (void *) &InitialiseLoopedSoundList}});

            injector::WriteMemory<uint8_t> (0x4EC302 + 2, 3);
            InitaliseSoundTable ();
        }

    if (m_Config.RandomizePedSpeech)
        RegisterHooks ({{HOOK_JUMP, 0x5EFFE0, (void *) &RandomizeSayEvent}});

    if (m_Config.RandomizeGenericSfx)
        RegisterHooks (
            {{HOOK_JUMP, 0x506EA0, (void *) &RandomizeFrontendAudio}});
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
