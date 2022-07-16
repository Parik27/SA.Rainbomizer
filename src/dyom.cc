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
#include <regex>

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
    if (!ConfigManager::ReadConfig ("DYOMRandomizer", 
        std::pair("UseEnglishOnlyFilter", &m_Config.EnglishOnly)))
        return;

    if (!ConfigManager::ReadConfig ("DYOMRandomizer",
                                    std::pair ("AutoTranslateToEnglish",
                                               &m_Config.Translate)))
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
HANDLE
OpenSessionTranslator (HINTERNET internet)
{
    return InternetConnect (internet, "translate.google.com",
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
    fclose (file);
    FILE *file2 = fopen ((CFileMgr::ms_dirName + "\\DYOM8.dat"s).c_str (), "wb");
    fwrite (output.data (), 1, output.size (), file2);
    fclose (file2);

    return true;
}

/*******************************************************/
bool
DyomRandomizer::TranslateMission (HANDLE session)
{
    byte input[40000];
    byte  output[40000];
    FILE *file = fopen ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (), "rb");
    std::size_t read = fread (input, 1, 40000, file);
    fclose (file);
    if (read < 1 || input[0] == 0x00) //corrupted
        return false;
    //copy first 4 bytes to the resulting array
    memcpy (output, input, 4);
    std::size_t o_pos = 4;
    //get mission name string
    std::string text = "";
    int   i_pos = 4;
    while (input[i_pos] != 0x00)
        {
            text+=(char)input[i_pos];
            i_pos++;
        }
    std::size_t cut_pos = i_pos;
    //translate the mission name and write it to the resulting array
    std::string translation = TranslateText (session, text);
    memcpy (output + o_pos, translation.data (), translation.size ());
    o_pos += translation.size ();
    //write author name as-is
    i_pos++;
    while (input[i_pos] != 0x00)
        {
            i_pos++;
        }
    i_pos++;
    memcpy (output + o_pos, input+cut_pos, i_pos-cut_pos);
    o_pos += i_pos - cut_pos;
    //translate 3 intro text fields
    int counter = 0;
    text        = "";
    while (true)
        {
            if (input[i_pos] != 0x00)
                text += (char) input[i_pos];
            else
                {
                    if (text.length () > 1)
                        {
                            translation = TranslateText (session, text);
                        }
                    else
                        {
                            translation = text;
                        }
                    memcpy (output + o_pos, translation.c_str (),
                            translation.size ()+1);
                    o_pos += translation.size () + 1;
                    text                    = "";
                    counter++;
                }
            i_pos++;
            if (counter >= 3)
                break;
        }
    cut_pos = i_pos;
    //depending on the version objectives texts offset a little bit different to the header
    if (input[i_pos + 1] == NULL)
        i_pos += 0x194F;
    else
        i_pos += 0x195F;
    //write everything inbetween mission name and objectives texts to the resulting array
    memcpy (output + o_pos, input+cut_pos, i_pos-cut_pos);
    o_pos += i_pos - cut_pos;
    //translate and write objectives to the resulting array (100 max)
    counter = 0;
    text    = "";
    while (true)
        {
            if (input[i_pos] != 0x00)
                text += (char) input[i_pos];
            else
                {
                    if (text.length () > 1)
                        {
                            translation = TranslateText (session, text);
                        }
                    else
                        {
                            translation = text;
                    }
                    memcpy (output + o_pos, translation.c_str (),
                            translation.size ()+1);
                    o_pos += translation.size () + 1;
                    text                    = "";
                    counter++;
                }
            i_pos++;
            if (counter >= 100)
                break;
        }
    //write the rest of the file to the resulting array
    memcpy (output + o_pos, input + i_pos, read - i_pos);
    //rename original file
    std::remove ((CFileMgr::ms_dirName + "\\DYOM9_orig.dat"s).c_str ());
    std::rename ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (),
                 (CFileMgr::ms_dirName + "\\DYOM9_orig.dat"s).c_str ());
    //finally write the file with translated mission
    FILE *file2 = fopen ((CFileMgr::ms_dirName + "\\DYOM9.dat"s).c_str (), "wb");
    fwrite (output, 1, o_pos + read - i_pos, file2);
    fclose (file2);
    FILE *file3
        = fopen ((CFileMgr::ms_dirName + "\\DYOM8.dat"s).c_str (), "wb");
    fwrite (output, 1, o_pos + read - i_pos, file3);
    fclose (file3);
    return true;
}

/*******************************************************/
std::string
DyomRandomizer::TranslateText (HANDLE session, const std::string &text)
{
    std::string response = ReadStringFromRequest (
        MakeRequest (session, "m?sl=auto&tl=en&q=" + text));
    std::size_t pos = response.find ("\"result-container\"");
    if (pos == response.npos)
        return text;
    std::size_t pos2 = GetNthOccurrenceOfString (response, "</div>", 7);
    std::string translation = response.substr (pos + 19, pos2 - pos - 19);
    //fix quotation marks
    translation = std::regex_replace (translation,
                                      std::regex ("&#39;"), "'");
    //translator tends to break tags with spaces, attempt to fix
    translation = std::regex_replace(translation,std::regex("(~)\\s*([a-zA-Z])\\s*(~)"),"$1$2$3");
    //trim everything above 99 symbols (crashes overwise)
    if (translation.length () > 99)
        {
            translation = translation.substr (0, 99);
    }
    // remove remaining broken tags (crashes overwise)
    pos = 0;
    while (true)
        {
            std::size_t tild = translation.find ("~", pos);
            if (tild != translation.npos)
                {
                    if (translation.substr (tild + 2, 1).compare ("~") != 0)
                        {
                            translation = translation.replace (tild, 1, "");
                            pos         = tild;
                        }
                    else
                        pos = tild + 3;
                }
            else
                break;
        }
    return translation;
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

            std::string list;

            if (m_Config.EnglishOnly)
                list = "list?english=1&";
            else
                list = random (100) > 38 ? "list?" : "list_d?";

            int total_pages = GetTotalNumberOfDYOMMissionPages (session, list);
            while (!ParseMission (
                session,
                GetRandomEntryFromPage (session, list + "page="
                                                     + std::to_string (random (
                                                         total_pages)))))
                ;

            CloseHandle (session);
            CloseHandle (handle);

            if (m_Config.Translate)
                {
                    HINTERNET handle2
                        = InternetOpen ("123robot",
                                        INTERNET_OPEN_TYPE_PRECONFIG, NULL,
                                        NULL, 0);

                    HANDLE session2 = OpenSessionTranslator (handle2);
                    TranslateMission (session2);
                    CloseHandle (session2);
                    CloseHandle (handle2);
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
