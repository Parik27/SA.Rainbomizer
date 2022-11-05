#pragma once

#include <bass/bass.h>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>
#include <map>

#include "Internet.hh"

class DyomRandomizerTTS
{
    struct StreamEntry
    {
        enum
        {
            FREE,
            REQUESTED,
            LOADED,
            PLAYING
        } state;

        int         objective;
        bool        shouldPlay;
        HSTREAM     sound;
        std::string text;
        std::string speaker;

        StreamEntry (const std::string &text, const std::string &speaker,
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
        std::string speaker;
        int         objective;
        bool        play;
    };

    bool                               areAnySoundsLoading;
    bool                               areAnySoundsPlaying;
    std::mutex                         queueMutex;
    std::vector<StreamEntry>           streams;
    std::vector<QueueEntry>            queue;
    std::map<std::string, std::string> voices;

    InternetUtils internet;

    std::string GetSoundURL (const std::string &text, const std::string &voice);
    std::string GuessObjectiveSpeaker (const char *text);

    void ProcessStreams ();
    void ProcessQueue ();
    void CleanupStreams ();
    void ProcessTTS ();

    void LoadEntry (StreamEntry &entry);
    void EnqueueObjective (int objective, bool play);

    std::thread workerThread;
    bool running = false;
    bool reset = false;

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
