#pragma once

#include <bass/bass.h>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>
#include <map>
#include <array>
#include <regex>

#include "Internet.hh"
#include "tts/TTSBackend.hh"

class DyomRandomizerTTS
{

    struct SpeakerVoice
    {
        std::string Voice;
        int         Pitch;
        TTSBackend *Backend;
        bool        RemoveSpeakerName;
    };

    struct StreamEntry
    {
        enum
        {
            FREE,
            REQUESTED,
            LOADED,
            PLAYING
        } state;

        int          objective;
        bool         shouldPlay;
        HSTREAM      sound;
        std::string  text;
        SpeakerVoice speaker;
        void        *data;

        StreamEntry (const std::string &text, SpeakerVoice &speaker,
                     int objective)
            : objective (objective), text (text), speaker (speaker)
        {
            state = REQUESTED;
        }

        bool ProcessFSM (DyomRandomizerTTS &tts);
    };

    struct QueueEntry
    {
        std::string text;
        SpeakerVoice speaker;
        int         objective;
        bool        play;
    };

    bool                          areAnySoundsLoading;
    bool                          areAnySoundsPlaying;
    std::mutex                    queueMutex;
    std::vector<StreamEntry>      streams;
    std::vector<std::regex>       swearFilter;
    std::vector<QueueEntry>       queue;
    std::array<SpeakerVoice, 100> voices;
    bool                          voicesInitialised = false;

    InternetUtils internet;

    std::string GuessObjectiveSpeaker (const char *text);
    void        RemoveSpeakerName (std::string &str);

    void ProcessStreams ();
    void ProcessQueue ();
    void CleanupStreams ();
    void ProcessTTS ();

    void ReadSwearFilterFile ();

    void LoadEntry (StreamEntry &entry);
    void EnqueueObjective (int objective, bool play);

    std::thread workerThread;
    bool        running = false;
    bool        reset   = false;

    void BuildObjectiveSpeakerMap ();

public:
    DyomRandomizerTTS ();
    ~DyomRandomizerTTS ();

    void SetDuration (int duration);

    std::string GetRandomVoice ();

    bool
    IsBusy ()
    {
        return areAnySoundsLoading || areAnySoundsPlaying;
    }

    void Reset ();
    void PlayObjectiveSound ();
};
