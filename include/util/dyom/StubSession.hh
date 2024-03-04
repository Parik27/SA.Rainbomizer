#include <string>

// clang-format off
class DyomStubSession
{

public:
    bool StartsAutomatically () { return false; }
    bool EndsAutomatically () { return false; }
    // legitimate is true if and only if the mission pass text was displayed by dyom.
    void ReportMissionPass (bool legitimate = false) { }
    void ReportMissionSkip () { }
    void ReportStartSkip () { }
    void ReportRetry () { }
    void ProcessTimer () { }
    bool IsRunning () { return false; }
    int GetTimeLeft () { return 0; }
    bool IsRestoringFromCrash () { return false; }
    void ReportObjective (const std::string &original, const std::string &translated) { }
    void BackupObjectiveTexts (std::string objectives[100]) {}
    void RestoreObjectiveTexts (std::string objectives[100]) {}
    void ReportMissionStart (const std::string &html, const std::string &url) {}
    bool ShouldSkipMission (const std::string &url) { return false; }
    void ReportMissionUnplayable (const std::string &url) {}
};
// clang-format on
