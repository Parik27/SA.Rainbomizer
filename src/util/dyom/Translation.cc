#include "util/dyom/Translation.hh"

#include "base.hh"
#include "logger.hh"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <config.hh>

static inline void
ltrim (std::string &s)
{
    s.erase (s.begin (),
             std::find_if (s.begin (), s.end (), [] (unsigned char ch) {
                 return !std::isspace (ch);
             }));
}

// trim from end (in place)
static inline void
rtrim (std::string &s)
{
    s.erase (std::find_if (s.rbegin (), s.rend (),
                           [] (unsigned char ch) { return !std::isspace (ch); })
                 .base (),
             s.end ());
}

// trim from both ends (in place)
static inline void
trim (std::string &s)
{
    ltrim (s);
    rtrim (s);
}

/*******************************************************/
std::string
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
DyomTranslator::DyomTranslator (const std::string &translationChain,
                                const std::string &charactersMap)
{
    internet.Open ("translate.google.com");

    if (translationChain.empty ())
        mTranslationChain.push_back ("en");
    else
        {
            std::istringstream iss (translationChain);

            for (std::string token; std::getline (iss, token, ';');)
                mTranslationChain.push_back (std::move (token));
        }

    if (!charactersMap.empty ())
        {
            std::istringstream iss (charactersMap);
            std::string        token_v;
            char               ch = '0';
            for (std::string token; std::getline (iss, token, ';')
                                    && std::getline (iss, token_v, ';');)
                {
                    ch = (char)std::stoi (token_v, nullptr, 16);
                    mCharacterMap.insert (std::pair<std::string, char> (
                        std::move (token),
                        ch));
                }
        }
}

/*******************************************************/
void
DyomTranslator::EnqueueTranslation (std::string &text)
{
    if (translationQueue.size () + text.size () > 5000)
        DoTranslate ();

    translationQueue += " ~" + std::to_string (queueCounter) + "~";
    if (text == "_")
        return;

    if (text.back () == '\n')
        text.pop_back ();

    translationQueue += text;

    translationOut.push_back (&text);

    queueCounter++;
}

/*******************************************************/
void
DyomTranslator::DoTranslate ()
{
    std::string translated = TranslateText (translationQueue);

    Logger::GetLogger ()->LogMessage (translated);
    Logger::GetLogger ()->LogMessage (translationQueue);

    ProcessDidTranslate (translated);
    
    for (int i = 0; i < queueCounter && didTranslate; i++)
        {
            auto delim    = "~" + std::to_string (i) + "~";
            auto delimEnd = "~" + std::to_string (i + 1) + "~";

            auto start = translated.find (delim) + delim.size ();
            auto end   = translated.find (delimEnd);

            if (start != translated.npos
                && (end != translated.npos || i == queueCounter - 1))
                {
                    auto &out = *translationOut[i];

                    out = translated.substr (start, end - start);
                    FixupGxtTokens (out);
                    out = out.substr (0, 99);
                    trim (out);
                    Logger::GetLogger ()->LogMessage (out);
                }
        }

    translationQueue = "";
    queueCounter     = 0;
    translationOut.clear ();
}

/*******************************************************/
void
DyomTranslator::ProcessDidTranslate (std::string translated)
{
    std::string orig = translationQueue;

    // Remove whitespaces and other special characters since google translate
    // tends to mess with these
    auto cleanupString = [] (auto &str) {
        auto it = std::remove_if (str.begin (), str.end (), [] (char const &c) {
            return !std::isalnum (c);
        });
        str.erase (it, str.end ());
    };

    cleanupString (orig);
    cleanupString (translated);

    if (orig != translated)
        didTranslate = true;
}

/*******************************************************/
void
DyomTranslator::TranslateDyomFile (DYOM::DYOMFileStructure &file)
{
    using namespace std::chrono;
    auto start = high_resolution_clock::now ();

    for (int i = 0; i < 3; i++)
        if (file.g_HEADERSTRINGS[i].size () > 1)
            EnqueueTranslation (file.g_HEADERSTRINGS[i]);

    for (int i = 0; i < 100; i++)
        if (file.g_TEXTOBJECTIVES[i].size () > 1)
            EnqueueTranslation (file.g_TEXTOBJECTIVES[i]);

    DoTranslate ();

    auto stop     = high_resolution_clock::now ();
    auto duration = duration_cast<microseconds> (stop - start);

    Logger::GetLogger ()->LogMessage (
        "Translating mission took "
        + std::to_string (duration.count () / 1000.0f / 1000.0f) + " seconds");
}

/*******************************************************/
std::string
DyomTranslator::TranslateText (const std::string &text)
{
    std::string translation = text;
    std::regex  rtext ("result-container\">([\\S\\s]*?)<");
    
    for (const auto &lang : mTranslationChain)
        {
            std::string request = "m?sl=auto&tl=";
            request += lang;
            request += "&q=";
            // languages with multibyte characters will break everything unless
            // we encode here
            request += EncodeURL (translation);

            std::string response = internet.Get (request.c_str ()).GetString ();
            std::cmatch cm;
            if (!std::regex_search (response.c_str (), cm, rtext))
                {
                    MessageBox (NULL, "TRANSLATION FAILED!!", NULL,
                                MB_ICONHAND);
                    FILE *f = GetRainbomizerDataFile ("test.html", "w");
                    fwrite (response.data (), 1, response.size (), f);
                    fclose (f);
                    return text;
                }

            translation = cm[1];
        }

    for (const auto &[key, value] : mCharacterMap)
        {
            std::string::size_type pos = 0;
            while ((pos = translation.find (key, pos)) != std::string::npos)
                {
                    translation.replace (pos, key.length(), 1, value);
                    pos += 1;
                }
        }

    // translator tends to break tags with spaces, attempt to fix
    DecodeSpecialChars (translation);
    translation
        = std::regex_replace (translation,
                              std::regex ("~\\s*([a-zA-Z0-9]+)\\s*~"), "~$1~");

    return translation;
}

/*******************************************************/
void
DyomTranslator::DecodeSpecialChars (std::string &text)
{
#define SPECIAL_CHAR(enc, orig)                                                \
    text = std::regex_replace (text, std::regex (enc), orig);

    SPECIAL_CHAR ("&#39;", "'");
    SPECIAL_CHAR ("&quot;", "\"");
    SPECIAL_CHAR ("&lt;", "<");
    SPECIAL_CHAR ("&gt;", ">");
    SPECIAL_CHAR ("&amp;", "&");
    SPECIAL_CHAR ("\xe2\x80\x99", "'"); //UTF-8 quotation mark

    if (mCharacterMap.size () < 1)
    // Remove any other non-ascii characters
    text.erase (std::remove_if (text.begin (), text.end (),
                                [] (char ch) { return ch < 0; }),
                text.end ());
}

/*******************************************************/
void
DyomTranslator::FixupGxtTokens (std::string &text)
{
    // remove remaining broken tags (crashes otherwise)
    std::size_t pos = 0;
    while (true)
        {
            pos = text.find ("~", pos);
            if (pos == text.npos)
                break;

            if (text.size () <= pos + 2 || text[pos + 2] != '~')
                text[pos] = ' ';

            pos += 3;
        }
}
