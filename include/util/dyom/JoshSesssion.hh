#include <cstdint>
#include <regex>
#include <string>
#include <ctime>

#include <windows.h>

#include "logger.hh"
#include "base.hh"
#include "dyom.hh"
#include "Translation.hh"

#include <set>

class DyomJoshSession
{
    const int SESSION_DURATION = 1 * 60 * 60;

    uint32_t objectiveTime = 0;
    time_t startTime = 0;

    int32_t missionsPassed = 0;
    int32_t missionsSkipped = 0;

    bool active = false;
    bool restoringFromCrash = false;

    FILE* graphFile = nullptr;
    FILE* subtitlesFile = nullptr;
    FILE* sessionFile = nullptr;

    std::set<uint32_t> playedMissions;

    // Util Functions
    FILE *
    OpenFile (const std::string &name, const char *mode = "r",
              const std::string &subdirs = "")
    {
        return fopen (GetRainbomizerFileName (name, "dyom/" + subdirs).c_str (),
                      mode);
    }

    template<int VK>
    bool IsKeyUp (int OverrideVK = VK)
    {
        static bool prevState = false;
        if (!prevState && GetAsyncKeyState(OverrideVK))
            prevState = true;

        if (prevState && !GetAsyncKeyState(OverrideVK)) {
            prevState = false;
            return true;
        }

        return false;
    }

    template <typename... Args>
    void
    FPrintf (const std::string &name, const std::string &subdirs,
             const char *format, Args... args)
    {
        FILE *f = OpenFile (name, "w", subdirs);
        if (!f)
            MessageBox(NULL, name.c_str(), NULL, NULL);
        fprintf (f, format, args...);
        fclose (f);
    }

    // Class functions
    std::string GetSessionFileName (time_t start);

    void OpenSessionFile ();

    void Reset (bool write = true);

    void LoadSessionInfo ();
    void SaveSessionInfo ();

    void LoadPlayedMissions ();
    void SavePlayedMissions ();

    void AddLastMissionToPlayedList ();

public:
    DyomJoshSession ();

    // Return true if autoplay starts automatically when random mission is
    // selected.
    bool
    StartsAutomatically ()
    {
        return true;
    }

    // Return true if autoplay ends automatically when the session is completed.
    bool
    EndsAutomatically ()
    {
        return true;
    }

    void
    ReportMissionPass (bool legitimate = false)
    {
        if (active)
            {
            if (legitimate)
                AddLastMissionToPlayedList ();

            missionsPassed++;
            SaveSessionInfo ();
            }
    }

    void
    ReportMissionSkip ()
    {
        if (active)
            {
                missionsSkipped++;
                SaveSessionInfo ();
            }
    }

    void
    ReportStartSkip ()
    {
        auto &config = DyomRandomizer::GetInstance ()->m_Config;

        if (!active || (config.AutoReset && missionsPassed == 0))
            {
            Reset ();
            active = true;
            }
        else
            {
            // Free skips on mission crash
            if (!restoringFromCrash)
                ReportMissionSkip ();

            restoringFromCrash = false;
            }
    }

    void
    ReportRetry ()
    {
        if (restoringFromCrash)
            restoringFromCrash = false;
        active = true;
    }

    void ProcessTimer ();

    bool
    IsRunning ()
    {
        return active;
    }

    int
    GetTimeLeft ()
    {
        return SESSION_DURATION - (time(NULL) - startTime);
    }

    bool IsRestoringFromCrash ();

    void RestoreObjectiveTexts (std::string out[100]);

    void BackupObjectiveTexts (std::string texts[100]);

    void ReportObjective (const std::string &original,
                          const std::string &translated);

    void ReportMissionStart (const std::string &html, const std::string &url);

    bool ShouldSkipMission (const std::string &url);

    void ReportMissionUnplayable (const std::string &url);
};
