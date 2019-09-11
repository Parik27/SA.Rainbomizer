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

#pragma once

#include <string>
#include <memory>
#include <vector>

namespace cpptoml
{
	class table;
}

/*******************************************************/
struct BaseConfig
{
    bool enabled = true;

	void Read(std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct GeneralConfig : public BaseConfig
{
    int seed = -1;
	void Read(std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct TrafficConfig : public BaseConfig
{

    bool forcedVehicleEnabled = false;
    int  forcedVehicleID      = 500;

    bool enableTrains    = true;
    bool enableBoats     = true;
    bool enableAircrafts = true;
    bool enableCars      = true;
    bool enableBikes     = true;
    bool enableTrailers  = true;

    int defaultModel = 0;

    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct CarColConfig : public BaseConfig
{

    std::vector<int64_t> exceptions = {0, 1};

    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct PoliceHeliConfig : public BaseConfig
{
};

/*******************************************************/
struct CheatConfig : public BaseConfig
{

    bool enableEasterEgg = true;
    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct HandlingConfig : public BaseConfig
{
    bool enabled = false;
};

/*******************************************************/
struct WeaponConfig : public BaseConfig
{
    bool enabled = false;
};

/*******************************************************/
struct ParkedCarConfig : public BaseConfig
{

    bool randomizeFixedSpawns  = true;
    bool randomizeRandomSpawns = true;

    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct LicensePlateConfig : public BaseConfig
{
};

/*******************************************************/
struct SoundsConfig : public BaseConfig
{

    bool        matchSubtitles  = true;
    std::string audioEventsFile = "data/AudioEvents.txt";

    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct ScriptVehicleConfig : public BaseConfig
{
    bool skipChecks = false;
    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct Configs
{
    GeneralConfig       general;
    TrafficConfig       traffic;
    CarColConfig        carcol;
    PoliceHeliConfig    policeHeli;
    CheatConfig         cheat;
    HandlingConfig      handling;
    WeaponConfig        weapon;
    ParkedCarConfig     parkedCar;
    LicensePlateConfig  licensePlate;
    SoundsConfig        sounds;
    ScriptVehicleConfig scriptVehicle;
};

/*******************************************************/
class ConfigManager
{
    static ConfigManager *mInstance;

    Configs mConfigs;

    ConfigManager (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for ConfigManager.
    static ConfigManager *GetInstance ();

    /// Initialises
    void Initialise (std::string file);

    /// Gets the config
    const Configs &GetConfigs ();
};
