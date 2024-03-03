#include "base.hh"
#include "bass/bass.h"
#include "dyom.hh"
#include "functions.hh"
#include <exception>
#include <mutex>
#include <thread>
#include <util/dyom/TTS.hh>
#include <sstream>
#include <iomanip>
#include <regex>
#include "bass_fx.h"
#include "logger.hh"
#include "gender/gen_ext.h"
#include "util/dyom/tts/TikTok.hh"
#include "util/dyom/tts/TikTokSinging.hh"
#include <util/dyom/tts/StreamlabsPolly.hh>

#include <iostream>
#include <array>
#include <regex>
#include <random>
#include <algorithm>

// Implement function to get first name file for gender.c
extern "C" const char* get_first_name_file ()
{
    static auto nam_dict = GetRainbomizerFileName ("nam_dict.txt", "data/");
    return nam_dict.c_str ();
}

/*******************************************************/
std::string
DyomRandomizerTTS::GuessObjectiveSpeaker (const char *text)
{
    /* List of regex's to figure out the speaker from objective text.
       Supports stuff like:

       1. ~g~CJ~w~ haha
       2. (CJ) haha
       3. emak) haha
       4. CJ = haha

       and might match some random stuff as well
    */

    std::array  res{std::regex ("^\\W*(?:(?:~.+?~\\W*)|(?:[\\([]\\W*))(.+?"
                                 ")(?:[:)(\\] ]|(?:~.+?~))"),
                   std::regex ("^\\W*(\\w+?)\\W*[:)(\\]=]")};
    std::cmatch cm;

    for (auto &re : res)
        {
            if (std::regex_search (text, cm, re))
                return cm[1];
        }

    return "";
}

/*******************************************************/
void
DyomRandomizerTTS::RemoveSpeakerName (std::string &str)
{
    std::array res{std::regex ("^\\W*(?:(?:~.~\\W*)|(?:[\\([]\\W*))(.+?"
                               ")(?:[:)(\\] ]|(?:~.~))"),
                   std::regex ("^\\W*(\\w+?)\\W*[:)(\\]=]")};

    for (auto &re : res)
        str = std::regex_replace (str, re, "");
}

/*******************************************************/
Gender
GetSpeakerGender (std::string name)
{
    char gender = get_gender (name.data (), 0, GC_ANY_COUNTRY);
    printf ("Detecting gender for: %s -> %c (Country: %s)\n", name.c_str (),
            gender, get_country ());
    switch (gender)
        {
        case IS_FEMALE:
        case IS_MOSTLY_FEMALE: return GENDER_F;

        case IS_MALE:
        case IS_MOSTLY_MALE: return GENDER_M;
        }

    return GENDER_U;
}

/*******************************************************/
void
DyomRandomizerTTS::BuildObjectiveSpeakerMap ()
{
    struct VoiceUseFrequency
    {
        Voice voice;
        TTSBackend* backend;
        int Frequency;
    };
    
    if (voicesInitialised)
        return;

    std::map<std::string, SpeakerVoice> speakerVoices;

    std::vector<VoiceUseFrequency> voiceUseFrequency;

    // Generate an array biased towards English voices

    // Check: https://www.desmos.com/calculator/5jrwlemp7l for calculations

    // A value of 5% makes it so that about 20.3% have non-English voices (under
    // some assumptions). A value of 5% makes it so that there's a 10% chance of
    // non-English voice chosen in a mission with only one speaker (narrator).

    auto AddBackend = [&] (TTSBackend* backend)
    {
        auto voices = backend->GetVoices ();
        for (auto it = voices.begin (); it != voices.end (); ++it)
        {
            bool english = it->Country == "US" || it->Country == "GB"
                           || it->Country == "IN" || it->Country == "AU";

            int baseFreq = english ? 0 : (random (10000) > 500);
            if (english || (random(10000) <= 500))
                voiceUseFrequency.push_back ({*it, backend, 0});
            else
                voiceUseFrequency.push_back ({*it, backend, 1});
        }
    };

    static StreamlabsPolly sm_StreamlabsTTS;
    static TikTokTTS sm_TiktokTTS;
    static TikTokSinging sm_TiktokSingingTTS;

    AddBackend (&sm_TiktokSingingTTS);
    AddBackend (&sm_StreamlabsTTS);
    AddBackend (&sm_TiktokTTS);

    std::shuffle (voiceUseFrequency.begin (), voiceUseFrequency.end (),
                  std::mt19937{(unsigned int) time(NULL)});

    for (int i = 0; i < 100; i++)
        {
            auto objectiveTexts = (const char *) ScriptSpace[9883];

            std::string objText = objectiveTexts + i * 100;

            objText = std::regex_replace (objText, std::regex ("~.+?~"), "");
            objText = std::regex_replace (objText, std::regex ("_"), "");
            objText = std::regex_replace (objText, std::regex ("\\s+"), " ");

            std::string speaker
                = GuessObjectiveSpeaker (objectiveTexts + i * 100);

            speaker
                = std::regex_replace (speaker, std::regex ("[^A-Za-z]"), "");
            std::transform (speaker.begin (), speaker.end (), speaker.begin (),
                            [] (unsigned char c) { return std::tolower (c); });

            if (speakerVoices.count (speaker))
                {
                    voices[i]                   = speakerVoices[speaker];
                    voices[i].RemoveSpeakerName = true;
                    continue;
                }

            auto gender    = GetSpeakerGender (speaker);
            auto candidate = voiceUseFrequency.end ();
            for (auto it = voiceUseFrequency.begin ();
                 it != voiceUseFrequency.end (); ++it)
                {
                    if ((it->voice.Gender == gender || gender == GENDER_U)
                        && (candidate == voiceUseFrequency.end ()
                            || candidate->Frequency > it->Frequency))
                        candidate = it;
                }

            speakerVoices[speaker]
                = {candidate->voice.Name,
                   candidate->Frequency
                       * (candidate->voice.Gender == GENDER_F ? -4 : 4),
                   candidate->backend,
                   false};

            candidate->Frequency++;
            voices[i] = speakerVoices[speaker];
        }

    for (auto &[speaker, voice] : speakerVoices)
        {
            printf ("%s: %s (REM: %s) (+%d)\n",
                    speaker.size () == 0 ? "Narrator" : speaker.c_str (),
                    voice.Voice.c_str (),
                    voice.RemoveSpeakerName ? "TRUE" : "FALSE", voice.Pitch);
        }

    voicesInitialised = true;
}

/*******************************************************/
void
DyomRandomizerTTS::EnqueueObjective (int objective, bool play)
{
    auto objectiveTexts = (const char*) ScriptSpace[9883];

    std::string objText = objectiveTexts + objective * 100;

    objText = std::regex_replace (objText, std::regex ("~.+?~"), "");
    objText = std::regex_replace (objText, std::regex ("_"), "");
    objText = std::regex_replace (objText, std::regex ("\\s+"), " ");

    for (auto &reg : swearFilter)
        objText = std::regex_replace (objText, reg, "redacted");

    if (voices[objective].RemoveSpeakerName)
        RemoveSpeakerName (objText);

    if (objText.size () < 3)
        return;

    std::lock_guard lock (queueMutex);
    queue.push_back ({objText, voices[objective], objective, play});
}

HSTREAM
BASS_FXDEF (BASS_FX_TempoCreate) (DWORD chan, DWORD flags)
{
    typedef HSTREAM (WINAPI * BFXTC) (DWORD, DWORD);

    static auto  handle = LoadLibrary ("bass_fx.dll");
    static BFXTC proc = (BFXTC) GetProcAddress (handle, "BASS_FX_TempoCreate");

    return proc (chan, flags);
}

/*******************************************************/
void
DyomRandomizerTTS::LoadEntry (StreamEntry &entry)
{
    static bool bassInitialised = false;
    if (!bassInitialised)
        {
            BASS_Init (-1, 44100, 0, 0, nullptr);
            bassInitialised = true;
        }


    std::string voice = entry.speaker.Voice;

    printf ("Loading TTS Entry, objective: %d, voice: %s\n", entry.objective,
            voice.c_str ());

    entry.sound = entry.speaker.Backend->GetSoundStream (entry.text, voice, &entry.data);
    entry.sound = BASS_FX_TempoCreate (entry.sound, BASS_FX_FREESOURCE);

    if (!entry.sound)
        {
            Logger::GetLogger ()->LogMessage (
                "BASS error (invalid sound): "
                + std::to_string (BASS_ErrorGetCode ()));
        }

    BASS_ChannelSetAttribute (entry.sound, BASS_ATTRIB_TEMPO_PITCH,
                              entry.speaker.Pitch);

    if (DyomRandomizer::GetInstance ()->m_Config.SpeedUpLongTTSTexts
        && entry.text.size () > 32)
        BASS_ChannelSetAttribute (entry.sound, BASS_ATTRIB_TEMPO,
                                  entry.text.size () / 1.5f);


    BASS_ChannelSetAttribute(entry.sound, BASS_ATTRIB_VOL, FrontendMenuManager->m_nSfxVolume / 12.0f);

    if (DyomRandomizer::GetInstance()->m_Config.OverrideTTSVolume >= 0.0f)
        BASS_ChannelSetAttribute (
            entry.sound, BASS_ATTRIB_VOL,
            DyomRandomizer::GetInstance ()->m_Config.OverrideTTSVolume);

    entry.state = StreamEntry::LOADED;
}

/*******************************************************/
bool
DyomRandomizerTTS::StreamEntry::ProcessFSM (DyomRandomizerTTS &tts)
{
    switch (state)
        {
        case FREE: break;

        case REQUESTED:
            if (shouldPlay)
                tts.areAnySoundsLoading = true;

            tts.LoadEntry (*this);

            [[fallthrough]];

        case LOADED:
            if (shouldPlay && sound)
                {
                    printf ("Playing loaded TTS Entry, sound: %lx\n", sound);
                    BASS_ChannelPlay (sound, false);
                    state = PLAYING;
                }
            break;

        case PLAYING:
            if (BASS_ChannelIsActive (sound) != BASS_ACTIVE_STOPPED)
                {
                    return true;
                }

            printf ("Finished playing TTS Entry, sound: %lx\n", sound);
            BASS_StreamFree (sound);
            speaker.Backend->FreeData(data);
            state = FREE;
            break;
        }

    tts.areAnySoundsLoading = false;
    return state == PLAYING ? true : false;
}

/*******************************************************/
void
DyomRandomizerTTS::ProcessStreams ()
{
    bool soundsPlaying = false;

    for (auto &entry : streams)
        {
            if (entry.ProcessFSM (*this))
                soundsPlaying = true;
        }

    areAnySoundsPlaying = soundsPlaying;
}

/*******************************************************/
void
DyomRandomizerTTS::ProcessQueue ()
{
    std::lock_guard lock (queueMutex);
    for (auto &entry : queue)
        {
            bool exists = false;

            // Ensure there isn't already a running stream
            for (auto &stream : streams)
                {
                    if (stream.objective == entry.objective)
                        {
                            stream.shouldPlay = entry.play;
                            exists            = true;

                            break;
                        }
                }

            if (!exists)
                {
                    streams.emplace_back (entry.text, entry.speaker, entry.objective);
                    streams.back ().shouldPlay = entry.play;

                    printf ("Added objective to streams, %d\n",
                            entry.objective);
                }
        }

    queue.clear ();
}

/*******************************************************/
void
DyomRandomizerTTS::CleanupStreams ()
{
    // Stop sounds from previous objectives
    for (auto it = streams.begin (); it != streams.end ();)
        {
            auto &entry = *it;
            if (entry.objective < ScriptSpace[9903]
                && entry.state == StreamEntry::PLAYING)
                BASS_ChannelStop (entry.sound);

            if (entry.state == StreamEntry::FREE)
                it = streams.erase (it);
            else
                it++;
        }
}

/*******************************************************/
void
DyomRandomizerTTS::PlayObjectiveSound ()
{
    BuildObjectiveSpeakerMap ();
    EnqueueObjective (ScriptSpace[9903], true);
    EnqueueObjective (ScriptSpace[9903] + 1, false);
}

/*******************************************************/
void
DyomRandomizerTTS::ProcessTTS ()
{
    this->running = true;
    while (this->running)
        {
            using namespace std::chrono_literals;

            ProcessStreams ();
            ProcessQueue ();
            CleanupStreams ();

            if (reset)
                {
                    queue.clear ();
                    streams.clear ();
                    areAnySoundsLoading = false;
                    areAnySoundsPlaying = false;
                    voicesInitialised   = false;
                    reset               = false;
                }

            std::this_thread::sleep_for (100ms);
        }
}

/*******************************************************/
void
DyomRandomizerTTS::ReadSwearFilterFile ()
{
    FILE *filterFile = OpenRainbomizerFile ("Swear_Words.txt", "r", "data/");
    if (filterFile)
        {
            char line[512] = {};

            while (fgets (line, 256, filterFile))
                {
                    line[strcspn (line, "\n")] = 0;
                    if (strlen (line) <= 2)
                        continue;

                    try
                        {
                            std::regex reg{line, std::regex_constants::icase};
                            swearFilter.push_back (reg);
                        }
                    catch (std::regex_error &e)
                        {
                            Logger::GetLogger ()->LogMessage (
                                "Error parsing regex: " + std::string (line)
                                + " " + e.what ());
                        }
                }
        }
    else if (!filterFile)
        {
            // Log a message if file wasn't found
            Logger::GetLogger ()->LogMessage (
                "Failed to read file: rainbomizer/data/Swear_Words.txt");
        }
    /*auto filterFile = GetRainbomizerDataFile("Swear_Words.txt");*/
}

/*******************************************************/
DyomRandomizerTTS::DyomRandomizerTTS ()
{
    ReadSwearFilterFile ();
    internet.Open ("streamlabs.com");

    workerThread = std::thread (&DyomRandomizerTTS::ProcessTTS, this);
}

/*******************************************************/
void
DyomRandomizerTTS::Reset ()
{
    using namespace std::chrono_literals;

    reset           = true;
    auto resetStart = time (NULL);

    const int TIMEOUT_DURATION = 4;

    while (reset && time (NULL) - resetStart < TIMEOUT_DURATION)
        std::this_thread::sleep_for (100ms);
}

/*******************************************************/
DyomRandomizerTTS::~DyomRandomizerTTS ()
{
    internet.Close ();
    running = false;
    workerThread.join ();
}
