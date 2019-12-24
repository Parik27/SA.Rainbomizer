#include "sounds.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include <cctype>
#include "text.hh"
#include <ctime>
#include <algorithm>
#include <ctype.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <string>
#include "config.hh"
#include "injector/injector.hpp"

SoundRandomizer *SoundRandomizer::mInstance = nullptr;

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
// char *__fastcall CorrectSubtitles (CText *text, void *edx, char *key)
// {
//     auto soundsR = SoundRandomizer::GetInstance ();
//     auto config  = ConfigManager::GetInstance ()->GetConfigs ().sounds;

//     if (!config.matchSubtitles)
//         return text->Get (key);

//     try
//         {
//             auto prevIndex = soundsR->GetPreviousPairs ().at (key);
//             soundsR->GetPreviousPairs ().erase (key);

//             auto soundPair
//                 = SoundRandomizer::GetInstance ()->GetPairByIndex
//                 (prevIndex);

//             auto text = (char *) GxtManager::GetText (soundPair.name);
//             soundsR->SetPreviousOverridenText (key, text);

//             return text;
//         }
//     catch (std::out_of_range e)
//         {
//             return text->Get (key);
//         }
// }

/*******************************************************/
std::vector<FILE *>
GetPakFiles (CAEMp3BankLoader *loader)
{
    std::vector<FILE *> files;
    if (!loader->m_pPakFileNames)
        {
            Logger::GetLogger ()->LogMessage ("Unable to intialise Sound List");
            Logger::GetLogger ()->LogMessage ("Unable to read Pak file names");

            return files;
        }

    for (int i = 0; i < loader->m_nNumPakFiles; i++)
        {
            std::string fileName (loader->m_pPakFileNames + 52 * i);
            FILE *      pakFile = fopen (GetGameDirRelativePathA (
                                       ("audio/SFX/" + fileName).c_str ()),
                                   "rb");

            if (!pakFile)
                {
                    Logger::GetLogger ()->LogMessage (
                        "Unable to intialise Sound List");
                    Logger::GetLogger ()->LogMessage ("Unable to read "
                                                      + fileName);
                }
            files.push_back (pakFile);
        }
    return files;
}

/*******************************************************/
void
ForEachBankSound (FILE *file, BankLkup bank,
                  std::function<void (PakFile, int)> callback)
{
    uint16_t soundCount;
    fseek (file, bank.m_dwOffset, SEEK_SET);
    fread (&soundCount, 2, 1, file);

    for (int i = 0; i < soundCount; i++)
        {
            PakFile pak;
            fread (&pak, sizeof (PakFile), 1, file);

            callback (pak, i);
        }
}

/*******************************************************/
std::string
GetSubtitleForSFX (short bank, short slot, short sfxIndex)
{
    auto soundRandomizer = SoundRandomizer::GetInstance ();

    if (sfxIndex == 3)
        {
            int event = SoundRandomizer::CalculateEventFromSFX (bank, slot);
            return soundRandomizer->GetSubtitleByID (event);
        }
    return "";
}

/*******************************************************/
std::vector<SFXPair>
GetValidSoundPairs (CAEMp3BankLoader *loader, std::vector<FILE *> files,
                    bool closeFiles = true)
{
    std::vector<SFXPair> list;
    for (int i = 0; i < loader->m_nNumBankLkups; i++)
        {
            auto bank = loader->m_pBankLkups[i];
            if (bank.sfxIndex > files.size ())
                break;

            auto pakFile = files[bank.sfxIndex];

            ForEachBankSound (pakFile, bank, [&] (PakFile pak, int j) {
                if (pak.loopOffset == -1)
                    list.push_back (
                        {i, j, GetSubtitleForSFX (i, j, bank.sfxIndex)});
            });
        }

    // close file
    if (closeFiles)
        std::for_each (files.begin (), files.end (), fclose);

    return list;
}

/*******************************************************/
char __fastcall InitialiseSoundsList (CAEMp3BankLoader *thisLoader)
{
    int  ret             = thisLoader->Initialise ();
    auto soundRandomizer = SoundRandomizer::GetInstance ();

    soundRandomizer->mBankLkups
        = GetValidSoundPairs (thisLoader, GetPakFiles (thisLoader));

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
std::string
SoundRandomizer::GetSubtitleByID (int id)
{
    if (this->mSubtitles.count (id))
        return mSubtitles[id];
    return "";
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
                mSubtitles[id] = name + 6;
        }
}

/*******************************************************/
char
ReturnRandomBankAndSound (signed int *wavId, int *bank, int *sfx,
                          signed int slot)
{
    auto soundRandomizer = SoundRandomizer::GetInstance ();
    auto sound
        = soundRandomizer
              ->mBankLkups[random (soundRandomizer->mBankLkups.size () - 1)];

    *bank = sound.bank;
    *sfx  = sound.sfx;

    soundRandomizer->UpdatePreviousPairs (*wavId, sound);
    return true;
}

/*******************************************************/
int
SoundRandomizer::CalculateEventFromSFX (int bank, int sfx)
{
    return (bank - 147) * 200 + sfx + 2000;
}

/*******************************************************/
std::string
SoundRandomizer::GetNewSubtitleForEvent (int event, const SFXPair &pair)
{
    std::string next_sub = pair.subtitle;
    // A 1 in 100 chance
    if (pair.subtitle == "" && random (100) == 0)
        {
            static std::vector<std::string> place_holders
                = {"*sound of people talking*", "bla bla bla",
                   "grumble grumble grumble", "INSERT WORDS HERE",
                   "Alert: A sound is currently playing. Thank you for your "
                   "time."};

            next_sub = place_holders[random (place_holders.size ())];
        }
    else if (pair.subtitle != "")
        next_sub = GxtManager::GetText (pair.subtitle);

    return next_sub;
}

/*******************************************************/
void
SoundRandomizer::UpdatePreviousPairs (int event, const SFXPair &pair)
{
    std::string sub = GxtManager::GetText (GetSubtitleByID (event));
    if (sub != "")
        {
            std::string next_sub = GetNewSubtitleForEvent (event, pair);

            mPreviousPairs[sub] = mCurrentReplacedSubIndex;
            mReplacedSubtitles[mCurrentReplacedSubIndex] = next_sub;

            mCurrentReplacedSubIndex++;
            mCurrentReplacedSubIndex %= 10;
        }
}

/*******************************************************/
void
CorrectSubtitles (char *string, int time, int16_t flags,
                  char bAddToPreviousBrief)
{
    auto soundRandomizer = SoundRandomizer::GetInstance ();
    if (bAddToPreviousBrief && soundRandomizer->mPreviousPairs.count (string))
        {
            bAddToPreviousBrief = soundRandomizer->mPreviousPairs[string] + 2;
            soundRandomizer->mPreviousPairs.erase (
                soundRandomizer->mPreviousPairs.find (string));
        }
    CMessages::AddMessage (string, time, flags, bAddToPreviousBrief);
}

/*******************************************************/
void
ShuffleString (char *str)
{
    static char colour_codes[] = {'b', 'g', 'l', 'p', 'r', 'w', 'y', 'h'};

    while (*str)
        {
            if (isalpha (*str) && str[-1] == '~' && str[1] == '~')
                *str = colour_codes[random (sizeof (colour_codes) - 1)];

            else if (isalpha (*str))
                {
                    if (isupper (*str))
                        *str = random (90, 65);
                    else
                        *str = random (122, 97);
                }
            str++;
        }
}

/*******************************************************/
void
DisplayCorrectedSubtitles (char *str)
{
    int8_t index = injector::ReadMemory<int8_t> (0xC1A7F0 + 0x2C);

    // bAddToPreviousBrief = soundRandomizer->mPreviousPairs[string] + 2;
    // puts(str);
    if (index > 1)
        {
            auto        soundRandomizer = SoundRandomizer::GetInstance ();
            std::string new_sub         = "";

            new_sub = soundRandomizer->mReplacedSubtitles[index - 2];

            if (new_sub != "")
                {
                    memset (str, 0, 400);
                    strcpy (str, new_sub.c_str ());
                }
            else
                ShuffleString (str);
        }

    CHud::SetMessage (str);
}

/*******************************************************/
void
SoundRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().sounds;
    if (!config.enabled)
        return;

    // CMessages::Display

    Logger::GetLogger ()->LogMessage ("Intialised SoundRandomizer");
    RegisterHooks ({{HOOK_CALL, 0x4EC1D2, (void *) &ReturnRandomBankAndSound},
                    {HOOK_CALL, 0x468137, (void *) &CorrectSubtitles},
                    {HOOK_CALL, 0x468E9A, (void *) &InitialiseTexts},
                    {HOOK_CALL, 0x618E97, (void *) &InitialiseTexts},
                    {HOOK_CALL, 0x5BA167, (void *) &InitialiseTexts},
                    {HOOK_CALL, 0x4D99B3, (void *) &InitialiseSoundsList},
                    {HOOK_CALL, 0x69F09C, (void *) &DisplayCorrectedSubtitles},
                    {HOOK_CALL, 0x4681BE, (void *) &CorrectSubtitles}});

    // This NOP's the code that deletes the m_pPakFiles variable which is used
    // in InitialiseSoundList
    injector::MakeNOP (0x04DFDCE, 0x7);
    injector::MakeNOP (0x04DFDC3, 0x5);

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
