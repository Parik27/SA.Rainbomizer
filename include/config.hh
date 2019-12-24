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

namespace cpptoml {
class table;
} // namespace cpptoml

/*******************************************************/
/* Related to Weapon Pattern configuration */
/*******************************************************/
const int WEAPON_SLOT = 101;
const int WEAPON_ANY  = -1;

struct WeaponPattern
{
    std::string          thread  = "";
    int                  ped     = -1;
    int                  weapon  = -1;
    std::vector<int64_t> allowed = {};
    std::vector<int64_t> denied  = {};
};

/*******************************************************/
/* Related to Vehicle Pattern configuration */
/*******************************************************/
enum eVehicleTypes
{
    VEHICLES_ALL  = -1,
    VEHICLES_CARS = 12,
    VEHICLES_BIKES,
    VEHICLES_HELIS,
    VEHICLES_BOATS,
    VEHICLES_PLANES,
    VEHICLES_BMX    = 10,
    VEHICLES_TRAINS = 6
};

struct VehiclePattern
{
    std::string          thread     = "";
    int                  vehicle    = -1;
    std::vector<int16_t> allowed    = {};
    std::vector<int16_t> denied     = {};
    int                  coords[3]  = {-1, -1, -1};
    int                  move[4]    = {0, 0, 0, 0};
    bool                 seat_check = true;
};

/*******************************************************/
struct BaseConfig
{
    bool enabled = true;

    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct GeneralConfig : public BaseConfig
{
    int  seed      = -1;
    bool unprotect = false;
    int  save_slot = 8;

    void Read (std::shared_ptr<cpptoml::table> table);
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
    bool                 fades      = false;

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
    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct WeaponConfig : public BaseConfig
{
    bool                       enabled             = true;
    bool                       playerRandomization = true;
    bool                       skipChecks          = false;
    std::vector<WeaponPattern> patterns            = {};

    void Read (std::shared_ptr<cpptoml::table> table);
    void ReadPattern (std::shared_ptr<cpptoml::table> pattern);
    void ReadTable (std::shared_ptr<cpptoml::table> pattern,
                    const std::string &key, std::vector<int64_t> &out);
};

/*******************************************************/
struct PickupsConfig : public BaseConfig
{
    bool sameType = false;

    void Read (std::shared_ptr<cpptoml::table> table);
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

    bool        matchSubtitles    = true;
    std::string audioEventsFile   = "data/AudioEvents.txt";
    bool        forceAudioEnabled = false;
    int         forceAudioID      = -1;

    void Read (std::shared_ptr<cpptoml::table> table);
};

/*******************************************************/
struct ScriptVehicleConfig : public BaseConfig
{
    bool                        skipChecks = false;
    std::vector<VehiclePattern> patterns   = {};

    int StrToVehicleType (const std::string &str);

    void Read (std::shared_ptr<cpptoml::table> table);
    void ReadPattern (std::shared_ptr<cpptoml::table> pattern);
    void ReadTable (std::shared_ptr<cpptoml::table> pattern,
                    const std::string &key, std::vector<int16_t> &v);
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
    PickupsConfig       pickups;
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

    void WriteDefaultConfig (const std::string &file);
    std::shared_ptr<cpptoml::table> ParseDefaultConfig ();

public:
    /// Returns the static instance for ConfigManager.
    static ConfigManager *GetInstance ();

    /// Initialises
    void Initialise (const std::string &file);

    /// Gets the config
    const Configs &GetConfigs ();
};
