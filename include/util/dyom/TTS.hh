#pragma once

#include <bass/bass.h>
#include <mutex>
#include <string>
#include <thread>
#include <condition_variable>

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

        StreamEntry (const std::string &text, int objective)
            : objective (objective), text (text)
        {
            state = REQUESTED;
        }

        bool ProcessFSM (DyomRandomizerTTS &tts);
    };

    struct QueueEntry
    {
        std::string text;
        int         objective;
        bool        play;
    };

    bool                     areAnySoundsLoading;
    bool                     areAnySoundsPlaying;
    std::mutex               queueMutex;
    std::vector<StreamEntry> streams;
    std::vector<QueueEntry>  queue;

    InternetUtils internet;

    std::string GetSoundURL (const std::string& text);

    void ProcessStreams ();
    void ProcessQueue ();
    void CleanupStreams ();
    void ProcessTTS ();

    void LoadEntry (StreamEntry &entry);
    void EnqueueObjective (int objective, bool play);

    std::thread workerThread;
    bool running = false;

public:
    DyomRandomizerTTS ();
    ~DyomRandomizerTTS ();

    bool
    IsBusy ()
    {
        return areAnySoundsLoading || areAnySoundsPlaying;
    }

    void Reset ();
    void PlayObjectiveSound ();
};
