#include "base.hh"
#include "bass/bass.h"
#include "dyom.hh"
#include "functions.hh"
#include <mutex>
#include <thread>
#include <util/dyom/TTS.hh>
#include <sstream>
#include <iomanip>
#include <regex>
#include "bass_fx.h"
#include "logger.hh"

#include <iostream>
#include <array>
#include <regex>

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

    std::array  res{std::regex ("^\\W*(?:(?:~.~\\W*)|(?:[\\([]\\W*))(.+?"
                                 ")(?:[:)(\\] ]|(?:~.~))"),
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
DyomRandomizerTTS::EnqueueObjective (int objective, bool play)
{
    auto objectiveTexts = (const char*) ScriptSpace[9883];

    std::string objText = objectiveTexts + objective * 100;

    objText = std::regex_replace (objText, std::regex ("~.+?~"), "");
    objText = std::regex_replace (objText, std::regex ("_"), "");
    objText = std::regex_replace (objText, std::regex ("\\s+"), " ");

    std::string speaker
        = GuessObjectiveSpeaker (objectiveTexts + objective * 100);

    printf ("Final objective text after replacing: %s: %s\n",
            speaker.length () == 0 ? "Narrator" : speaker.c_str (),
            objText.c_str ());

    if (objText.size () < 3)
        return;

    std::lock_guard lock (queueMutex);
    queue.push_back ({objText, speaker, objective, play});
}

static std::string
EncodeURL (const std::string &s)
{
    const std::string safe_characters
        = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";
    std::ostringstream oss;
    for (auto c : s)
        {
            if (safe_characters.find (c) != std::string::npos)
                oss << c;
            else
                oss << '%' << std::setfill ('0') << std::setw (2)
                    << std::uppercase << std::hex << (0xff & c);
        }
    return oss.str ();
}

/*******************************************************/
std::string
DyomRandomizerTTS::GetRandomVoice ()
{
    static constexpr const char *englishVoices[] = {
        "Brian",  "Amy",    "Emma",     "Geraint", "Russell",
        "Nicole", "Joey",   "Justin",   "Matthew", "Ivy",
        "Joanna", "Kendra", "Kimberly", "Salli",   "Raveena",
    };

    static constexpr const char *voices[]
        = {"Zeina",     "Zhiyu",  "Mads",   "Naja",    "Ruben",   "Lotte",
           "Mathieu",   "Celine", "Lea",    "Chantal", "Hans",    "Marlene",
           "Vicki",     "Aditi",  "Karl",   "Dora",    "Giorgio", "Carla",
           "Bianca",    "Takumi", "Mizuki", "Seoyeon", "Liv",     "Jacek",
           "Jan",       "Ewa",    "Maja",   "Ricardo", "Camila",  "Vitoria",
           "Cristiano", "Ines",   "Carmen", "Maxim",   "Tatyana", "Enrique",
           "Conchita",  "Lucia",  "Mia",    "Miguel",  "Lupe",    "Penelope",
           "Astrid",    "Filiz",  "Gwyneth"};

    if (random (100) < 10)
        return GetRandomElement (voices);
    else
        return GetRandomElement (englishVoices);
}

/*******************************************************/
std::string
DyomRandomizerTTS::GetSoundURL (const std::string &text,
                                const std::string &voice)
{
    auto res = internet
                   .Post ("/polly/speak",
                          "Content-Type: application/x-www-form-urlencoded",
                          "service=Polly&voice=" + voice
                              + "&text=" + EncodeURL (text))
                   .GetString ();

    auto start = res.find ("\"speak_url\"") + 13;
    auto end   = res.find ("\"", start);

    auto url = res.substr (start, end - start);
    return std::regex_replace (url, std::regex ("\\\\/"), "/");
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

    if (!voices.count (entry.speaker))
        voices[entry.speaker] = GetRandomVoice ();

    std::string voice = voices[entry.speaker];

    printf ("Loading TTS Entry, objective: %d, voice: %s\n", entry.objective,
            voice.c_str ());
    auto url = GetSoundURL (entry.text.c_str (), voice);

    entry.sound
        = BASS_StreamCreateURL (url.c_str (), 0, BASS_STREAM_DECODE, 0, 0);

    entry.sound = BASS_FX_TempoCreate (entry.sound, BASS_FX_FREESOURCE);
    if (!entry.sound)
        {
            Logger::GetLogger ()->LogMessage (
                "ERROR!!! : " + std::to_string (BASS_ErrorGetCode ()));
        }

    if (entry.text.size () > 32)
        BASS_ChannelSetAttribute (entry.sound, BASS_ATTRIB_TEMPO,
                                  entry.text.size () / 1.2f);


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
            tts.areAnySoundsLoading = false;

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
                return true;

            printf ("Finished playing TTS Entry, sound: %lx\n", sound);
            BASS_StreamFree (sound);
            state = FREE;
            break;
        }

    return false;
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
                    voices.clear ();
                    areAnySoundsLoading = false;
                    areAnySoundsPlaying = false;
                    reset               = false;
                }

            std::this_thread::sleep_for (100ms);
        }
}

/*******************************************************/
DyomRandomizerTTS::DyomRandomizerTTS ()
{
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
