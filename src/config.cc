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
#include "scm.hh"
#include "base.hh"

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
MissionConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
    CONFIG (table, forcedMissionEnabled, "ForcedMissionEnabled", bool);
    CONFIG (table, forcedMissionID, "ForcedMissionID", int);
    CONFIG (table, shufflingEnabled, "RandomizeOnce", bool);
    CONFIG (table, forceShufflingSeed, "ForcedRandomizeOnceSeed", bool);
    CONFIG (table, disableMainScmCheck, "DisableMainScmCheck", bool);

    std::string seed
        = table->get_as<std::string> ("RandomizeOnceSeed").value_or ("");

    if (seed != "")
        this->shufflingSeed = std::hash<std::string>{}(seed);
}

/*******************************************************/
void
ObjectConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
}

/*******************************************************/
void
CutsceneConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
    CONFIG (table, cutsceneFile, "CutsceneFile", std::string);
    CONFIG (table, randomizeModels, "RandomizeModels", bool);
    CONFIG (table, randomizeLocations, "RandomizeLocations", bool);
}

/*******************************************************/
void
ParticlesConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
}

/*******************************************************/
void
WeaponConfig::ReadTable (std::shared_ptr<cpptoml::table> pattern,
                         const std::string &key, std::vector<int64_t> &v)
{
    if (auto table = pattern->get_array (key))
        {
            for (const auto &val : *table)
                {
                    switch ((*val).type ())
                        {
                        case cpptoml::base_type::INT:
                            v.push_back (val.get ()->as<int64_t> ()->get ());
                            break;

                            case cpptoml::base_type::STRING: {
                                std::string value
                                    = val.get ()->as<std::string> ()->get ();

                                if (value == "slot")
                                    v.push_back (WEAPON_SLOT);

                                break;
                            }

                        default: continue;
                        }
                }
        }
}

void LanguageConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
    CONFIG (table, MinTimeBeforeTextChange, "MinTimeBeforeTextChange", int);
    CONFIG (table, MaxTimeBeforeTextChange, "MaxTimeBeforeTextChange", int);
}

/*******************************************************/
void
WeaponStatsConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
}

/*******************************************************/
void
RespawnPointConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if(!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
}

/*******************************************************/
void
WantedLevelConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    CONFIG (table, enabled, "Enabled", bool);
    CONFIG (table, missions, "RandomizeMissionWantedLevels", bool);
    CONFIG (table, chaos, "RandomizeChaosPoints", bool);
}

/*******************************************************/
void
WeaponConfig::ReadPattern (std::shared_ptr<cpptoml::table> pattern)
{
    WeaponPattern _pattern;
    _pattern.weapon = pattern->get_as<int> ("weapon").value_or (-1);
    _pattern.ped    = pattern->get_as<int> ("ped").value_or (-1);

    _pattern.thread = pattern->get_as<std::string> ("thread").value_or ("");

    ReadTable (pattern, "allowed", _pattern.allowed);
    ReadTable (pattern, "denied", _pattern.denied);

    this->patterns.push_back (_pattern);
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
                ReadPattern (pattern);
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
    CONFIG (table, unprotect, "Unprotect", int);
    CONFIG (table, enable_credits, "ModifyCredits", bool);
    CONFIG (table, save_slot, "AutosaveSlot", int);
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
ColourConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);
    CONFIG (table, cars, "RandomizeCarCols", bool);
    CONFIG (table, fades, "RandomizeFades", bool);
    CONFIG (table, texts, "RainbowText", bool);
    CONFIG (table, hueCycle, "RainbowHueCycle", bool);
    CONFIG (table, crazyMode, "CrazyMode", bool);

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
PickupsConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);

    puts ("Reading Pickups");

    CONFIG (table, sameType, "ReplaceWithWeaponsOnly", bool);
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
    CONFIG (table, forceAudioEnabled, "ForcedAudioLineEnabled", bool);
    CONFIG (table, forceAudioID, "ForcedAudioLine", int);
}

/*******************************************************/
void
ScriptVehicleConfig::ReadTable (std::shared_ptr<cpptoml::table> pattern,
                                const std::string &key, std::vector<int16_t> &v)
{

    auto int_array = pattern->get_array_of<int64_t> (key);
    auto str_array = pattern->get_array_of<std::string> (key);
    if (int_array)
        {
            for (const auto &vehicle : *int_array)
                {
                    if (vehicle < 400 || vehicle > 611)
                        continue;

                    v.push_back (vehicle);
                }
        }
    else if (str_array)
        {
            for (const auto &vehicle : *str_array)
                {
                    v.push_back (StrToVehicleType (vehicle));
                }
        }
}

/*******************************************************/
int
ScriptVehicleConfig::StrToVehicleType (const std::string &str)
{

    std::unordered_map<std::string, int> vehicle_maps
        = {{"cars", VEHICLES_CARS},      {"bikes", VEHICLES_BIKES},
           {"boats", VEHICLES_BOATS},    {"helis", VEHICLES_HELIS},
           {"planes", VEHICLES_PLANES},  {"bmx", VEHICLES_BMX},
           {"all", VEHICLES_ALL},        {"trains", VEHICLES_TRAINS},
           {"rc", VEHICLE_APPEARANCE_RC}};

    if (vehicle_maps.count (str) == 1)
        return vehicle_maps[str];

    try
        {
            int vehicle = std::stoi (str);
            if (vehicle >= 400 && vehicle <= 611)
                return vehicle;
        }
    catch (const std::exception &e)
        {
            puts (str.c_str ());
        }

    return VEHICLES_ALL;
}

/*******************************************************/
void
ScriptVehicleConfig::ReadPattern (std::shared_ptr<cpptoml::table> pattern)
{
    VehiclePattern _pattern;

    // Vehicle Pattern
    if (auto val = pattern->get_as<int64_t> ("vehicle"))
        _pattern.vehicle = val.value_or (VEHICLES_ALL);
    if (auto val = pattern->get_as<std::string> ("vehicle"))
        _pattern.vehicle = StrToVehicleType (val.value_or ("all"));

    _pattern.seat_check = pattern->get_as<bool> ("seat_check").value_or (true);
    _pattern.thread     = pattern->get_as<std::string> ("thread").value_or ("");

    auto coords = pattern->get_array_of<int64_t> ("coords");
    auto move   = pattern->get_array_of<int64_t> ("move");

    // Coords and Move
    if (coords && coords->size () == 3)
        {
            _pattern.coords[0] = (*coords)[0];
            _pattern.coords[1] = (*coords)[1];
            _pattern.coords[2] = (*coords)[2];
        }
    if (move && move->size () == 4)
        {
            _pattern.move[0] = (*move)[0];
            _pattern.move[1] = (*move)[1];
            _pattern.move[2] = (*move)[2];
            _pattern.move[3] = (*move)[3];
        }

    ReadTable (pattern, "allowed", _pattern.allowed);
    ReadTable (pattern, "denied", _pattern.denied);

    this->patterns.push_back (_pattern);
}

/*******************************************************/
void
ScriptVehicleConfig::Read (std::shared_ptr<cpptoml::table> table)
{
    if (!table)
        return;

    BaseConfig::Read (table);

    CONFIG (table, skipChecks, "SkipChecks", bool);

    // Read Patterns
    if (auto patterns = table->get_table_array ("Patterns"))
        {
            for (const auto &pattern : *patterns)
                {
                    ReadPattern (pattern);
                }
        }
}

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
    auto stream = std::istringstream (
        std::string ((char *) config_toml, sizeof (config_toml) - 1));

    cpptoml::parser p{stream};
    return p.parse ();
}

/*******************************************************/
void
ConfigManager::Initialise (const std::string &file)
{
    std::shared_ptr<cpptoml::table> config;
    try
        {
            config = cpptoml::parse_file (GetRainbomizerFileName (file));
        }
    catch (const std::exception &e)
        {
            Logger::GetLogger ()->LogMessage (e.what ());
            config = ParseDefaultConfig ();

            if (!DoesFileExist (file))
                WriteDefaultConfig (file);
        }

    mConfigs.general.Read (config);
    mConfigs.traffic.Read (config->get_table ("TrafficRandomizer"));
    mConfigs.colours.Read (config->get_table ("ColourRandomizer"));
    mConfigs.policeHeli.Read (config->get_table ("PoliceHeliRandomizer"));
    mConfigs.cheat.Read (config->get_table ("CheatRandomizer"));
    mConfigs.handling.Read (config->get_table ("HandlingRandomizer"));
    mConfigs.weapon.Read (config->get_table ("WeaponRandomizer"));
    mConfigs.parkedCar.Read (config->get_table ("ParkedCarRandomizer"));
    mConfigs.pickups.Read (config->get_table ("PickupsRandomizer"));
    mConfigs.licensePlate.Read (config->get_table ("LicensePlateRandomizer"));
    mConfigs.sounds.Read (config->get_table ("SoundsRandomizer"));
    mConfigs.scriptVehicle.Read (config->get_table ("ScriptVehicleRandomizer"));
    mConfigs.missions.Read (config->get_table ("MissionRandomizer"));
    mConfigs.dyom.Read (config->get_table ("DyomRandomizer"));
    mConfigs.objects.Read (config->get_table ("ObjectRandomizer"));
    mConfigs.blips.Read (config->get_table ("BlipsRandomizer"));
    mConfigs.particles.Read (config->get_table ("ParticleRandomizer"));
    mConfigs.cutscenes.Read (config->get_table ("CutsceneRandomizer"));
    mConfigs.weaponStats.Read (config->get_table ("WeaponStatsRandomizer"));
    mConfigs.clothes.Read (config->get_table ("ClothesRandomizer"));
    mConfigs.wanted.Read (config->get_table ("WantedLevelRandomizer"));
    mConfigs.respawnPoint.Read (config->get_table ("RespawnPointRandomizer"));
    mConfigs.lang.Read (config->get_table ("LanguageRandomizer"));
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
