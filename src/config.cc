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

#include "config.hh"
#include "cpptoml/cpptoml.h"
#include <cstdlib>

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
void
ConfigManager::Initialise (std::string file)
{
    auto config = cpptoml::parse_file ("config.toml");

    mConfigs.general.Read(config);
	mConfigs.traffic.Read(config->get_table("TrafficRandomizer"));
	mConfigs.carcol.Read(config->get_table("CarColRandomizer"));
    mConfigs.policeHeli.Read(config->get_table("PoliceHeliRandomizer"));
    mConfigs.cheat.Read(config->get_table("CheatRandomizer"));
    mConfigs.handling.Read(config->get_table("HandlingRandomizer"));
    mConfigs.weapon.Read(config->get_table("WeaponRandomizer"));
    mConfigs.parkedCar.Read(config->get_table("ParkedCarRandomizer"));
    mConfigs.licensePlate.Read(config->get_table("LicensePlateRandomizer"));
    mConfigs.sounds.Read(config->get_table("SoundsRandomizer"));
    mConfigs.scriptVehicle.Read(config->get_table("ScriptVehicleRandomizer"));
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
const Configs&
ConfigManager::GetConfigs()
{
	return this->mConfigs;
}
