#include <string>

// clang-format off
class DyomStubSession
{

public:
    bool StartsAutomatically () { return false; }
    void ReportMissionPass () { }
    void ReportMissionSkip () { }
    void ReportStartSkip () { }
    void ReportRetry () { }
    void ProcessTimer () { }
    bool IsRunning () { return false; }
    int GetTimeLeft () { return 0; }
    bool IsRestoringFromCrash () { return false; }
    void ReportObjective (const std::string &str) { }
};
// clang-format on
