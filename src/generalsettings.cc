#include "generalsettings.hh"
#include "config.hh"
#include "base.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"

GeneralSettings *GeneralSettings::mInstance = nullptr;

/*******************************************************/
void
GeneralSettings::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "EnableRainbomizer", std::pair ("Seed", &m_Config.Seed),
            std::pair ("Unprotect", &m_Config.Unprotect),
            std::pair ("AutosaveSlot", &m_Config.AutoSaveSlot),
            std::pair ("ModifyCredits", &m_Config.ModifyCredits)))
        return;

    Logger::GetLogger ()->LogMessage ("Initialising Rainbomizer...");
}

/*******************************************************/
void
GeneralSettings::DestroyInstance ()
{
    if (GeneralSettings::mInstance)
        delete GeneralSettings::mInstance;
}

/*******************************************************/
GeneralSettings *
GeneralSettings::GetInstance ()
{
    if (!GeneralSettings::mInstance)
        {
            GeneralSettings::mInstance = new GeneralSettings ();
            atexit (&GeneralSettings::DestroyInstance);
        }
    return GeneralSettings::mInstance;
}