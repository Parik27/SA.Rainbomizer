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

#define CPPTOML_NO_RTTI

#include "config.hh"
#include "cpptoml/cpptoml.h"
#include <cstdlib>
#include "logger.hh"
#include "config_default.hh"
#include <sstream>

ConfigManager *ConfigManager::mInstance = nullptr;

#define CONFIG(table, member, key, type)                                       \
    this->member = table->get_as<type> (key).value_or (this->member);

/*******************************************************/
void
BaseConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
}

/*******************************************************/
void
HandlingConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
}

/*******************************************************/
void
WeaponConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
    CONFIG (table, skipChecks, "SkipChecks", bool);
    CONFIG (table, playerRandomization, "RandomizePlayerWeapons", bool);

    // Read Pattern
    if (auto patterns = table->get_table_array ("Patterns"))
        {
            for (const auto &pattern : *patterns)
                {
                    WeaponPattern _pattern;
                    _pattern.weapon
                        = pattern->get_as<int> ("weapon").value_or (-1);
                    _pattern.ped = pattern->get_as<int> ("ped").value_or (-1);

                    _pattern.thread
                        = pattern->get_as<std::string> ("thread").value_or ("");

                    auto read_table = [&] (std::string           key,
                                           std::vector<int64_t> &v) {
                        if (auto table = pattern->get_array (key))
                            {
                                for (const auto &val : *table)
                                    {
                                        switch ((*val).type ())
                                            {
                                            case cpptoml::base_type::INT:
                                                v.push_back (
                                                    val.get ()
                                                        ->as<int64_t> ()
                                                        ->get ());
                                                break;

                                                case cpptoml::base_type::
                                                    STRING: {
                                                    std::string value
                                                        = val.get ()
                                                              ->as<
                                                                  std::
                                                                      string> ()
                                                              ->get ();

                                                    if (value == "slot")
                                                        v.push_back (
                                                            WEAPON_SLOT);

                                                    break;
                                                }

                                            default: continue;
                                            }
                                    }
                            }
                    };
                    read_table ("allowed", _pattern.allowed);
                    read_table ("denied", _pattern.denied);

                    this->patterns.push_back (_pattern);
                }
        }

    // Fallback Pattern
    this->patterns.push_back (WeaponPattern ());
}

/*******************************************************/
void
GeneralConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);
    CONFIG (table, seed, "Seed", int);
}

/*******************************************************/
void
TrafficConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);
    CONFIG (table, forcedVehicleEnabled, "ForcedVehicleEnabled", bool);
    CONFIG (table, forcedVehicleID, "ForcedVehicleID", int);
    CONFIG (table, enableTrains, "EnableTrains", bool);
    CONFIG (table, enableBoats, "EnableBoats", bool);
    CONFIG (table, enableAircrafts, "EnableAircrafts", bool);
    CONFIG (table, enableCars, "EnableCars", bool);
    CONFIG (table, enableBikes, "EnableBikes", bool);
    CONFIG (table, enableTrailers, "EnableTrailers", bool);
    CONFIG (table, defaultModel, "DefaultModel", int);
}

/*******************************************************/
void
CarColConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);

    this->exceptions = table->get_array_of<int64_t> ("Exceptions")
                           .value_or (this->exceptions);
}

/*******************************************************/
void
CheatConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);

    CONFIG (table, enableEasterEgg, "EnableEasterEgg", bool);
}

/*******************************************************/
void
ParkedCarConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);

    CONFIG (table, randomizeFixedSpawns, "RandomizeFixedSpawns", bool);
    CONFIG (table, randomizeRandomSpawns, "RandomizeRandomSpawns", bool);
}

/*******************************************************/
void
SoundsConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);

    CONFIG (table, matchSubtitles, "MatchSubtitles", bool);
    CONFIG (table, audioEventsFile, "AudioEventsFile", std::string);
}

/*******************************************************/
void
ScriptVehicleConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);

    CONFIG (table, skipChecks, "SkipChecks", bool);
}

/*******************************************************/
bool
DoesFileExist (const std::string &file)
{
    FILE *f = fopen (file.c_str (), "r");
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
    FILE *f = fopen (file.c_str (), "wb");
    fwrite (config_toml, config_toml_len, 1, f);
    fclose (f);
}

/*******************************************************/
std::shared_ptr<cpptoml::table>
ConfigManager::ParseDefaultConfig ()
{
    // Read the default config file
    auto stream = std::istringstream (
        std::string ((char *) config_toml, config_toml_len));

    cpptoml::parser p{stream};
    return std::move (p.parse ());
}

/*******************************************************/
void
ConfigManager::Initialise (const std::string &file)
{
    std::shared_ptr<cpptoml::table> config;
    try
        {
            config = cpptoml::parse_file (file);
        }
    catch (std::exception e)
        {
            Logger::GetLogger ()->LogMessage (e.what ());
            config = ParseDefaultConfig ();

            if (!DoesFileExist (file))
                WriteDefaultConfig (file);
        }

    mConfigs.general.Read (config);
    mConfigs.traffic.Read (config->get_table ("TrafficRandomizer"));
    mConfigs.carcol.Read (config->get_table ("CarColRandomizer"));
    mConfigs.policeHeli.Read (config->get_table ("PoliceHeliRandomizer"));
    mConfigs.cheat.Read (config->get_table ("CheatRandomizer"));
    mConfigs.handling.Read (config->get_table ("HandlingRandomizer"));
    mConfigs.weapon.Read (config->get_table ("WeaponRandomizer"));
    mConfigs.parkedCar.Read (config->get_table ("ParkedCarRandomizer"));
    mConfigs.licensePlate.Read (config->get_table ("LicensePlateRandomizer"));
    mConfigs.sounds.Read (config->get_table ("SoundsRandomizer"));
    mConfigs.scriptVehicle.Read (config->get_table ("ScriptVehicleRandomizer"));
}

/*******************************************************/
void
ConfigManager::DestroyInstance ()
{
    if (ConfigManager::mInstance)
        delete ConfigManager::mInstance;
}

/*******************************************************/
ConfigManager *
ConfigManager::GetInstance ()
{
    if (!ConfigManager::mInstance)
        {
            ConfigManager::mInstance = new ConfigManager ();
            atexit (&ConfigManager::DestroyInstance);
        }
    return ConfigManager::mInstance;
}

/*******************************************************/
const Configs &
ConfigManager::GetConfigs ()
{
    return this->mConfigs;
}
