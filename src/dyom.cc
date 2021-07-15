#include "dyom.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include "injector/calling.hpp"
#include <wininet.h>
#include "config.hh"
#include <cstring>
#include "missions.hh"

using namespace std::literals;

DyomRandomizer *DyomRandomizer::mInstance = nullptr;

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

            char *column_name = (char *) buf + 0x18E11;
            strcpy ((char *) buf + 0x18E11, "FILE9");
        }
    return ret;
}

/*******************************************************/
void
DyomRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("DYOMRandomizer"))
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
bool
ReadRequestResponse (HANDLE request, std::vector<uint8_t> &out)
{
    DWORD dwSize;
    DWORD dwDownloaded;

    for (;;)
        {

            if (!InternetQueryDataAvailable (request, &dwSize, 0, 0))
                {
                    Logger::GetLogger ()->LogMessage (
                        "InternetQueryDataAvailable failed "
                        + std::to_string (GetLastError ()));

                    return false;
                }
            auto lpszData = new TCHAR[dwSize + 1];
            if (!InternetReadFile (request, lpszData, dwSize, &dwDownloaded))
                {
                    Logger::GetLogger ()->LogMessage (
                        "InternetReadFile failed "
                        + std::to_string (GetLastError ()));

                    delete[] lpszData;
                    break;
                }

            out.insert (out.end (), lpszData, lpszData + dwSize);

            if (dwDownloaded == 0)
                break;
        }

    InternetCloseHandle (request);
    return true;
}

/*******************************************************/
HANDLE
MakeRequest (HANDLE session, const std::string &file)
{
    HANDLE request = HttpOpenRequest (session, "GET", file.c_str (), NULL, NULL,
                                      NULL, INTERNET_FLAG_SECURE, 0);
    HttpSendRequest (request, NULL, 0, NULL, 0);

    return request;
}

/*******************************************************/
HANDLE
OpenSession (HINTERNET internet)
{
    return InternetConnect (internet, "dyom.gtagames.nl",
                            INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL,
                            INTERNET_SERVICE_HTTP, 0, 0);
}

/*******************************************************/
std::string
ReadStringFromRequest (HANDLE request)
{
    std::vector<uint8_t> output;
    ReadRequestResponse (request, output);

    return std::string (output.begin (), output.end ()).c_str ();
}

/*******************************************************/
int
DyomRandomizer::GetTotalNumberOfDYOMMissionPages (HANDLE      session,
                                                  std::string list)
{
    HANDLE      request = MakeRequest (session, list.c_str ());
    std::string lists   = ReadStringFromRequest (request);

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
const std::size_t
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
DyomRandomizer::GetRandomEntryFromPage (HANDLE session, std::string page)
{
    std::string entries
        = ReadStringFromRequest (MakeRequest (session, page.c_str ()));

    int entries_count = CountOccurrencesInString (entries, "<a href='show/");
    std::size_t start = GetNthOccurrenceOfString (entries, "<a href='show/",
                                                  random (entries_count - 1))
                        + 9;
    std::size_t end = entries.find ("'>", start + 1);

    return entries.substr (start, end - start);
}

/*******************************************************/
bool
DyomRandomizer::ParseMission (HANDLE session, const std::string &url)
{
    std::string mission = ReadStringFromRequest (MakeRequest (session, url));
    if (mission.find ("<a title='download for slot 1'  href='") == mission.npos)
        return false;

    std::vector<uint8_t> output;
    ReadRequestResponse (MakeRequest (session, "download/" + url.substr (5)),
                         output);
    FILE *file = fopen ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (), "wb");
    fwrite (output.data (), 1, output.size (), file);

    return true;
}

/*******************************************************/
void
DyomRandomizer::DownloadRandomMission ()
{
    if (InternetAttemptConnect (0) == ERROR_SUCCESS)
        {
            HINTERNET handle
                = InternetOpen ("123robot", INTERNET_OPEN_TYPE_PRECONFIG, NULL,
                                NULL, 0);

            HANDLE session = OpenSession (handle);

            std::string list = random (100) > 38 ? "list" : "list_d";

            int total_pages = GetTotalNumberOfDYOMMissionPages (session, list);
            while (!ParseMission (
                session,
                GetRandomEntryFromPage (session, list + "?page="
                                                     + std::to_string (random (
                                                         total_pages)))))
                ;

            CloseHandle (session);
            CloseHandle (handle);
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
