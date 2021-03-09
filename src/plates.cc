#include "plates.hh"
#include <cstdlib>
#include "base.hh"
#include "logger.hh"
#include <cstring>
#include "util/text.hh"
#include "functions.hh"
#include <algorithm>
#include "config.hh"

LicensePlateRandomizer *LicensePlateRandomizer::mInstance = nullptr;

/*******************************************************/
const char *__fastcall GetNewCustomPlateText (void *thisInfo, void *edx)
{
    return GxtManager::GetRandomWord ();
}

/*******************************************************/
void __fastcall InitialiseText (CText *text, void *edx, char a2)
{
    text->Load (a2);
    GxtManager::Initialise (text);
}

/*******************************************************/
void
GenerateNewText (char *buf)
{
    memset (buf, 0, 8);

    const char *word   = GxtManager::GetRandomWord ();
    int         length = strlen (word);

    strncpy (buf, word, std::min (length, 8));
}

/*******************************************************/
void
GetNewPlateText (char *buf, int len)
{
    GenerateNewText (buf);
}

/*******************************************************/
void
LicensePlateRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("LicensePlateRandomizer"))
        return;

    Logger::GetLogger ()->LogMessage ("Intialised LicensePlateRandomizer");
    RegisterHooks ({{HOOK_CALL, 0x6D10FB, (void *) &GetNewCustomPlateText},
                    {HOOK_CALL, 0x4C9484, (void *) &GetNewPlateText},
                    {HOOK_CALL, 0x468E9A, (void *) &InitialiseText},
                    {HOOK_CALL, 0x618E97, (void *) &InitialiseText},
                    {HOOK_CALL, 0x5BA167, (void *) &InitialiseText}});
}

/*******************************************************/
void
LicensePlateRandomizer::DestroyInstance ()
{
    if (LicensePlateRandomizer::mInstance)
        delete LicensePlateRandomizer::mInstance;
}

/*******************************************************/
LicensePlateRandomizer *
LicensePlateRandomizer::GetInstance ()
{
    if (!LicensePlateRandomizer::mInstance)
        {
            LicensePlateRandomizer::mInstance = new LicensePlateRandomizer ();
            atexit (&LicensePlateRandomizer::DestroyInstance);
        }
    return LicensePlateRandomizer::mInstance;
}
