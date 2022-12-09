/*
    Rainbomizer - A (probably fun) Grand Theft Auto San Andreas Mod that
                  randomizes stuff
    Copyright (C) 2019 - Parik

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

 */

#include <string>
#define CPPTOML_NO_RTTI

#include <filesystem>
#include "config.hh"
#include "cpptoml/cpptoml.h"
#include <cstdlib>
#include "logger.hh"
#include "config_default.hh"
#include <sstream>
#include <string>
#include "base.hh"

/*******************************************************/
bool
DoesFileExist (const std::string &file)
{
    FILE *f = OpenRainbomizerFile (file, "r");
    if (f)
        {
            fclose (f);
            return true;
        }
    return false;
}

/*******************************************************/
void
ConfigManager::WriteDefaultConfig (const std::string &file)
{
    FILE *f = OpenRainbomizerFile (file.c_str (), "wb");
    if (f)
        {
            fwrite (config_toml, sizeof (config_toml) - 1, 1, f);
            fclose (f);
        }
}

/*******************************************************/
std::shared_ptr<cpptoml::table>
ConfigManager::ParseDefaultConfig ()
{
    // Read the default config file
    auto stream = std::istringstream (config_toml);

    cpptoml::parser p{stream};
    return p.parse ();
}

/*******************************************************/
ConfigManager::ConfigManager (const std::string &file)
{
    Logger::GetLogger ()->LogMessage (GetRainbomizerFileName (file));
    m_pDefaultConfig = ParseDefaultConfig ();
    try
        {
            m_pConfig = cpptoml::parse_file (GetRainbomizerFileName (file));
        }
    catch (const std::exception &e)
        {
            Logger::GetLogger ()->LogMessage (e.what ());

            if (!DoesFileExist (file))
                WriteDefaultConfig (file);

            m_pConfig = m_pDefaultConfig;
        }
}

/*******************************************************/
ConfigManager *
ConfigManager::GetInstance ()
{
    static ConfigManager mgr ("config.toml");
    return &mgr;
}

/*******************************************************/
bool
ConfigManager::GetIsEnabled (const std::string &name)
{
    // Finds "name" key in the main table. Also allows an "Enabled" key in the
    // table for the randomizer/whatever.

    // Example:
    // TrafficRandomizer = true
    // ColourRandomizer = false
    // [ColourRandomizer]
    // Enabled = true

    // Will be parsed as TrafficRandomizer and ColourRandomizer enabled.
    // Enabled key takes precedence over main table key.

    bool enabled = true;
    ReadValue ("Randomizers", name, enabled);
    ReadValue (name, "Enabled", enabled);

    // Logger::GetLogger ()->LogMessage (name + ": "
    //                                  + std::to_string (enabled));

    return enabled;
}

template <typename T>
void
ConfigManager::ReadValue (const std::string &tableName, const std::string &key,
                          T &out)
{
    auto table    = m_pConfig->get_table (tableName);
    auto defTable = m_pDefaultConfig->get_table (tableName);
    if (table)
        out = table->get_as<T> (key).value_or (
            (defTable) ? defTable->get_as<T> (key).value_or (out) : out);
}

#define READ_VALUE_ADD_TYPE(type)                                              \
    template void ConfigManager::ReadValue<type> (                             \
        const std::string &tableName, const std::string &key, type &out);

READ_VALUE_ADD_TYPE (bool)
READ_VALUE_ADD_TYPE (int)
READ_VALUE_ADD_TYPE (double)
READ_VALUE_ADD_TYPE (std::string)
