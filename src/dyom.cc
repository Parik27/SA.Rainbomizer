#include "base.hh"
#include "dyom.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include "injector/calling.hpp"
#include <ctime>
#include <fstream>
#include <wininet.h>
#include "config.hh"
#include <cstring>
#include "missions.hh"
#include "util/dyom/DYOMFileFormat.hh"
#include "util/scrpt.hh"
#include "util/dyom/Translation.hh"
#include "util/dyom/StubSession.hh"

using namespace std::literals;

DyomRandomizer *DyomRandomizer::mInstance = nullptr;
DyomStubSession sm_Session;

/*******************************************************/
CRunningScript *
StoreRandomizedDYOMScript (uint8_t *startIp)
{
    auto dyomRandomizer = DyomRandomizer::GetInstance ();

    auto out = HookManager::CallOriginalAndReturn<
        injector::cstd<CRunningScript *(uint8_t *)>, 0x46683B> (nullptr,
                                                                startIp);
    if (ScriptParams[0] == 91092)
        dyomRandomizer->mDyomScript = out;

    DyomRandomizer::mEnabled = true;
    return out;
}

/*******************************************************/
void
DyomHandleOnScriptOpCodeProcess ()
{
    auto dyomRandomizer = DyomRandomizer::GetInstance ();

    if (DyomRandomizer::mEnabled && dyomRandomizer->mDyomScript)
        dyomRandomizer->HandleDyomScript (dyomRandomizer->mDyomScript);

    HookManager::CallOriginalAndReturn<injector::cstd<void ()>, 0x469FB0> (
        [] { (*((int *) 0xA447F4))++; });
}

/*******************************************************/
size_t
AdjustCodeForDYOM (FILE *file, void *buf, size_t len)
{
    size_t ret = CallAndReturn<size_t, 0x538950> (file, buf, len);

    char *signature = (char *) buf + 0x18CE0;
    if (std::string (signature, 4) == "DYOM")
        {
            uint8_t *slots
                = reinterpret_cast<uint8_t *> ((char *) buf + 0x18D90);
            *slots = 9;
            
            strcpy ((char *) buf + 0x18E11, "FILE9");

            if (DyomRandomizer::m_Config.RandomSpawn)
                {
                    float x = randomFloat (-3000.0, 3000.0);
                    float y = randomFloat (-3000.0, 3000.0);

                    injector::WriteMemory (uintptr_t (buf) + 75332, x);
                    injector::WriteMemory (uintptr_t (buf) + 75337, y);
                    injector::WriteMemory (uintptr_t (buf) + 75344, x);
                    injector::WriteMemory (uintptr_t (buf) + 75349, y);
                    injector::WriteMemory (uintptr_t (buf) + 75363, x);
                    injector::WriteMemory (uintptr_t (buf) + 75368, y);
                    injector::WriteMemory (uintptr_t (buf) + 75373, -100.0f);
                }
        }
    return ret;
}

/*******************************************************/
void
DyomRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "DYOMRandomizer",
            std::pair ("UseEnglishOnlyFilter", &m_Config.EnglishOnly),
            std::pair ("RandomSpawn", &m_Config.RandomSpawn),
            std::pair ("AutoTranslateToEnglish",
                       &m_Config.AutoTranslateToEnglish)))
        return;

    if (!ConfigManager::ReadConfig ("MissionRandomizer"))
        {
            RegisterDelayedHooks (
                {{HOOK_CALL, 0x469FB0,
                  (void *) &DyomHandleOnScriptOpCodeProcess}});

            RegisterDelayedFunction ([] { injector::MakeNOP (0x469fb5, 2); });
        }

    RegisterHooks ({{HOOK_CALL, 0x46683B, (void *) &StoreRandomizedDYOMScript},
                    {HOOK_CALL, 0x468E7F, (void *) AdjustCodeForDYOM}});

    Logger::GetLogger ()->LogMessage ("Intialised DyomRandomizer");
}

/*******************************************************/
int
DyomRandomizer::GetTotalNumberOfDYOMMissionPages (std::string list)
{
    std::string lists = internet.Get (list.c_str ()).GetString ();

    auto start = lists.find ("... <span class=pagelink>");
    start      = lists.find ("\' >", start) + 3;

    auto end = lists.find ("</a>", start);

    return std::stoi (lists.substr (start, end - start));
}

/*******************************************************/
int
CountOccurrencesInString (const std::string &str, const std::string &substr)
{
    int         count = 0;
    std::size_t found = str.find (substr);
    while (found != str.npos)
        {
            found = str.find (substr, found + 1);
            count++;
        }

    return count;
}

/*******************************************************/
std::size_t
GetNthOccurrenceOfString (const std::string &str, const std::string &substr,
                          int n)
{
    int         count = 0;
    std::size_t found = str.find (substr);
    while (found != str.npos)
        {
            if (count == n)
                return found;

            count++;
            found = str.find (substr, found + 1);
        }

    return 0;
}

/*******************************************************/
std::string
DyomRandomizer::GetRandomEntryFromPage (std::string page)
{
    std::string entries = internet.Get (page.c_str ()).GetString ();

    int entries_count = CountOccurrencesInString (entries, "<a href='show/");
    std::size_t start = GetNthOccurrenceOfString (entries, "<a href='show/",
                                                  random (entries_count - 1))
                        + 9;
    std::size_t end = entries.find ("'>", start + 1);

    return entries.substr (start, end - start);
}

/*******************************************************/
bool
IsDyomFileValid (std::vector<uint8_t> &data)
{
    if (data.size () < 32)
        return false;

    int version = *reinterpret_cast<int *> (data.data ());
    if (abs (version) > 6)
        return false;

    return true;
}

/*******************************************************/
void
DyomRandomizer::HandleExternalSubtitles ()
{
    if (!enableExternalSubtitles)
        return;
    
    if (prevObjectiveForSubtitles != ScriptSpace[9903])
        {
            sm_Session.ReportObjective (storedObjectives[ScriptSpace[9903]]);
            prevObjectiveForSubtitles = ScriptSpace[9903];
        }
}

/*******************************************************/
void
DyomRandomizer::SaveMission (const std::vector<uint8_t> &data)
{
    DYOM::DYOMFileStructure dyomFile;
    DyomTranslator          translator;

    dyomFile.Read (data);

    // Translation
    if (m_Config.AutoTranslateToEnglish)
        {
            originalName = dyomFile.g_HEADERSTRINGS[0];
            for (int i = 0; i < 100; i++)
                storedObjectives[i] = dyomFile.g_TEXTOBJECTIVES[i];

            translator.TranslateDyomFile (dyomFile);

            if (translator.GetDidTranslate ())
                {
                    prevObjectiveForSubtitles = -1;
                    enableExternalSubtitles   = true;
                    sm_Session.ReportObjective (originalName);
                    dyomFile.g_HEADERSTRINGS[0]
                        = "[TRANSLATED]" + dyomFile.g_HEADERSTRINGS[0];
                }
            else
                enableExternalSubtitles = false;
        }

    dyomFile.Save (CFileMgr::ms_dirName + "\\DYOM9.dat"s);
    dyomFile.Save (CFileMgr::ms_dirName + "\\DYOM8.dat"s);
}

/*******************************************************/
bool
DyomRandomizer::ParseMission (const std::string &url)
{
    std::string mission = internet.Get (url).GetString ();

    if (mission.find ("<a title='download for slot 1'  href='") == mission.npos)
        return false;

    std::vector<uint8_t> output;
    internet.Get ("download/" + url.substr (5)).GetResponse (output);

    if (!IsDyomFileValid (output))
        {
            Logger::GetLogger ()->LogMessage (
                "Dyom File downloaded failed verification: " + url);
            return false;
        }

    SaveMission (output);
    
    return true;
}

/*******************************************************/
void
DyomRandomizer::DownloadRandomMission ()
{
    if (InternetAttemptConnect (0) == ERROR_SUCCESS)
        {
            internet.Open ("dyom.gtagames.nl");
            std::string list;

            if (m_Config.EnglishOnly)
                list = "list?english=1&";
            else
                list = random (100) > 38 ? "list?" : "list_d?";

            int total_pages = GetTotalNumberOfDYOMMissionPages (list);

            //#define FORCED_MISSION "show/71368"

#ifdef FORCED_MISSION
            ParseMission (FORCED_MISSION);
#else
            while (!ParseMission (GetRandomEntryFromPage (
                list + "page=" + std::to_string (random (total_pages)))))
                ;
#endif

            internet.Close ();
        }
}

/*******************************************************/
void
DyomRandomizer::HandleAutoplay (CRunningScript *scr)
{
    static int previousOffset = 0;
    int currentOffset = scr->m_pCurrentIP - (unsigned char *) ScriptSpace;

    enum eState
    {
        STATE_INACTIVE,
        STATE_FADE_OUT,
        STATE_FADING,
        STATE_MISSION_DOWNLOAD,
        STATE_MISSION_PLAY,
        STATE_PASSFAIL_CHECK
    } static state
        = STATE_INACTIVE;

    static clock_t fadeOutStart = 0;
    static bool    downloadNew  = true;

    // Check to ensure that current script is the dyom script.
    if (currentOffset == previousOffset)
        return;

    sm_Session.ProcessTimer ();

    // Check for new game
    if (currentOffset == 91092)
        {
            state       = STATE_INACTIVE;
            downloadNew = true;
        }

    switch (state)
        {
            case STATE_INACTIVE: {

                // Autoplay for dyom sessions that request it
                if (currentOffset == 103522
                    && sm_Session.StartsAutomatically ())
                    {
                        state = STATE_MISSION_PLAY;
                        if ((char *) &ScriptSpace[10918] == "DYOM9.dat"s)
                            sm_Session.ReportStartSkip ();
                        else
                            sm_Session.ReportRetry ();
                    }

                if (currentOffset == 91111 && GetAsyncKeyState (VK_F4))
                    state = STATE_FADE_OUT;
                break;
            }

            case STATE_FADE_OUT: {
                if (currentOffset == 91111)
                    {
                        Scrpt::CallOpcode (0x16A, "do_fade", 250, 0);
                        fadeOutStart = clock ();
                        state        = STATE_FADING;
                    }
                break;
            }

            case STATE_FADING: {
                if (clock () - fadeOutStart > 250)
                    state = STATE_MISSION_DOWNLOAD;
                break;
            }

            case STATE_MISSION_DOWNLOAD: {
                char *scriptName = (char *) &ScriptSpace[10918];

                ScriptSpace[10922] = 0;

                strcpy (scriptName, downloadNew ? "DYOM9.dat" : "DYOM8.dat");
                scr->m_pCurrentIP = (unsigned char *) ScriptSpace + 95061;

                state = STATE_MISSION_PLAY;

                break;
            }

            case STATE_MISSION_PLAY: {
                if (currentOffset == 91111)
                    {
                        scr->m_pCurrentIP
                            = (unsigned char *) ScriptSpace + 91647;
                        ScriptSpace[16141] = 0;
                        state              = STATE_PASSFAIL_CHECK;
                    }
                break;
            }

            case STATE_PASSFAIL_CHECK: {
                // Mission Pass (no offset check to make it instant)
                if (ScriptSpace[16141])
                    {
                        state       = STATE_FADE_OUT;
                        downloadNew = true;

                        // Additional check to make sure that the mission has
                        // objectives.
                        if (ScriptSpace[10916] > 0)
                            sm_Session.ReportMissionPass ();
                    }

                // Mission Fail (+ pass check just to be safe)
                if (currentOffset == 91111)
                    {
                        state       = STATE_FADE_OUT;
                        downloadNew = ScriptSpace[16141];
                        if (!downloadNew)
                            state = STATE_MISSION_PLAY;
                        else
                            sm_Session.ReportMissionPass ();
                    }
                break;
            }
        }
}

/*******************************************************/
void
DyomRandomizer::HandleDyomScript (CRunningScript *scr)
{
    if (!CGame::bMissionPackGame)
        {
            mDyomScript = nullptr;
            return;
        }

    static int previousOffset = 0;
    int currentOffset = scr->m_pCurrentIP - (unsigned char *) ScriptSpace;

    if (currentOffset != previousOffset)
        {
            ScriptSpace[10915] = 0;

            if (currentOffset == 103522)
                {
                    if ((char *) &ScriptSpace[10918] == "DYOM9.dat"s)
                        DownloadRandomMission ();
                }
            if (currentOffset == 101718 || currentOffset == 101738)
                {
                    if (ScriptSpace[11439] == 9)
                        {
                            // Random Mission
                            strcpy ((char *) ScriptSpace[9889],
                                    "~n~Random Mission");

                            if (currentOffset == 101738)
                                scr->m_pCurrentIP
                                    = (unsigned char *) ScriptSpace + 101718;
                        }
                }
        }

    HandleAutoplay (scr);
    HandleExternalSubtitles ();
}

/*******************************************************/
void
DyomRandomizer::DestroyInstance ()
{
    if (DyomRandomizer::mInstance)
        delete DyomRandomizer::mInstance;
}

/*******************************************************/
DyomRandomizer *
DyomRandomizer::GetInstance ()
{
    if (!DyomRandomizer::mInstance)
        {
            DyomRandomizer::mInstance = new DyomRandomizer ();
            atexit (&DyomRandomizer::DestroyInstance);
        }
    return DyomRandomizer::mInstance;
}

bool DyomRandomizer::mEnabled = false;
