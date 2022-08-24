#include "util/dyom/Translation.hh"

#include "logger.hh"

#include <algorithm>
#include <chrono>

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
void
DyomTranslator::EnqueueTranslation (std::string &text)
{
    if (translationQueue.size () + text.size () > 5000)
        DoTranslate ();

    translationQueue += " ~" + std::to_string (queueCounter) + "~";
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

            if (start != translated.npos)
                {
                    auto &out = *translationOut[i];

                    out = translated.substr (start, end - start);
                    FixupGxtTokens (out);
                    out = out.substr (0, 99);
                    trim (out);
                    Logger::GetLogger()->LogMessage(out);
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
    std::string response
        = internet.Get ("m?sl=auto&tl=en&q=" + text).GetString ();

    std::regex  rtext ("result-container\">(.*?)<");
    std::cmatch cm;
    if (!std::regex_search (response.c_str (), cm, rtext))
        return text;

    std::string translation = cm[1];

    // translator tends to break tags with spaces, attempt to fix
    translation
        = std::regex_replace (translation,
                              std::regex ("~\\s*([a-zA-Z0-9]+)\\s*~"), "~$1~");
    DecodeSpecialChars (translation);

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
