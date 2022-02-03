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

#include "scm.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "injector/injector.hpp"
#include "util/loader.hh"
#include "config.hh"
#include <cmath>
#include <map>
#include <unordered_map>
#include "scm_patterns.hh"
#include "util/scrpt.hh"
#include "injector/calling.hpp"
#include "util/text.hh"
#include <chrono>
#include "traffic.hh"
#include <array>

ScriptVehicleRandomizer *ScriptVehicleRandomizer::mInstance = nullptr;

const int MODEL_DUMPER   = 406;
const int MODEL_FIRETRU  = 407;
const int MODEL_PONY     = 413;
const int MODEL_AMBULAN  = 416;
const int MODEL_TAXI     = 420;
const int MODEL_FIRELA   = 0x220;
const int MODEL_PCJ      = 461;
const int MODEL_FAGGIO   = 462;
const int MODEL_FREEWAY  = 463;
const int MODEL_SANCHZ   = 468;
const int MODEL_QUAD     = 471;
const int MODEL_WALTON   = 478;
const int MODEL_BMX      = 481;
const int MODEL_DOZER    = 486;
const int MODEL_MAVERICK = 487;
const int MODEL_GREENWOO = 492;
const int MODEL_BIKE     = 509;
const int MODEL_MTBIKE   = 510;
const int MODEL_HYDRA    = 520;
const int MODEL_NRG      = 522;
const int MODEL_COPBIKE  = 523;
const int MODEL_CEMENT   = 524;
const int MODEL_FORKLIFT = 530;
const int MODEL_COMBINE  = 532;
const int MODEL_FELTZER  = 533;
const int MODEL_VORTEX   = 539;
const int MODEL_ANDROM   = 592;
const int MODEL_BOXBURG  = 609;

const int waterCarsCheatActive  = 0x969130 + 34;
const int flyingCarsCheatActive = 0x969130 + 48;

static const struct ScriptVehicleRandomizer::RCHeliMagnet emptyTemplate;
static ScriptVehicleRandomizer::RCHeliMagnet              ryder2;
static ScriptVehicleRandomizer::RCHeliMagnet              quarry;

static bool ignoreNextHook = false; // For Flying School times
static bool currentTrailerAttached
    = false; // For attaching any vehicle in Trucking missions
static bool wasFlyingCarsOn = false; // Stores state of flying cars before
                                     // starting a mission that forces them

// Store original train type + randomized train type for random trains
static int              lastTrainOldType = -1;
static int              lastTrainNewType = -1;
static std::vector<int> snailTrailTrain;

static ScriptVehicleRandomizer::BoatSchoolTimes boatSchool;

// Key is traintype ID, value is number of carriages for that train type
// Used when randomizing trainType to get number of carriages of new train
// If new train has less carriages, need to ensure that now nonexistent
// carriages are ignored
std::unordered_map<int, int> trainTypes
    = {{0, 5}, {1, 3}, {2, 3},  {3, 4},  {4, 3},  {5, 4},  {6, 3},  {7, 3},
       {8, 2}, {9, 1}, {10, 2}, {11, 3}, {12, 5}, {13, 6}, {14, 1}, {15, 1}};

static float timerStartTime = -1.0f;
static float timerCurrent   = -1.0f;

template <typename T>
T &
GetGlobalVar (uint32_t index)
{
    return *reinterpret_cast<T *> (&ScriptSpace[index]);
}

template <typename T>
T &
GetScriptParam (uint32_t index)
{
    return *reinterpret_cast<T *> (&ScriptParams[index]);
}

/*******************************************************/
void
SlowDownAndromedaInStoaway (uint8_t *vehicle, float speed)
{
    // uint16_t modelIndex = *reinterpret_cast<uint16_t *> (vehicle + 0x22);

    if (speed < 1.1 && speed > 0.9
        && ScriptVehicleRandomizer::GetInstance ()->mLastThread == "desert9")
        speed = 0.7;

    if (speed < 0.92
        && ScriptVehicleRandomizer::GetInstance ()->mLastThread == "manson5"
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_VORTEX)
        speed = 0.82;

    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "finalec"
        && ScriptVehicleRandomizer::mEOTL3Slow)
        speed = 0.9;

    CVehicleRecording::SetPlaybackSpeed (vehicle, speed);
}

/*******************************************************/
void
RevertVehFixes (int index)
{
    if (index == MODEL_FIRELA)
        ScriptVehicleRandomizer::GetInstance ()->ApplyEOTLFixes (MODEL_FIRELA);
    else if (index == MODEL_SANCHZ)
        ScriptVehicleRandomizer::GetInstance ()->ApplyCarCheckFix (-1);

    CStreaming::SetMissionDoesntRequireModel (index);
}

///*******************************************************/
void
EnableFlyingCars (char flag)
{
    if (!ScriptVehicleRandomizer::GetInstance ()->GetIfPlayerInFlyingCar ()
        && flag)
        {
            ScriptVehicleRandomizer::GetInstance ()->SetPlayerAsInFlyingCar (
                true);
            wasFlyingCarsOn
                = injector::ReadMemory<bool> (flyingCarsCheatActive);
        }
}

///*******************************************************/
void
DisableFlyingCars ()
{
    if (ScriptVehicleRandomizer::GetInstance ()->GetIfPlayerInFlyingCar ())
        {
            ScriptVehicleRandomizer::GetInstance ()->SetPlayerAsInFlyingCar (
                false);
            injector::WriteMemory (flyingCarsCheatActive, wasFlyingCarsOn);
        }
}

///*******************************************************/
void
SetFlyingCar (bool flag)
{
    EnableFlyingCars (flag);
    if (flag)
        injector::WriteMemory (flyingCarsCheatActive, 1);
    else
        DisableFlyingCars ();
}

/*******************************************************/
void __fastcall ResetFlyingCar (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("desert6") || scr->CheckName ("casino9")
        || scr->CheckName ("casin10") || scr->CheckName ("heist2")
        || (scr->CheckName ("cprace")
            && (ScriptSpace[352] >= 19 && ScriptSpace[352] <= 24)))
        DisableFlyingCars ();
    else if (scr->CheckName ("boat"))
        injector::WriteMemory (waterCarsCheatActive, 0);
}

/*******************************************************/
void __fastcall FlyingCarsForVB (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("mansio2"))
        {
            if (GetScriptParam<float> (0) < 0.5f)
                SetFlyingCar (true);
            else if (GetScriptParam<float> (0) > 0.5f)
                SetFlyingCar (false);
        }
}

/*******************************************************/
void __fastcall FlyingCarsForFlightSchool (CRunningScript *scr, void *edx,
                                           short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("desert5"))
        {
            if (ScriptParams[0] == 0)
                SetFlyingCar (true);
            else if (ScriptParams[0] == 1)
                SetFlyingCar (false);
        }
}

/*******************************************************/
void __fastcall WaterCarsForBoatSchool (CRunningScript *scr, void *edx,
                                        short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat"))
        {
            if (ScriptParams[1] == 0)
                injector::WriteMemory (waterCarsCheatActive, 1);
            else if (ScriptParams[1] == 1)
                injector::WriteMemory (waterCarsCheatActive, 0);
        }
}

/*******************************************************/
void __fastcall FixCarChecks (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    CVector playerPos = FindPlayerCoors ();
    if (ScriptParams[1] == MODEL_SANCHZ // For Kickstart
        || (ScriptParams[1] == MODEL_FORKLIFT
            && scr->CheckName ("heist9"))   // For Breaking the Bank
        || ScriptParams[1] == MODEL_DUMPER) // For Explosive Situation
        {
            int newVeh
                = ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ();
            if (newVeh != -1)
                ScriptParams[1] = newVeh;
        }
    else if (ScriptParams[1] == MODEL_CEMENT
             && FindPlayerVehicle ()) // For Deconstruction
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (ScriptParams[1] == MODEL_HYDRA
             && FindPlayerVehicle ()) // For Vertical Bird
        {
            int playerVeh = FindPlayerVehicle ()->m_nModelIndex;
            if (CModelInfo::IsPlaneModel (playerVeh)
                || CModelInfo::IsHeliModel (playerVeh)
                || CModelInfo::IsCarModel (playerVeh)
                || CModelInfo::IsQuadBikeModel (playerVeh)
                || CModelInfo::IsMonsterTruckModel (playerVeh))
                {
                    ScriptParams[1] = playerVeh;
                }
        }
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_BMX
             && playerPos.x >= 1356.45f && playerPos.x <= 1362.45f
             && playerPos.y >= -1758.0f && playerPos.y <= -1752.0f
             && playerPos.z >= 9.85f && playerPos.z <= 15.85f)
        {
            ScriptVehicleRandomizer::GetInstance ()->mCourierMissionType = 0;
            ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
        }
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_FREEWAY
             && playerPos.x >= -2593.44f && playerPos.x <= -2587.44f
             && playerPos.y >= 70.21f && playerPos.y <= 76.21f
             && playerPos.z >= 0.91f && playerPos.z <= 6.91f)
        {
            ScriptVehicleRandomizer::GetInstance ()->mCourierMissionType = 1;
            ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
        }
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_FAGGIO
             && playerPos.x >= 1884.79f && playerPos.x <= 1890.79f
             && playerPos.y >= 2084.39f && playerPos.y <= 2090.39f
             && playerPos.z >= 7.05f && playerPos.z <= 13.05f)
        {
            ScriptVehicleRandomizer::GetInstance ()->mCourierMissionType = 2;
            ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
        }
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_BMX
             && playerPos.x >= 1942.09f && playerPos.x <= 1950.09f
             && playerPos.y >= -1384.72f && playerPos.y <= -1376.72f
             && playerPos.z >= 15.0f && playerPos.z <= 21.0f)
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_MTBIKE)
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_NRG)
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (scr->CheckName ("firetru") && FindPlayerVehicle ()
             && ScriptParams[1] == MODEL_FIRETRU)
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (scr->CheckName ("ambulan") && FindPlayerVehicle ()
             && ScriptParams[1] == MODEL_AMBULAN)
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
}

/*******************************************************/
void __fastcall FixEOTLPosition (CMatrix *matrix, void *edx, CMatrix *attach,
                                 char link)
{
    matrix->Attach (attach, link);
    if (ScriptVehicleRandomizer::GetInstance ()->PosFixEnabled ())
        matrix->pos.z += 2.5;
}

/*******************************************************/
void
ScriptVehicleRandomizer::ApplyEOTLFixes (int newFiretruck)
{
    injector::WriteMemory<uint16_t> (0x5DFE3D + 4, newFiretruck);
    if (newFiretruck == MODEL_FIRELA)
        {
            injector::WriteMemory<uint16_t> (0x5DFE4D + 2, 0x648 + 21 * 4);
            mPosFixEnabled = false;
        }
    else
        {
            int model_nodes = 0;

            if (CModelInfo::IsBikeModel (newFiretruck)
                || CModelInfo::IsBmxModel (newFiretruck))
                model_nodes = 0x5A0;
            else if (CModelInfo::IsTrainModel (newFiretruck))
                model_nodes = 0x668;
            else if (CModelInfo::IsBoatModel (newFiretruck))
                model_nodes = 0x5B0;
            else
                model_nodes = 0x648;

            injector::WriteMemory<uint16_t> (0x5DFE4D + 2, model_nodes + 1 * 4);
            mPosFixEnabled = true;
        }
}

/*******************************************************/
void
ApplyFixesBasedOnModel (int model, int newModel, float x, float y, float z)
{
    std::string_view thread
        = ScriptVehicleRandomizer::GetInstance ()->mLastThread;

    if (model == MODEL_FIRELA)
        ScriptVehicleRandomizer::GetInstance ()->ApplyEOTLFixes (newModel);
    else if ((model == MODEL_SANCHZ && thread == "kicksta")
             || (model == MODEL_FORKLIFT && thread == "heist9")
             || (model == MODEL_DUMPER && thread == "casino2")
             || (model == MODEL_FORKLIFT && thread == "ryder2")
             || (model == MODEL_WALTON && thread == "doc2")
             || (model == MODEL_ANDROM && thread == "desert9")
             || (model == MODEL_GREENWOO && thread == "drugs4")
             || (model == MODEL_VORTEX && thread == "boat")
             || (model == MODEL_BOXBURG && thread == "guns1")
             || (model == MODEL_PCJ && thread == "toreno2")
             || (model == MODEL_COMBINE && thread == "truth1")
             || (model == MODEL_PONY && thread == "music1")
             || (model == MODEL_FELTZER && thread == "finalec")
             || (model == 553 && thread == "heist2")
             || (model == 417 && thread == "des3")
             || (model == 501 && thread == "zero4" && int (x) > -1000)
             || (model == MODEL_VORTEX && thread == "manson5"
                 && int (y) == -1116)
             || (model == MODEL_DOZER && thread == "quarry"
                 && (ScriptSpace[8171] == 1 || ScriptSpace[8171] == 5)))
        ScriptVehicleRandomizer::GetInstance ()->ApplyCarCheckFix (newModel);
}

/*******************************************************/
void
GetModelExceptions (int model, int &newModel)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "quarry"
        && (ScriptSpace[8171] == 1 || ScriptSpace[8171] == 5)
        && model == MODEL_DOZER)
        {
            newModel = random (0, 2);
            if (newModel == 0)
                newModel = MODEL_DOZER;
            else if (newModel == 1)
                newModel = 501;
            else
                newModel = 465;
        }
    else if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "quarry"
             && ScriptSpace[8171] == 4 && model == MODEL_DUMPER)
        newModel = MODEL_DUMPER;
    else if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "dskool")
        {
            if (ScriptSpace[247] == 15 && model == 429)
                {
                    static constexpr std::array alleyOopCars
                        = {429, 541, 480, 494, 506, 451};
                    newModel = GetRandomElement (alleyOopCars);
                }
            else if (ScriptSpace[247] == 11)
                {
                    static constexpr std::array wheelieWeaveCars
                        = {429, 541, 480, 494, 506, 451, 477, 603, 596,
                           597, 598, 599, 576, 504, 505, 490, 528, 542,
                           533, 587, 559, 402, 411, 412, 415};
                    newModel = GetRandomElement (wheelieWeaveCars);
                }
        }
}

/*******************************************************/
void
ApplyFixesBasedOnMission ()
{
    currentTrailerAttached = false;
    timerStartTime         = -1.0f;

    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "sweet6")
        GetGlobalVar<int> (1920)++;

    int &g_Timer_Value = ScriptSpace[253];
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "dskool"
        && ScriptVehicleRandomizer::m_Config.MoreSchoolTestTime)
        {
            int currentTest = ScriptSpace[247];
            if (currentTest == 1)
                g_Timer_Value = 16000; // Timer value
            else if (currentTest == 2)
                g_Timer_Value = 26000;
            else if (currentTest == 3 || currentTest == 4)
                g_Timer_Value = 20000;
            else if (currentTest == 5)
                g_Timer_Value = 15000;
            else if (currentTest == 8 || currentTest == 9)
                g_Timer_Value = 31000;
            else if (currentTest == 10)
                g_Timer_Value = 12000;
            else if (currentTest == 12 || currentTest == 13)
                g_Timer_Value = 14000;
        }

    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "bskool"
        && ScriptVehicleRandomizer::m_Config.MoreSchoolTestTime)
        {
            int currentTest = ScriptSpace[247];
            if (currentTest == 1)
                g_Timer_Value = 20000; // Timer value
            else if (currentTest == 2)
                g_Timer_Value = 30000;
        }

    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "desert5")
        ignoreNextHook = false;
}

/*******************************************************/
void
ApplySpecificPosChanges (int model, int &newModel, float &x, float &y, float &z)
{
    if ((ScriptVehicleRandomizer::GetInstance ()->mLastThread == "smoke3"
         || ScriptVehicleRandomizer::GetInstance ()->mLastThread == "drugs1")
        && model == 466 && CModelInfo::IsCarModel (newModel))
        {
            x += 3.0f;
        }
    else if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "manson5"
             && int (y) == -1116 && model == 539)
        {
            if (newModel == 460)
                {
                    x += 40.0f;
                    z -= 6.0f;
                }
            else if (CModelInfo::IsPlaneModel (newModel) && newModel != 539
                     && newModel != 520)
                {
                    x -= 30.1f;
                    y -= 67.8f;
                    z -= 3.0f;
                }
            else if (CModelInfo::IsHeliModel (newModel))
                {
                    y -= 17.8f;
                }
        }
}

/*******************************************************/
void *
RandomizeCarForScript (int model, float x, float y, float z, bool createdBy)
{
    int newModel
        = ScriptVehicleRandomizer::GetInstance ()->ProcessVehicleChange (model,
                                                                         x, y,
                                                                         z);
    if (ScriptVehicleRandomizer::m_Config.ForcedVehicle > 350)
        newModel = ScriptVehicleRandomizer::m_Config.ForcedVehicle;

    Logger::GetLogger ()->LogMessage ("Vehicle Spawned: "
                                      + std::to_string (newModel));

    ApplyFixesBasedOnModel (model, newModel, x, y, z);

    if (!ScriptVehicleRandomizer::m_Config.SkipChecks
        && ScriptVehicleRandomizer::m_Config.ForcedVehicle < 350)
        GetModelExceptions (model, newModel);

    ApplyFixesBasedOnMission ();
    ApplySpecificPosChanges (model, newModel, x, y, z);

    // Load the new vehicle. Fallback to the original if needed
    if (StreamingManager::AttemptToLoadVehicle (newModel) == ERR_FAILED)
    {
        Logger::GetLogger ()->LogMessage (
                "Failed to load new vehicle, using fallback");
        if (ScriptVehicleRandomizer::mFallbackVeh > 0)
        {
            model = ScriptVehicleRandomizer::mFallbackVeh;
            ScriptVehicleRandomizer::mFallbackVeh = -1;
        }
        newModel = model;
    }

    uint8_t *vehicle = (uint8_t *) CCarCtrl::CreateCarForScript (newModel, x, y,
                                                                 z, createdBy);

    if (CModelInfo::IsPoliceModel (newModel))
        {
            uint32_t *door_lock
                = reinterpret_cast<uint32_t *> (vehicle + 0x4F8);
            *door_lock = 1;
        }

    return vehicle;
}

/*******************************************************/
void
RandomizeRCVehicleForScript (float x, float y, float z, float angle,
                             short model)
{
    int newModel
        = ScriptVehicleRandomizer::GetInstance ()->ProcessVehicleChange (model,
                                                                         x, y,
                                                                         z);

    ApplyFixesBasedOnModel (model, newModel, x, y, z);
    ApplyFixesBasedOnMission ();

    if (StreamingManager::AttemptToLoadVehicle (newModel) == ERR_FAILED)
        newModel = model;

    GivePlayerRemoteControlledCar (x, y, z, angle, newModel);
}

/*******************************************************/
bool
CompareCoordinates (int x1, int y1, int z1, int x2, int y2, int z2)
{
    return (x1 == x2 && y1 == y2 && z1 == z2)
           || ((x1 == -1 && y1 == -1 && z1 == -1)
               || (x2 == -1 && y2 == -1 && z2 == -1));
}

/*******************************************************/
int16_t __fastcall UpdateLastThread (CRunningScript *script, void *edx,
                                     int16_t count)
{
    script->CollectParameters (count);
    int currentMissionId
        = ScriptVehicleRandomizer::GetInstance ()->mCurrentMissionRunning;
    if (((currentMissionId >= 121 && currentMissionId <= 123)
         || (currentMissionId == 125) || (currentMissionId == 127)
         || (currentMissionId >= 131 && currentMissionId <= 133))
        && std::string (script->m_szName) == "noname")
        return 0;

    ScriptVehicleRandomizer::GetInstance ()->UpdateLastThread (
        script->m_szName);
    return 0;
}

/*******************************************************/
int
ScriptVehicleRandomizer::ProcessVehicleChange (int id, float &x, float &y,
                                               float &z)
{
    if (!m_Config.SkipChecks)
        {
            Vector3 pos = {x, y, z};
            for (auto &pattern : mPatterns)
                {
                    if (pattern.MatchVehicle (id, mLastThread, pos))
                        {
                            int newVehID = pattern.GetRandom (pos);
                            x            = pos.x;
                            y            = pos.y;
                            z            = pos.z;
                            return newVehID;
                        }
                }
        }
    if (find (recognisedScripts.begin (), recognisedScripts.end (), mLastThread)
            == recognisedScripts.end ()
        || m_Config.GenericPatterns)
        {
            std::vector<uint16_t> validVehicles;
            std::vector<uint16_t> validSeats;
            eVehicleClass         vehicleType = GetVehicleType (id);

            if (id == 530)
                validSeats.push_back (530);
            else if (id == 539)
                validSeats.push_back (539);
            else
                {
                    switch (vehicleType)
                        {
                        case VEHICLE_AUTOMOBILE:
                        case VEHICLE_MTRUCK:
                        case VEHICLE_BIKE:
                        case VEHICLE_BMX:
                        case VEHICLE_QUAD:
                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::CAR)].begin (),
                                carTypes[int (CarType::CAR)].end ());

                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::BIKE)].begin (),
                                carTypes[int (CarType::BIKE)].end ());

                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::HELI)].begin (),
                                carTypes[int (CarType::HELI)].end ());
                            break;

                        case VEHICLE_PLANE:
                        case VEHICLE_FPLANE:
                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::PLANE)].begin (),
                                carTypes[int (CarType::PLANE)].end ());

                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::HELI)].begin (),
                                carTypes[int (CarType::HELI)].end ());
                            break;

                        case VEHICLE_HELI:
                        case VEHICLE_FHELI:
                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::HELI)].begin (),
                                carTypes[int (CarType::HELI)].end ());
                            break;

                        case VEHICLE_BOAT:
                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::BOAT)].begin (),
                                carTypes[int (CarType::BOAT)].end ());

                            validVehicles.push_back (460);
                            validVehicles.push_back (539);
                            validVehicles.push_back (447);
                            validVehicles.push_back (417);
                            break;

                        case VEHICLE_TRAIN:
                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::TRAIN)].begin (),
                                carTypes[int (CarType::TRAIN)].end ());
                            break;

                        case VEHICLE_TRAILER:
                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::CAR)].begin (),
                                carTypes[int (CarType::CAR)].end ());

                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::PLANE)].begin (),
                                carTypes[int (CarType::PLANE)].end ());

                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::HELI)].begin (),
                                carTypes[int (CarType::HELI)].end ());

                            validVehicles.insert (
                                validVehicles.end (),
                                carTypes[int (CarType::TRAILER)].begin (),
                                carTypes[int (CarType::TRAILER)].end ());
                            break;
                        }
                    for (auto vehicle : validVehicles)
                        {
                            if (mSeatsCache[id - 400]
                                <= mSeatsCache[vehicle - 400])
                                validSeats.push_back (vehicle);
                        }
                }

            if (validSeats.size () > 0)
                return GetRandomElement (validSeats);

            return id;
        }
    return random (400, 611);
}

/*******************************************************/
bool
CheckForCAutomobile (uint8_t *vehicle)
{
    uint16_t modelIndex = *reinterpret_cast<uint16_t *> (vehicle + 0x22);
    if (CModelInfo::IsBikeModel (modelIndex)
        || CModelInfo::IsBmxModel (modelIndex)
        || CModelInfo::IsBoatModel (modelIndex)
        || CModelInfo::IsTrainModel (modelIndex))
        return false;

    return true;
}

/*******************************************************/
bool
CheckForCarNode (uint8_t *vehicle, int node)
{
    int *carNodes = reinterpret_cast<int *> (vehicle + 0x648);
    if (!carNodes[node])
        return false;

    return true;
}

/* Was thinking to replace these with a macro, couldn't come up with one
 * :thinking: */

/*******************************************************/
char __fastcall FixGearUp (CVehicle *vehicle)
{
    if (CheckForCarNode ((uint8_t *) vehicle, 21)
        || CheckForCarNode ((uint8_t *) vehicle, 22))
        vehicle->SetGearUp ();

    return 2;
}

/*******************************************************/
void *__fastcall PopDoorFix (uint8_t *vehicle, void *edx, int a2, int a3,
                             char a4)
{
    if (!CheckForCAutomobile (vehicle) || !CheckForCarNode (vehicle, a2))
        return nullptr;

    return CallMethodAndReturn<void *, 0x6ADEF0> (vehicle, a2, a3, a4);
}

/*******************************************************/
void *__fastcall PopPanelFix (uint8_t *vehicle, void *edx, int a2, char a3,
                              char a4)
{
    if (!CheckForCAutomobile (vehicle) || !CheckForCarNode (vehicle, a2))
        return nullptr;

    return CallMethodAndReturn<void *, 0x6ADF80> (vehicle, a2, a3, a4);
}

/*******************************************************/
void *__fastcall PopBootFix (uint8_t *vehicle, void *edx)
{
    if (!CheckForCAutomobile (vehicle) || !CheckForCarNode (vehicle, 0x11))
        return nullptr;

    return CallMethodAndReturn<void *, 0x6AF910> (vehicle);
}

/*******************************************************/
int __fastcall VehicleUpdateFix (uint8_t *vehicle, void *edx, int model)
{
    if (!CheckForCAutomobile (vehicle)
            || (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "music1"
                   && CModelInfo::IsHeliModel (
                       ScriptVehicleRandomizer::GetInstance ()
                           ->GetNewCarForCheck ())))
        return model;

    return CallMethodAndReturn<int, 0x6E3290> (vehicle, model);
}

/*******************************************************/
void *__fastcall CloseBootFix (uint8_t *vehicle, void *edx, int a2, int a3,
                               char a4)
{
    if (!CheckForCAutomobile (vehicle) || !CheckForCarNode (vehicle, a2))
        return nullptr;

    return CallMethodAndReturn<void *, 0x6AFA20> (vehicle, a2, a3, a4);
}

/*******************************************************/
void *__fastcall FixDoorFix (uint8_t *vehicle, void *edx, int a2, int a3)
{
    if (!CheckForCAutomobile (vehicle) || !CheckForCarNode (vehicle, a2))
        return nullptr;

    return CallMethodAndReturn<void *, 0x6A35A0> (vehicle, a2, a3);
}

/*******************************************************/
void *__fastcall FixPanelFix (uint8_t *vehicle, void *edx, int a2, char a3)
{
    if (!CheckForCAutomobile (vehicle) || !CheckForCarNode (vehicle, a2))
        return nullptr;

    return CallMethodAndReturn<void *, 0x6A3670> (vehicle, a2, a3);
}

/*******************************************************/
void __fastcall FixGTAMadman (CRunningScript *scr, void *edx, int opcode)
{
    if (scr->CheckName ("heist4"))
        {
            if (opcode == 1534)
                {
                    scr->CollectParameters (9);
                    CVector playerPos = FindPlayerCoors ();
                    CVector truckPos
                        = *reinterpret_cast<CVector *> (ScriptParams + 1);

                    float dist   = Dist (playerPos, truckPos);
                    bool  result = dist < 10;

                    scr->UpdateCompareFlag (result);
                    return;
                }
        }
    scr->ProcessCommands1526to1537 (opcode);
}

/*******************************************************/
void __fastcall ProcessModifiedSchoolTimes (CRunningScript *scr, void *edx,
                                            short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("dskool"))
        {
            ScriptVehicleRandomizer::GetInstance ()->SaveTestTime (
                ScriptSpace[253]);
            if (ScriptSpace[247] == 6 || ScriptSpace[247] == 7)
                ScriptSpace[253] -= 20000;
            else if (ScriptSpace[247] == 16)
                ScriptSpace[253] -= 120000;
        }
}

/*******************************************************/
void __fastcall ProcessModifiedSchoolTimesFlying (CRunningScript *scr,
                                                  void *edx, short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[1] == 0 && !ignoreNextHook)
        {
            if (scr->CheckName ("desert5"))
                {
                    ScriptVehicleRandomizer::GetInstance ()->SaveTestTime (
                        ScriptSpace[253]);
                    Scrpt::CallOpcode (0x14F, "stop_timer", GlobalVar (253));
                    Scrpt::CallOpcode (0x396, "pause_timer", 1);
                    ScriptSpace[253] -= 120000;
                    ignoreNextHook = true;
                }
        }
}

/*******************************************************/
void __fastcall ProcessModifiedSchoolTimesBoat (CRunningScript *scr, void *edx,
                                                short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat") && ScriptSpace[8189] != 4)
        {
            int currentTest = ScriptSpace[8189];
            ScriptVehicleRandomizer::GetInstance ()->SaveTestTime (
                ScriptSpace[8197]);
            Scrpt::CallOpcode (0x14F, "stop_timer", GlobalVar (8197));
            Scrpt::CallOpcode (0x396, "pause_timer", 1);
            boatSchool.oldRecord         = -1;
            boatSchool.newRecordAchieved = true;
            if (currentTest == 1)
                {
                    boatSchool.oldRecord = ScriptSpace[1963];
                    ScriptSpace[8197] -= 18000;
                }
            else if (currentTest == 2)
                {
                    boatSchool.oldRecord = ScriptSpace[1964];
                    ScriptSpace[8197] -= 40000;
                }
            else if (currentTest == 3)
                {
                    boatSchool.oldRecord = ScriptSpace[1965];
                    ScriptSpace[8197] -= 60000;
                }
            else if (currentTest == 5)
                {
                    boatSchool.oldRecord = ScriptSpace[1967];
                    ScriptSpace[8197] -= 60000;
                }
        }
    else if (scr->CheckName ("zero1")
             && !ScriptVehicleRandomizer::GetInstance ()->m_Config.OldAirRaid)
        ((float *) ScriptParams)[1] = 45.0f;
}

/*******************************************************/
void __fastcall DisplayCorrectSchoolTime (CRunningScript *scr, void *edx,
                                          short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("dskool")
        && (ScriptSpace[247] == 6 || ScriptSpace[247] == 7
            || ScriptSpace[247] == 16))
        {
            int actualTime = ScriptVehicleRandomizer::GetInstance ()
                                 ->GetOriginalTestTime ();
            int seconds     = actualTime / 1000;
            int miliseconds = actualTime - (seconds * 1000);
            miliseconds /= 10;
            ScriptSpace[301] = seconds;
            ScriptSpace[302] = miliseconds;
        }
}

/*******************************************************/
void __fastcall DisplayCorrectFlyingSchoolTime (CRunningScript *scr, void *edx,
                                                short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("desert5"))
        {
            int actualTime = ScriptVehicleRandomizer::GetInstance ()
                                 ->GetOriginalTestTime ();
            int seconds      = actualTime / 1000;
            ScriptSpace[301] = seconds;
        }
}

/*******************************************************/
void __fastcall DisplayCorrectBoatTimeOnTV (CRunningScript *scr, void *edx,
                                            short count)
{
    scr->CollectParameters (count);

    float &x = GetScriptParam<float> (0);
    float &y = GetScriptParam<float> (1);

    if (scr->CheckName ("boat") && ScriptSpace[8189] != 4)
        {
            if (x >= 321.0f && x <= 323.0f && y >= 30.0f && y <= 32.0f)
                {
                    int currentTest = ScriptSpace[8189];
                    if (currentTest == 1 && ScriptSpace[1963] != 60000)
                        {
                            boatSchool.tempActualTime = ScriptSpace[1963];
                            ScriptSpace[1963] -= 18000;
                        }
                    else if (currentTest == 2 && ScriptSpace[1964] != 80000)
                        {
                            boatSchool.tempActualTime = ScriptSpace[1964];
                            ScriptSpace[1964] -= 40000;
                        }
                    else if (currentTest == 3 && ScriptSpace[1965] != 180000)
                        {
                            boatSchool.tempActualTime = ScriptSpace[1965];
                            ScriptSpace[1965] -= 60000;
                        }
                    else if (currentTest == 5 && ScriptSpace[1967] != 200000)
                        {
                            boatSchool.tempActualTime = ScriptSpace[1967];
                            ScriptSpace[1967] -= 60000;
                        }
                }
            else if ((x >= 322.0f && x <= 324.0f && y >= 109.0f && y <= 111.0f)
                     || (x >= 259.0f && x <= 261.0f && y >= 64.0f
                         && y <= 66.0f))
                {
                    if (!boatSchool.newRecordAchieved)
                        y = -100.0f;
                }
        }
}

/*******************************************************/
void __fastcall DisplayCorrectBoatTimePart1 (CRunningScript *scr, void *edx,
                                             short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat") && ScriptSpace[8189] != 4)
        {
            int currentTest = ScriptSpace[8189];
            if (((float *) ScriptParams)[1] >= 74.0f
                && ((float *) ScriptParams)[1] <= 76.0f
                && boatSchool.tempActualTime != -1)
                {
                    if (currentTest == 1)
                        ScriptSpace[1963] = boatSchool.tempActualTime;
                    else if (currentTest == 2)
                        ScriptSpace[1964] = boatSchool.tempActualTime;
                    else if (currentTest == 3)
                        ScriptSpace[1965] = boatSchool.tempActualTime;
                    else if (currentTest == 5)
                        ScriptSpace[1967] = boatSchool.tempActualTime;
                    boatSchool.tempActualTime = -1;
                    boatSchool.type           = 3;
                }
            if ((((float *) ScriptParams)[1] >= 274.0f
                 && ((float *) ScriptParams)[1] <= 276.0f)
                || (((float *) ScriptParams)[1] >= 54.0f
                    && ((float *) ScriptParams)[1] <= 56.0f))
                {
                    boatSchool.finishTime
                        = (float) ScriptVehicleRandomizer::GetInstance ()
                              ->GetOriginalTestTime ();
                    boatSchool.finishTime /= 1000.0f;
                    boatSchool.type = 0;
                }
            else if (((float *) ScriptParams)[1] >= 294.0f
                     && ((float *) ScriptParams)[1] <= 296.0f)
                boatSchool.type = 1;
            else if ((((float *) ScriptParams)[1] >= 314.0f
                      && ((float *) ScriptParams)[1] <= 316.0f)
                     || (((float *) ScriptParams)[1] >= 64.0f
                         && ((float *) ScriptParams)[1] <= 66.0f))
                {
                    int actualTime = ScriptVehicleRandomizer::GetInstance ()
                                         ->GetOriginalTestTime ();
                    actualTime += boatSchool.damage;
                    if (currentTest == 1 && actualTime < boatSchool.oldRecord)
                        ScriptSpace[1963] = actualTime;
                    else if (currentTest == 1
                             && actualTime >= boatSchool.oldRecord)
                        {
                            boatSchool.newRecordAchieved = false;
                            ScriptSpace[8195]            = 0;
                            ScriptSpace[1963]            = boatSchool.oldRecord;
                        }
                    else if (currentTest == 2
                             && actualTime < boatSchool.oldRecord)
                        ScriptSpace[1964] = actualTime;
                    else if (currentTest == 2
                             && actualTime >= boatSchool.oldRecord)
                        {
                            boatSchool.newRecordAchieved = false;
                            ScriptSpace[8195]            = 0;
                            ScriptSpace[1964]            = boatSchool.oldRecord;
                        }
                    else if (currentTest == 3
                             && actualTime < boatSchool.oldRecord)
                        ScriptSpace[1965] = actualTime;
                    else if (currentTest == 3
                             && actualTime >= boatSchool.oldRecord)
                        {
                            boatSchool.newRecordAchieved = false;
                            ScriptSpace[8195]            = 0;
                            ScriptSpace[1965]            = boatSchool.oldRecord;
                        }
                    else if (currentTest == 5
                             && actualTime < boatSchool.oldRecord)
                        ScriptSpace[1967] = actualTime;
                    else if (currentTest == 5
                             && actualTime >= boatSchool.oldRecord)
                        {
                            boatSchool.newRecordAchieved = false;
                            ScriptSpace[8195]            = 0;
                            ScriptSpace[1967]            = boatSchool.oldRecord;
                        }
                    boatSchool.overallTime = (float) actualTime / 1000.0f;
                    boatSchool.type        = 2;
                }
        }
}

/*******************************************************/
void __fastcall DisplayCorrectBoatTimePart2 (CRunningScript *scr, void *edx,
                                             short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat") && ScriptSpace[8189] != 4)
        {
            int currentTest = ScriptSpace[8189];
            if (ScriptParams[1] == 1)
                {
                    if (currentTest == 1)
                        ((float *) ScriptParams)[0] += 18.0f;
                    else if (currentTest == 2)
                        ((float *) ScriptParams)[0] += 40.0f;
                    else if (currentTest == 3)
                        ((float *) ScriptParams)[0] += 1.0f;
                    else if (currentTest == 5)
                        ((float *) ScriptParams)[0] += 1.0f;
                }
            else if (boatSchool.type != -1)
                {
                    if (boatSchool.type == 0)
                        ((float *) ScriptParams)[0] = boatSchool.finishTime;
                    else if (boatSchool.type == 1)
                        boatSchool.damage
                            = ((float *) ScriptParams)[0] * 1000.0f;
                    else if (boatSchool.type == 2)
                        {
                            ((float *) ScriptParams)[0]
                                = boatSchool.overallTime;
                            boatSchool.damage = 0;
                        }
                    else if (boatSchool.type == 3)
                        {
                            if (currentTest == 1)
                                ((float *) ScriptParams)[0]
                                    = (float) ScriptSpace[1963] / 1000.0f;
                            else if (currentTest == 2)
                                ((float *) ScriptParams)[0]
                                    = (float) ScriptSpace[1964] / 1000.0f;
                            else if (currentTest == 3)
                                ((float *) ScriptParams)[0]
                                    = (float) ScriptSpace[1965] / 1000.0f;
                            else if (currentTest == 5)
                                ((float *) ScriptParams)[0]
                                    = (float) ScriptSpace[1967] / 1000.0f;
                        }
                    boatSchool.type = -1;
                }
        }
}

/*******************************************************/
int __fastcall AlwaysPickUpPackagesTBone (CVehicle *currentVeh, void *edx)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "driv2"
        && FindPlayerVehicle ())
        return 2;
    else
        return CallMethodAndReturn<int, 0x6D1080> (currentVeh);
}

/*******************************************************/
int
InitialiseCacheForRandomization (void *fileName)
{
    ScriptVehicleRandomizer::GetInstance ()->InitialiseCache ();
    return CGame::Init3 (fileName);
}

/*******************************************************/
void
SetMaddDoggOffset (CVehicle *vehicle, float *out, float offset)
{
    *out = ms_modelInfoPtrs[vehicle->m_nModelIndex]
               ->m_pColModel->m_boundBox.m_vecMax.z
           + offset;
}

/*******************************************************/
void __fastcall FixMaddDogg (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);

    // Madd Dogg - move boxes
    if (scr->CheckName ("doc2")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_WALTON)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[0] >> 8));

            SetMaddDoggOffset (vehicle, (float *) &ScriptParams[3], 0.5);
        }
    // Highjack - expand Cesar jump point
    else if (scr->CheckName ("toreno2")
             && !CModelInfo::IsBikeModel (
                 ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ())
             && !CModelInfo::IsQuadBikeModel (
                 ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()))
        {
            if (((float *) ScriptParams)[3] > -11.0f
                && ((float *) ScriptParams)[3] < -9.0f)
                {
                    ((float *) ScriptParams)[1] -= 5.0f;
                    ((float *) ScriptParams)[2] -= 4.0f;
                }
        }
    // New Model Army - increase radius for picking up objects
    else if (scr->CheckName ("zero4")
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != 501
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != 465)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[0] >> 8));
            if (CModelInfo::IsHeliModel (ScriptVehicleRandomizer::GetInstance ()
                                             ->GetNewCarForCheck ())
                || CModelInfo::IsPlaneModel (
                    ScriptVehicleRandomizer::GetInstance ()
                        ->GetNewCarForCheck ()))
                ((float *) ScriptParams)[3]
                    = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                          ->m_pColModel->m_boundBox.m_vecMin.z;
            else
                {
                    ((float *) ScriptParams)[2]
                        = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                              ->m_pColModel->m_boundBox.m_vecMax.y
                          + 2.0f;
                    ((float *) ScriptParams)[3]
                        = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                              ->m_pColModel->m_boundBox.m_vecMax.z
                          + 1.0f;
                }
        }
}

/*******************************************************/
void __fastcall FixMaddDoggBoxes (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("doc2")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_WALTON)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
        }
    else if (scr->CheckName ("desert9")
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != MODEL_ANDROM)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            ((float *) ScriptParams)[3]
                = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                      ->m_pColModel->m_boundBox.m_vecMin.y;
        }
    else if (scr->CheckName ("des10")
             && ScriptVehicleRandomizer::m_Config.RandomizeTrains)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            if (lastTrainNewType != 0 && lastTrainNewType != 3
                && lastTrainNewType != 6 && lastTrainNewType != 10
                && lastTrainNewType != 12 && lastTrainNewType != 13)
                SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
        }
    else if (scr->CheckName ("zero4")
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != 501
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != 465)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            if (CModelInfo::IsHeliModel (ScriptVehicleRandomizer::GetInstance ()
                                             ->GetNewCarForCheck ())
                || CModelInfo::IsPlaneModel (
                    ScriptVehicleRandomizer::GetInstance ()
                        ->GetNewCarForCheck ()))
                {
                    if (((float *) ScriptParams)[4] > -0.5)
                        ((float *) ScriptParams)[4]
                            = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                  ->m_pColModel->m_boundBox.m_vecMin.z
                              + 1.0f;
                    else if (((float *) ScriptParams)[4] < -1.1)
                        ((float *) ScriptParams)[4]
                            = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                  ->m_pColModel->m_boundBox.m_vecMin.z;
                    else
                        ((float *) ScriptParams)[4]
                            = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                  ->m_pColModel->m_boundBox.m_vecMin.z
                              + 0.2f;
                }
            else
                {
                    if (((float *) ScriptParams)[4] > -0.5)
                        {
                            ((float *) ScriptParams)[3]
                                = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                      ->m_pColModel->m_boundBox.m_vecMax.y
                                  + 2.0f;
                            ((float *) ScriptParams)[4]
                                = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                      ->m_pColModel->m_boundBox.m_vecMax.z
                                  + 2.0f;
                        }
                    else if (((float *) ScriptParams)[4] < -1.1)
                        {
                            ((float *) ScriptParams)[3]
                                = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                      ->m_pColModel->m_boundBox.m_vecMax.y
                                  + 2.0f;
                            ((float *) ScriptParams)[4]
                                = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                      ->m_pColModel->m_boundBox.m_vecMax.z
                                  + 1.0f;
                        }
                    else
                        {
                            ((float *) ScriptParams)[3]
                                = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                      ->m_pColModel->m_boundBox.m_vecMax.y
                                  + 2.0f;
                            ((float *) ScriptParams)[4]
                                = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                      ->m_pColModel->m_boundBox.m_vecMax.z
                                  + 1.2f;
                        }
                }
        }
}

/*******************************************************/
void __fastcall FixHeightInDrugs4Auto (CRunningScript *scr, void *edx,
                                       short count)
{
    scr->CollectParameters (count);
    // Reuniting the Families - Change attach point of CJ / bike cops based on vehicle
    if (scr->CheckName ("drugs4")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_GREENWOO)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            if (ScriptParams[0] == ScriptSpace[3])
                SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0.35);
            else if (((float *) ScriptParams)[3] > 2.9f
                     || ((float *) ScriptParams)[3] < -3.5f)
                SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0.01);
        }
    // Green Goo - Change attach point of army guys based on train
    else if (scr->CheckName ("des10")
             && ScriptVehicleRandomizer::m_Config.RandomizeTrains)
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            if (lastTrainNewType != 0 && lastTrainNewType != 3
                && lastTrainNewType != 6 && lastTrainNewType != 10
                && lastTrainNewType != 12 && lastTrainNewType != 13)
                SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
        }
    // Catalyst - change attach point of Ryder when catching boxes based on vehicle
    else if (scr->CheckName ("ryder3"))
        {
            if (ScriptParams[0] != GetGlobalVar<int> (3))
                {
                    CVehicle *vehicle
                        = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                        + 0xA18 * (ScriptParams[1] >> 8));
                    SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
                }
        }
}

/*******************************************************/
int
MoveFlyingSchoolCorona (float x, float y, float z, float dirX, float dirY,
                        float dirZ, float radius, int type)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "desert5")
        {
            if (int (x) == 377 && int (z) == 80)
                x += 300.0f;
        }
    return CallAndReturn<int, 0x4935A0> (x, y, z, dirX, dirY, dirZ, radius,
                                         type);
}

/*******************************************************/
int
MoveFlyingSchoolBlip (int blipType, float x, float y, float z, int a5,
                      int display)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "desert5")
        {
            if (int (x) == 377 && int (z) == 80)
                x += 300.0f;
        }
    return CallAndReturn<int, 0x583820> (blipType, x, y, z, a5, display);
}

/*******************************************************/
void __fastcall MoveFlyingSchoolTrigger (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    // Moves trigger for Barrel Roll / Loop-The-Loop coronas further back
    if (scr->CheckName ("desert5")
        && ConfigManager::ReadConfig ("ScriptVehicleRandomizer")
        && ScriptVehicleRandomizer::m_Config.MoreSchoolTestTime)
        {
            float coordCompareX = ((float *) ScriptParams)[1];
            float coordCompareZ = ((float *) ScriptParams)[3];
            if (int (coordCompareX) == 377 && int (coordCompareZ) == 80)
                ((float *) ScriptParams)[1] += 300.0f;
        }
    // Expands garage + placing box triggers for large cars
    else if (scr->CheckName ("guns1")
             && ConfigManager::ReadConfig ("ScriptVehicleRandomizer")
             && (ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                     == 431
                 || ScriptVehicleRandomizer::GetInstance ()
                            ->GetNewCarForCheck ()
                        == 437
                 || ScriptVehicleRandomizer::GetInstance ()
                            ->GetNewCarForCheck ()
                        == 443
                 || ScriptVehicleRandomizer::GetInstance ()
                            ->GetNewCarForCheck ()
                        == 578
                 || ScriptVehicleRandomizer::GetInstance ()
                            ->GetNewCarForCheck ()
                        == 455))
        {
            float xRadius = ((float *) ScriptParams)[4];
            float yRadius = ((float *) ScriptParams)[5];
            float zRadius = ((float *) ScriptParams)[6];
            if (int (xRadius) == 2 && int (yRadius) == 3 && int (zRadius) == 5)
                {
                    ((float *) ScriptParams)[5] += 2.0f;
                }
            else if (int (xRadius) == 1 && int (yRadius) == 1
                     && int (zRadius) == 2)
                {
                    ((float *) ScriptParams)[4] += 1.0f;
                    ((float *) ScriptParams)[5] += 1.0f;
                    ((float *) ScriptParams)[6] += 1.0f;
                }
        }
    // Allows player to jump out of a flying car in Dam and Blast upon reaching marker
    else if (scr->CheckName ("heist2")
             && ConfigManager::ReadConfig ("ScriptVehicleRandomizer")
             && (CModelInfo::IsCarModel (ScriptVehicleRandomizer::GetInstance ()
                                             ->GetNewCarForCheck ())
                 || CModelInfo::IsMonsterTruckModel (
                     ScriptVehicleRandomizer::GetInstance ()
                         ->GetNewCarForCheck ())))
        {
            float xRadius = ((float *) ScriptParams)[4];
            float yRadius = ((float *) ScriptParams)[5];
            float zRadius = ((float *) ScriptParams)[6];
            if (int (xRadius) == 50 && int (yRadius) == 50
                && int (zRadius) == 25
                && ScriptVehicleRandomizer::mTempVehHandle != -1)
                {
                    CVector playerLoc = FindPlayerCoors ();
                    if (playerLoc.x >= -642.42f && playerLoc.x <= -542.42f
                        && playerLoc.y >= 1505.88f && playerLoc.y <= 1605.88f
                        && playerLoc.z >= 725.0f && playerLoc.z <= 775.0f)
                        {
                            short isKeyPressed
                                = CallAndReturn<short, 0x485B10> (0, 15);
                            if (isKeyPressed == 255)
                                Scrpt::CallOpcode (0x633, "actor_exit_car",
                                                   GlobalVar (3));
                        }
                }
        }
    // Teleports player for phone call unlocking Four Dragons
    // This plays the cutscene immediately so it cannot softlock any random missions
    else if (scr->CheckName ("litcas")
             && ConfigManager::ReadConfig ("MissionRandomizer"))
        {
            ScriptVehicleRandomizer::lastPlayerPos = FindPlayerCoors ();
            CVector newPos                         = {2041.0f, 1007.3f, 10.67f};
            Scrpt::CallOpcode (0x4E4, "refresh_game_renderer", newPos.x,
                               newPos.y);
            Scrpt::CallOpcode (0xA0B, "set_rendering_origin", newPos.x,
                               newPos.y, newPos.z, 270.0f);

            CRunningScript::SetCharCoordinates (FindPlayerPed (),
                                                {newPos.x, newPos.y, newPos.z},
                                                1, 1);
        }
    // Adds 3 second delay between EOTL parts.
    // Gives time for badly scripted missions (Catalina cough) to fully clean up.
    else if (scr->CheckName ("riot")
             && ConfigManager::ReadConfig ("MissionRandomizer"))
        {
            float xPos        = ((float *) ScriptParams)[1];
            int   xSweetHouse = int (GetGlobalVar<float> (462));
            if (xSweetHouse == int (xPos))
                {
                    static int startTime;
                    if (GetGlobalVar<int> (151) == 5)
                        {
                            int currentTime = clock () - startTime;
                            if (currentTime > 3000)
                                {
                                    if (GetGlobalVar<int> (629) == 3)
                                        ScriptSpace[151] = 0;
                                    else if (GetGlobalVar<int> (629) == 4)
                                        ScriptSpace[151] = 1;
                                    Scrpt::CallOpcode (0x4D7, "freeze_player",
                                                       GlobalVar (3), 0);
                                    Scrpt::CallOpcode (0x16A, "do_fade", 100,
                                                       1);
                                }
                        }
                    else if ((GetGlobalVar<int> (629) == 3
                              && GetGlobalVar<int> (151) == 0)
                             || (GetGlobalVar<int> (629) == 4
                                 && GetGlobalVar<int> (151) == 1))
                        {
                            Scrpt::CallOpcode (0x16A, "do_fade", 250, 0);
                            Scrpt::CallOpcode (0x4D7, "freeze_player",
                                               GlobalVar (3), 1);
                            ScriptSpace[151] = 5;
                            startTime        = clock ();
                        }
                }
        }
}

/*******************************************************/
void __fastcall IgnoreLandingGearCheck (CRunningScript *scr, void *edx, int a2,
                                        short count)
{
    ScriptParams[0] = 1;
    CallMethod<0x464370> (scr, a2, count);
}

/*******************************************************/
void __fastcall IgnoreLowriderCheck (CRunningScript *scr, void *edx,
                                     char result)
{
    if (scr->CheckName ("cesar") || scr->CheckName ("lowr"))
        {
            result = 1;
        }
    scr->UpdateCompareFlag (result);
}

/*******************************************************/
void __fastcall IgnoreWuZiMuCheck (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("bcesar"))
        {
            GetGlobalVar<int> (49) = 0;
        }
}

/*******************************************************/
void __fastcall IgnoreSweetGirlCarCheck (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("hoods5"))
        {
            if (ScriptParams[0] == 596)
                {
                    ScriptParams[0] = FindPlayerVehicle ()->m_nModelIndex;
                }
        }
}

/*******************************************************/
int __fastcall ActivateSAMForFlyingCars (CVehicle *veh, void *edx)
{
    if (ScriptVehicleRandomizer::GetInstance ()->GetIfPlayerInFlyingCar ())
        return 5;
    else
        return CallMethodAndReturn<int, 0x6D1080> (
            veh); // Gets vehicle type as normal
}

/*******************************************************/
void __fastcall InitialiseExtraText (CText *text, void *edx, char a2)
{
    text->Load (a2);
    GxtManager::Initialise (text);
}

// Hooks 02AC (give vehicle immunities)
// Attaches magnet object to RC heli
/*******************************************************/
void __fastcall Ryder2AttachMagnet (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
        {
            if (ScriptParams[1] == 1 && ScriptParams[2] == 0
                && ScriptParams[3] == 0 && ScriptParams[4] == 0
                && ScriptParams[5] == 0)
                {
                    ryder2.vehID = ScriptParams[0];
                    Scrpt::CallOpcode (0x107, "create_object", 3056, 2791.426f,
                                       -2418.577f, 6.66f, GlobalVar (10947));
                    Scrpt::CallOpcode (0x681, "attach_object",
                                       GlobalVar (10947), ryder2.vehID, 0.0f,
                                       0.0f, -0.2f, 0.0f, 0.0f, 0.0f);
                    Scrpt::CallOpcode (0x382, "set_object_collision",
                                       GlobalVar (10947), 0);
                }
        }
    else if (scr->CheckName ("des3"))
        {
            ScriptVehicleRandomizer::mTempVehHandle = ScriptParams[0];
            ScriptVehicleRandomizer::mDes3Stuck     = true;
        }
}

// Hooks 0175 (set car Z angle - Car.Angle)
/*******************************************************/
void __fastcall QuarryAttachMagnet (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("quarry")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_DOZER
        && (ScriptSpace[8171] == 1 || ScriptSpace[8171] == 5))
        {
            quarry.vehID = ScriptParams[0];
            Scrpt::CallOpcode (0x107, "create_object", 3056, 816.4382f,
                               845.8509f, 7.49f, GlobalVar (10947));
            Scrpt::CallOpcode (0x681, "attach_object", GlobalVar (10947),
                               quarry.vehID, 0.0f, 0.0f, -0.2f, 0.0f, 0.0f,
                               0.0f);
            Scrpt::CallOpcode (0x382, "set_object_collision", GlobalVar (10947),
                               0);
            quarry.atMagnetSection = true;
        }
    else if (scr->CheckName ("manson5"))
        {
            float carAngle = ((float *) ScriptParams)[1];
            if (int (carAngle) == 343)
                ScriptVehicleRandomizer::mTempVehHandle = ScriptParams[0];
        }
    else if (scr->CheckName ("finalec"))
        {
            float carAngle = ((float *) ScriptParams)[1];
            if (int (carAngle) == 359)
                ScriptVehicleRandomizer::mTempVehHandle = ScriptParams[0];
        }
}

/*******************************************************/
void
RCHeliPlayerJustEntered (ScriptVehicleRandomizer::RCHeliMagnet *RCCurrent,
                         int                                    currentMission)
{
    RCCurrent->isPlayerInVeh = true;
    CVector playerPos        = FindPlayerCoors ();
    playerPos.z += 1.0f;
    CRunningScript::SetCharCoordinates (FindPlayerPed (),
                                        {playerPos.x, playerPos.y, playerPos.z},
                                        1, 1);
    Scrpt::CallOpcode (0x382, "set_object_collision", GlobalVar (10947), 1);
    if (RCCurrent->objectAttached != -1)
        {
            Scrpt::CallOpcode (0x382, "set_object_collision",
                               RCCurrent->objHandles[RCCurrent->objectAttached],
                               1);
        }
    Scrpt::CallOpcode (0x825, "start_rotors_instantly", RCCurrent->vehID);
    if (currentMission == 118)
        {
            std::string tempLabel = "QUAR_H2";
            Scrpt::CallOpcode (0x3e5, "print_help", tempLabel.c_str ());
        }
}

/*******************************************************/
void
RCHeliCheckForObjects (ScriptVehicleRandomizer::RCHeliMagnet *RCCurrent,
                       int                                    currentMission)
{
    if (currentMission == 26
        && RCCurrent->fakeColHandle != RCCurrent->invalidHandle
        && RCCurrent->fakeColHandle != 0)
        Scrpt::CallOpcode (0x382, "set_object_collision",
                           RCCurrent->fakeColHandle, 0);
    Scrpt::CallOpcode (0x407, "get_offset_from_car_in_world", RCCurrent->vehID,
                       0.0f, 0.0f, -1.0f, GlobalVar (69), GlobalVar (70),
                       GlobalVar (71));
    short isKeyPressed = CallAndReturn<short, 0x485B10> (0, 17);
    if (isKeyPressed == 255 && RCCurrent->objectAttached == -1
        && RCCurrent->pickUpObjectTimer > 50)
        {
            for (int i = 0; i < 17; i++)
                {
                    if (RCCurrent->objHandles[i] != 0)
                        {
                            Scrpt::CallOpcode (0x1bb, "get_object_coords",
                                               RCCurrent->objHandles[i],
                                               GlobalVar (8924),
                                               GlobalVar (8925),
                                               GlobalVar (8926));
                            float xCompare = GetGlobalVar<float> (69)
                                             - GetGlobalVar<float> (8924);
                            float yCompare = GetGlobalVar<float> (70)
                                             - GetGlobalVar<float> (8925);
                            float zCompare = GetGlobalVar<float> (71)
                                             - GetGlobalVar<float> (8926);
                            if (xCompare >= -1.0f && xCompare <= 1.0f
                                && yCompare >= -1.0f && yCompare <= 1.0f
                                && zCompare >= -1.0f && zCompare <= 1.0f)
                                {
                                    float offsetZ = 0;
                                    if (currentMission == 26)
                                        offsetZ = -1.4f;
                                    else
                                        offsetZ = -1.2f;
                                    Scrpt::CallOpcode (0x681, "attach_object",
                                                       RCCurrent->objHandles[i],
                                                       RCCurrent->vehID, 0.0f,
                                                       0.0f, offsetZ, 0.0f,
                                                       0.0f, 0.0f);
                                    Scrpt::CallOpcode (0x97b,
                                                       "play_audio_at_object",
                                                       RCCurrent->objHandles[i],
                                                       1009);
                                    CVector playerPos = FindPlayerCoors ();
                                    playerPos.z += 0.3f;
                                    CRunningScript::SetCharCoordinates (
                                        FindPlayerPed (),
                                        {playerPos.x, playerPos.y, playerPos.z},
                                        1, 1);
                                    RCCurrent->objectAttached    = i;
                                    RCCurrent->pickUpObjectTimer = 0;
                                    break;
                                }
                        }
                }
        }
    else if (isKeyPressed == 255 && RCCurrent->objectAttached > -1
             && RCCurrent->pickUpObjectTimer > 50)
        {
            Scrpt::CallOpcode (0x682, "detach_object",
                               RCCurrent->objHandles[RCCurrent->objectAttached],
                               0.0f, 0.0f, 0.0f, 1);
            RCCurrent->objectAttached    = -1;
            RCCurrent->pickUpObjectTimer = 0;
        }
}

/*******************************************************/
void
RCHeliPlayerJustExited (ScriptVehicleRandomizer::RCHeliMagnet *RCCurrent,
                        int                                    currentMission)
{
    RCCurrent->isPlayerInVeh = false;
    if (currentMission == 26)
        RCCurrent->invalidHandle = RCCurrent->fakeColHandle;

    Scrpt::CallOpcode (0x382, "set_object_collision", GlobalVar (10947), 0);
    if (RCCurrent->objectAttached != -1)
        {
            Scrpt::CallOpcode (0x382, "set_object_collision",
                               RCCurrent->objHandles[RCCurrent->objectAttached],
                               0);
        }
    // DO NOT REMOVE THIS LOG
    // For some reason the game crashes getting in the RC heli without it?
    Logger::GetLogger ()->LogMessage (
        "Player has exited heli, reset collision");
}

/*******************************************************/
void __fastcall IsPlayerInVehicleCheck (CRunningScript *scr, void *edx,
                                        char flag)
{
    int                                    currentMission = -1;
    ScriptVehicleRandomizer::RCHeliMagnet *RCCurrent;
    if (scr->CheckName ("desert6") || scr->CheckName ("casino9")
        || scr->CheckName ("casin10") || scr->CheckName ("heist2")
        || (scr->CheckName ("cprace")
            && (ScriptSpace[352] >= 19 && ScriptSpace[352] <= 24)))
        SetFlyingCar (flag);
    else if (scr->CheckName ("ryder2") && ScriptParams[1] == ryder2.vehID
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != MODEL_FORKLIFT)
        {
            RCCurrent      = &ryder2;
            currentMission = 26;
        }
    else if (scr->CheckName ("quarry")
             && ((ScriptSpace[8171] == 1 || ScriptSpace[8171] == 5)
                 && ScriptParams[1] == quarry.vehID
                 && ScriptVehicleRandomizer::GetInstance ()
                            ->GetNewCarForCheck ()
                        != MODEL_DOZER))
        {
            RCCurrent      = &quarry;
            currentMission = 118;
        }

    if (currentMission != -1)
        {
            RCCurrent->pickUpObjectTimer++;

            if (flag && RCCurrent->isPlayerInVeh)
                {
                    RCHeliCheckForObjects (RCCurrent, currentMission);
                }
            else if (flag && !RCCurrent->isPlayerInVeh)
                {
                    RCHeliPlayerJustEntered (RCCurrent, currentMission);
                }
            else if (!flag && RCCurrent->isPlayerInVeh)
                {
                    RCHeliPlayerJustExited (RCCurrent, currentMission);
                }
        }

    scr->UpdateCompareFlag (flag);
}

/*******************************************************/
void __fastcall Ryder2IsCutsceneActive (CRunningScript *scr, void *edx,
                                        short count)
{
    scr->CollectParameters (count);
    // Marks box object as used once placed in the van
    if (scr->CheckName ("ryder2")
        && ConfigManager::ReadConfig ("ScriptVehicleRandomizer"))
        {
            ryder2.isCutsceneActive = ScriptParams[0];
            if (ryder2.objectAttached > -1)
                {
                    ryder2.objHandles[ryder2.objectAttached] = 0;
                    ryder2.objectAttached                    = -1;
                }
        }
    // Enables / disables cars on water in Cut Throat Business when needed
    else if (scr->CheckName ("manson5")
             && ConfigManager::ReadConfig ("ScriptVehicleRandomizer"))
        {
            if (ScriptParams[0] == 1)
                {
                    bool isWaterCarsOn
                        = injector::ReadMemory<bool> (waterCarsCheatActive);
                    if (isWaterCarsOn)
                        injector::WriteMemory (waterCarsCheatActive, 0);
                    else
                        injector::WriteMemory (waterCarsCheatActive, 1);
                }
            else if (ScriptParams[0] == 0
                     && ScriptVehicleRandomizer::mTempVehHandle != -1)
                {
                    Scrpt::CallOpcode (0x825, "start_rotors_instantly",
                                       ScriptVehicleRandomizer::mTempVehHandle);
                }
        }
    // Teleports player back to their original position after teleporting 
    // for Four Dragons cutscene.
    else if (scr->CheckName ("litcas")
             && ConfigManager::ReadConfig ("MissionRandomizer"))
        {
            if (ScriptParams[0] == 0)
                {
                    CVector oldPos = ScriptVehicleRandomizer::lastPlayerPos;
                    Scrpt::CallOpcode (0x4E4, "refresh_game_renderer", oldPos.x,
                                       oldPos.y);
                    Scrpt::CallOpcode (0xA0B, "set_rendering_origin", oldPos.x,
                                       oldPos.y, oldPos.z, 0.0f);

                    CRunningScript::SetCharCoordinates (
                        FindPlayerPed (), {oldPos.x, oldPos.y, oldPos.z}, 1, 1);
                }
        }
}

// Hooks 0177 (set object Z angle)
// Saves handles for all relevant objects for future reference
/*******************************************************/
void __fastcall Ryder2StoreBoxHandles (CRunningScript *scr, void *edx,
                                       short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2"))
        {
            if (ryder2.currentObj == 17)
                {
                    ryder2.fakeColHandle = ScriptParams[0];
                    ryder2.currentObj    = 17;
                }

            if (ryder2.isCutsceneActive)
                {
                    ryder2.atMagnetSection               = true;
                    ryder2.objHandles[ryder2.currentObj] = ScriptParams[0];
                    ryder2.currentObj++;
                }
        }
}

/*******************************************************/
void
GenericStoreObjects (int numberOfObjects)
{
    bool alreadyStored = false;
    for (int i = 0; i < 17; i++)
        {
            if (i < numberOfObjects)
                {
                    if (quarry.objHandles[i] == ScriptParams[0])
                        {
                            alreadyStored = true;
                            break;
                        }
                }
            else if (i >= numberOfObjects)
                quarry.objHandles[i] = 0;
        }
    if (!alreadyStored)
        {
            quarry.objHandles[quarry.currentObj] = ScriptParams[0];
            quarry.currentObj++;
        }
}

// Hooks 04e6 (is object near point)
/*******************************************************/
void __fastcall Quarry2StoreBarrelHandles (CRunningScript *scr, void *edx,
                                           short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("quarry") && ScriptSpace[8171] == 1
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_DOZER)
        {
            GenericStoreObjects (4);
        }
    else if (scr->CheckName ("zero4"))
        {
            if (!CModelInfo::IsHeliModel (
                    ScriptVehicleRandomizer::GetInstance ()
                        ->GetNewCarForCheck ())
                && !CModelInfo::IsPlaneModel (
                    ScriptVehicleRandomizer::GetInstance ()
                        ->GetNewCarForCheck ()))
                {
                    ((float *) ScriptParams)[4] = 2.0f;
                    ((float *) ScriptParams)[5] = 2.0f;
                    ((float *) ScriptParams)[6] = 10.0f;
                }
            else if (ScriptVehicleRandomizer::GetInstance ()
                             ->GetNewCarForCheck ()
                         != 501
                     && ScriptVehicleRandomizer::GetInstance ()
                                ->GetNewCarForCheck ()
                            != 465)
                {
                    ((float *) ScriptParams)[4] = 2.0f;
                    ((float *) ScriptParams)[5] = 2.0f;
                    ((float *) ScriptParams)[6] = 2.0f;
                }
        }
}

// Hooks 03ca (object exists)
/*******************************************************/
void __fastcall Quarry6StoreBarrelHandles (CRunningScript *scr, void *edx,
                                           short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("quarry") && ScriptSpace[8171] == 5
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_DOZER)
        {
            GenericStoreObjects (10);
        }
}

// Hooks 0366 (has object been damaged)
/*******************************************************/
void __fastcall Ryder2CheckBoxForDamage (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
        {
            for (int i = 0; i < 17; i++)
                {
                    if (ryder2.objHandles[i] != 0
                        && ScriptParams[0] == ryder2.objHandles[i])
                        {
                            ryder2.checkForObjectExistence = i;
                        }
                }
        }
}

/*******************************************************/
void __fastcall Ryder2CheckBoxForDamage2 (CRunningScript *scr, void *edx,
                                          char flag)
{
    if (scr->CheckName ("ryder2")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
        {
            if (flag && ryder2.checkForObjectExistence != -1)
                {
                    ryder2.objHandles[ryder2.checkForObjectExistence] = 0;
                    if (ryder2.objectAttached == ryder2.checkForObjectExistence)
                        ryder2.objectAttached = -1;
                }
        }
    ryder2.checkForObjectExistence = -1;
    scr->UpdateCompareFlag (flag);
}

// Hooks 0108 (delete object)
/*******************************************************/
void __fastcall Ryder2WasBoxDestroyedSomehow (CRunningScript *scr, void *edx,
                                              short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
        {
            for (int i = 0; i < 17; i++)
                {
                    if (ryder2.objHandles[i] != 0
                        && ScriptParams[0] == ryder2.objHandles[i])
                        {
                            ryder2.objHandles[i] = 0;
                        }
                }
        }
}

// Hooks 0382 (set object collision)
/*******************************************************/
void __fastcall Quarry2RemoveSafeBarrel (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("quarry") && ScriptSpace[8171] == 1
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_DOZER)
        {
            for (int i = 0; i < 4; i++)
                {
                    if (quarry.objHandles[i] != 0
                        && ScriptParams[0] == quarry.objHandles[i])
                        {
                            if (quarry.objectAttached != -1
                                && quarry.objectAttached == i)
                                {
                                    Scrpt::CallOpcode (
                                        0x682, "detach_object",
                                        quarry
                                            .objHandles[quarry.objectAttached],
                                        0.0f, 0.0f, 0.0f, 1);
                                    quarry.objectAttached = -1;
                                }
                            quarry.objHandles[i] = 0;
                        }
                }
        }
}

// Hooks 03E5 (displays black text box in corner with help info)
/*******************************************************/
void
Ryder2ReplaceHelp (char *str, bool quickMessage, bool permanent,
                   bool addToBrief)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "ryder2"
        && ryder2.isPlayerInVeh)
        {
            static int  currentTextBoxForMission = 0;
            std::string newText;
            if (currentTextBoxForMission == 0)
                {
                    newText = "Your helicopter is fitted with a magnet which "
                              "can pick up objects.";
                }
            else if (currentTextBoxForMission == 1)
                {
                    newText = "Press the Fire Button to pick up and drop the "
                              "boxes.";
                }
            else if (currentTextBoxForMission == 2)
                {
                    newText = "Once a box is attached, carefully place it in "
                              "the back "
                              "of Ryder's vehicle.";
                }
            if (currentTextBoxForMission != 2)
                currentTextBoxForMission++;
            else
                currentTextBoxForMission = 0;
            Call<0x588BE0> (newText.c_str (), quickMessage, permanent,
                            addToBrief);
        }
    else if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "quarry"
             && (ScriptSpace[8171] == 1 || ScriptSpace[8171] == 5)
             && quarry.isPlayerInVeh)
        {
            std::string newText = "Press the Fire Button to pick up and drop "
                                  "the barrels with your helicopter's magnet.";
            Call<0x588BE0> (newText.c_str (), quickMessage, permanent,
                            addToBrief);
        }
    else
        {
            Call<0x588BE0> (str, quickMessage, permanent, addToBrief);
        }
}

/*******************************************************/
void __fastcall Ryder2IncreaseRadius (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
        {
            ((float *) ScriptParams)[4] = 3.0f;
            ((float *) ScriptParams)[6] = 2.75f;
        }
}

// Mission Cleanup hook
/*******************************************************/
char
ResetEndOfMissionStuff (char enable)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "ryder2"
        || ScriptVehicleRandomizer::GetInstance ()->mLastThread == "quarry")
        {
            if ((ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                     != MODEL_FORKLIFT
                 && ryder2.atMagnetSection)
                || (ScriptVehicleRandomizer::GetInstance ()
                            ->GetNewCarForCheck ()
                        != MODEL_DOZER
                    && quarry.atMagnetSection))
                {
                    Scrpt::CallOpcode (
                        0x1c4, "remove_references_to_object",
                        GlobalVar (10947)); // Stops magnet from persisting in
                                            // save forever
                    ryder2.isPlayerInVeh = false;
                    quarry.isPlayerInVeh = false;
                }
        }

    if (ScriptVehicleRandomizer::GetInstance ()->mCurrentMissionRunning >= 121
        && ScriptVehicleRandomizer::GetInstance ()->mCurrentMissionRunning
               <= 133)
        {
            Scrpt::CallOpcode (0x1c3, "remove_references_to_vehicle",
                               GlobalVar (9002));
            Scrpt::CallOpcode (0x1c3, "remove_references_to_vehicle",
                               GlobalVar (9003));
            Scrpt::CallOpcode (0x1c3, "remove_references_to_vehicle",
                               GlobalVar (9004));
        }

    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "manson5")
        injector::WriteMemory (waterCarsCheatActive, 0);

    ScriptVehicleRandomizer::mTempVehHandle                         = -1;
    ScriptVehicleRandomizer::mFallbackVeh                           = -1;
    ScriptVehicleRandomizer::mNextVigilanteCarNum                   = 0;
    ScriptVehicleRandomizer::mVigilanteSlotsTaken                   = false;
    ScriptVehicleRandomizer::GetInstance ()->mCurrentMissionRunning = -1;
    return CallAndReturn<char, 0x6F5DB0> (enable);
}

/*******************************************************/
void __fastcall StoreStartedMission (CRunningScript *scr, void *edx,
                                     short count)
{
    scr->CollectParameters (count);
    ScriptVehicleRandomizer::GetInstance ()->mCurrentMissionRunning
        = ScriptParams[0];
    std::map oddMissions = ScriptVehicleRandomizer::GetInstance ()->oddMissions;
    if (oddMissions.find (ScriptParams[0]) != oddMissions.end ())
        ScriptVehicleRandomizer::GetInstance ()->UpdateLastThread (
            oddMissions[ScriptParams[0]]);
}

// Start Mission hook
// Reset variables for RC box magnet section at start of Robbing Uncle Sam
// Also resets other variables and handles randomizing vehicle sub-missions
/*******************************************************/
char
SetThingsForMissionStart ()
{
    // DO NOT REMOVE THIS LOG
    // For some reason the game crashes getting in the RC heli without it?
    Logger::GetLogger ()->LogMessage ("Clearing variables for mission start");
    ryder2                                        = emptyTemplate;
    quarry                                        = emptyTemplate;
    ScriptVehicleRandomizer::mTempVehHandle       = -1;
    ScriptVehicleRandomizer::mFallbackVeh         = -1;
    ScriptVehicleRandomizer::mDes3Stuck           = false;
    ScriptVehicleRandomizer::mEOTL3Slow           = false;
    ScriptVehicleRandomizer::mNextVigilanteCarNum = 0;
    ScriptVehicleRandomizer::mVigilanteSlotsTaken = false;
    int currentMissionId
        = ScriptVehicleRandomizer::GetInstance ()->mCurrentMissionRunning;

    if ((currentMissionId == 121 && ScriptVehicleRandomizer::m_Config.Taxi)
        || (currentMissionId == 122
            && ScriptVehicleRandomizer::m_Config.Paramedic)
        || (currentMissionId == 123
            && ScriptVehicleRandomizer::m_Config.Firefighting)
        || (currentMissionId == 124
            && ScriptVehicleRandomizer::m_Config.Vigilante)
        || (currentMissionId == 125
            && ScriptVehicleRandomizer::m_Config.Burglary)
        || (currentMissionId == 127
            && ScriptVehicleRandomizer::m_Config.Pimping)
        || (currentMissionId == 131
            && ScriptVehicleRandomizer::m_Config.Courier)
        || ((currentMissionId == 132 || currentMissionId == 133)
            && ScriptVehicleRandomizer::m_Config.Bike))
        {
            ChangePlayerVehicle (currentMissionId);
        }
    return CallAndReturn<char, 0x5619D0> ();
}

// Handles randomizing player vehicle for starting vehicle sub-missions
/*******************************************************/
void
ChangePlayerVehicle (int mission)
{
    CPhysical *player    = (CPhysical *) FindPlayerEntity (-1);
    CVector    moveSpeed = player->m_vecMoveSpeed;
    CVector    turnSpeed = player->m_vecTurnSpeed;
    CVector    location  = FindPlayerCoors ();
    if (FindPlayerVehicle ())
        {
            Scrpt::CallOpcode (0x3c0, "store_car_char_is_in", GlobalVar (3),
                               GlobalVar (9005));

            CVehicle *originalVeh = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                + 0xA18 * (ScriptSpace[9005] >> 8));
            ScriptVehicleRandomizer::mFallbackVeh = originalVeh->m_nModelIndex;

            Scrpt::CallOpcode (0x174, "get_car_heading", GlobalVar (9005),
                               GlobalVar (72));
            Scrpt::CallOpcode (0x792, "disembark_actor", GlobalVar (3));
            Scrpt::CallOpcode (0xa6, "delete_car", GlobalVar (9005));
        }

    int carType = -1;
    int courierType
        = ScriptVehicleRandomizer::GetInstance ()->mCourierMissionType;
    if (mission == 133 && ScriptSpace[726] == 0)
        carType = MODEL_BIKE;
    else if (mission == 131 && courierType == 0)
        carType = MODEL_BMX;
    else if (mission == 131 && courierType == 1)
        carType = MODEL_FREEWAY;
    else if (mission == 131 && courierType == 2)
        carType = MODEL_FAGGIO;
    else if (mission == 121)
        carType = MODEL_TAXI;
    else if (mission == 122)
        carType = MODEL_AMBULAN;
    else if (mission == 123)
        carType = MODEL_FIRETRU;
    else if (mission == 124)
        carType = MODEL_COPBIKE;
    else
        carType = MODEL_NRG;

    int carStore = 9002;
    if (mission == 124)
        carStore += ScriptVehicleRandomizer::mNextVigilanteCarNum;

    if (ScriptVehicleRandomizer::mVigilanteSlotsTaken)
        Scrpt::CallOpcode (0x1c3, "remove_references_to_vehicle",
                           GlobalVar (carStore));

    Scrpt::CallOpcode (0xa5, "create_car", carType, location.x, location.y,
                       location.z, GlobalVar (carStore));

    Scrpt::CallOpcode (0x036A, "put_actor_in_car", GlobalVar (3),
                       GlobalVar (carStore));
    Scrpt::CallOpcode (0x175, "set_car_heading", GlobalVar (carStore),
                       GlobalVar (72));

    if (mission == 124)
        {
            if (ScriptVehicleRandomizer::mNextVigilanteCarNum == 2)
                {
                    ScriptVehicleRandomizer::mNextVigilanteCarNum = 0;
                    ScriptVehicleRandomizer::mVigilanteSlotsTaken = true;
                }
            else
                ScriptVehicleRandomizer::mNextVigilanteCarNum++;
        }

    player                 = (CPhysical *) FindPlayerEntity (-1);
    player->m_vecMoveSpeed = moveSpeed;
    player->m_vecTurnSpeed = turnSpeed;
    ScriptVehicleRandomizer::GetInstance ()->mCourierMissionType = -1;
}

/*******************************************************/
void __fastcall FixRaiseDoorHeist9 (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("heist9")
        && (ScriptParams[1] == 1165 || ScriptParams[1] == 1166)
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
        {
            GetGlobalVar<float> (7937) = 1003.0f;
        }
}

/*******************************************************/
void __fastcall FixRaiseDoorHeist9PosCheck (CRunningScript *scr, void *edx,
                                            short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("heist9") && FindPlayerVehicle ()
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
        {
            CVector playerPos = FindPlayerCoors ();
            if (playerPos.z >= 950.0f && playerPos.y >= 1570.7f
                && playerPos.y <= 1577.0f)
                {
                    GetGlobalVar<float> (74) = 1573.0f;
                }
        }
}

/*******************************************************/
void __fastcall TrailerAttachmentCheck (CRunningScript *scr, void *edx,
                                        char flag)
{
    if (!flag && !currentTrailerAttached && FindPlayerVehicle ())
        {
            CVehicle *mainVehicle
                = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                + 0xA18 * (ScriptParams[0] >> 8));
            CVehicle *trailerVehicle
                = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                + 0xA18 * (ScriptParams[1] >> 8));
            Scrpt::CallOpcode (0x174, "get_car_heading", ScriptParams[0],
                               GlobalVar (69));
            Scrpt::CallOpcode (0x174, "get_car_heading", ScriptParams[1],
                               GlobalVar (70));
            Scrpt::CallOpcode (0x407, "get_offset_from_car_in_world",
                               ScriptParams[0], 0.0f,
                               -(ms_modelInfoPtrs[mainVehicle->m_nModelIndex]
                                     ->m_pColModel->m_boundBox.m_vecMin.y),
                               0.0f, GlobalVar (8924), GlobalVar (8925),
                               GlobalVar (8926));
            Scrpt::CallOpcode (0x407, "get_offset_from_car_in_world",
                               ScriptParams[1], 0.0f,
                               -(ms_modelInfoPtrs[trailerVehicle->m_nModelIndex]
                                     ->m_pColModel->m_boundBox.m_vecMax.y),
                               0.0f, GlobalVar (8927), GlobalVar (8928),
                               GlobalVar (8929));

            float angleCompare
                = GetGlobalVar<float> (69) - GetGlobalVar<float> (70);
            float xCompare
                = GetGlobalVar<float> (8924) - GetGlobalVar<float> (8927);
            float yCompare
                = GetGlobalVar<float> (8925) - GetGlobalVar<float> (8928);
            float zCompare
                = GetGlobalVar<float> (8926) - GetGlobalVar<float> (8929);
            if (angleCompare >= -45.0f && angleCompare <= 45.0f
                && xCompare >= -1.5f && xCompare <= 1.5f && yCompare >= -1.5f
                && yCompare <= 1.5f && zCompare >= -3.5f && zCompare <= 3.5f)
                {
                    Scrpt::CallOpcode (0x893, "attach_trailer", ScriptParams[0],
                                       ScriptParams[1]);
                    currentTrailerAttached = true;
                    flag                   = 1;
                }
        }
    else if (!flag && currentTrailerAttached)
        currentTrailerAttached = false;

    scr->UpdateCompareFlag (flag);
}

/*******************************************************/
void __fastcall HasTrailerForceAttached (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("toreno2") || scr->CheckName ("truck"))
        currentTrailerAttached = true;
}

/*******************************************************/
void __fastcall FixToreno2 (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("toreno2"))
        GetScriptParam<float> (4) = 0.5f;
}

/*******************************************************/
void __fastcall FixBoatSchoolObjectPlacements (CRunningScript *scr, void *edx,
                                               short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat"))
        {
            int currentVehicle
                = ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ();
            if (ScriptSpace[8189] == 5
                && (CModelInfo::IsHeliModel (currentVehicle)
                    || CModelInfo::IsPlaneModel (currentVehicle)
                    || CModelInfo::IsBoatModel (currentVehicle)))
                {
                    ((float *) ScriptParams)[2] -= 60.0f;
                }
        }
}

/*******************************************************/
void __fastcall RandomizeTrainForScript (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    lastTrainOldType = ScriptParams[0];
    lastTrainNewType = random (0, 15);

    if (scr->CheckName ("ryder3"))
        {
            std::vector<int> usableTrains = {6, 8, 9, 10, 14, 15};
            lastTrainNewType              = GetRandomElement (usableTrains);
        }
    else
        {
            while (trainTypes[lastTrainNewType] > trainTypes[lastTrainOldType])
                lastTrainNewType = random (0, 15);
        }

    eLoadError loadState1 = ERR_LOADED;
    eLoadError loadState2 = ERR_LOADED;
    ScriptParams[0]       = lastTrainNewType;

    if (lastTrainNewType == 0 || lastTrainNewType == 3 || lastTrainNewType == 6
        || lastTrainNewType == 10 || lastTrainNewType == 12
        || lastTrainNewType == 13)
        {
            loadState1 = StreamingManager::AttemptToLoadVehicle (537);
            loadState2 = StreamingManager::AttemptToLoadVehicle (569);
        }
    else if (lastTrainNewType == 8 || lastTrainNewType == 9
             || lastTrainNewType == 14)
        loadState1 = StreamingManager::AttemptToLoadVehicle (449);
    else if (lastTrainNewType == 1 || lastTrainNewType == 2
             || lastTrainNewType == 4 || lastTrainNewType == 5
             || lastTrainNewType == 7 || lastTrainNewType == 11)
        {
            loadState1 = StreamingManager::AttemptToLoadVehicle (538);
            loadState2 = StreamingManager::AttemptToLoadVehicle (570);
        }
    else if (lastTrainNewType == 15)
        loadState1 = StreamingManager::AttemptToLoadVehicle (538);

    if (loadState1 == ERR_FAILED || loadState2 == ERR_FAILED)
        {
            ScriptParams[0]  = lastTrainOldType;
            lastTrainNewType = lastTrainOldType;
        }
}

/*******************************************************/
void __fastcall IgnoreTrainCarriages (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("scrash2"))
        {
            if (snailTrailTrain.empty ()
                || snailTrailTrain[0] != ScriptParams[0])
                {
                    snailTrailTrain.clear ();
                    snailTrailTrain.push_back (ScriptParams[0]);
                    snailTrailTrain.push_back (lastTrainNewType);
                    snailTrailTrain.push_back (lastTrainOldType);
                }
            else if (snailTrailTrain[0] == ScriptParams[0]
                     && ScriptParams[1] == 2)
                {
                    lastTrainNewType = snailTrailTrain[1];
                    lastTrainOldType = snailTrailTrain[2];
                    snailTrailTrain.clear ();
                }
        }
    int numOfCarriagesNew  = trainTypes[lastTrainNewType];
    int numOfCarriagesOld  = trainTypes[lastTrainOldType];
    int carriageDifference = numOfCarriagesOld - numOfCarriagesNew;
    if (carriageDifference > 0)
        {
            if (numOfCarriagesNew == 1)
                ScriptParams[1] = 0;
            else if (numOfCarriagesNew == 2 && ScriptParams[1] > 1)
                ScriptParams[1] = 1;
            else if (numOfCarriagesNew == 3 && ScriptParams[1] > 2)
                ScriptParams[1] = 2;
            else if (numOfCarriagesNew == 4 && ScriptParams[1] > 3)
                ScriptParams[1] = 3;
            else if (numOfCarriagesNew == 5 && ScriptParams[1] > 4)
                ScriptParams[1] = 4;
        }
}

// Manually removes reporter from train if it is a non-passenger train
/*******************************************************/
void __fastcall FixSnailTrailTrain (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("scrash2"))
        {
            if (lastTrainNewType != 1 && lastTrainNewType != 2
                && lastTrainNewType != 4 && lastTrainNewType != 5
                && lastTrainNewType != 7 && lastTrainNewType != 11)
                Scrpt::CallOpcode (0x362, "remove_actor_from_car",
                                   ScriptParams[0], 843.377f, -1389.283f,
                                   -1.269f);
        }
}

/*******************************************************/
CTrain *
ChangeLastTrainCarriage (CTrain *train)
{
    if (lastTrainNewType == 9 || lastTrainNewType == 14
        || lastTrainNewType == 15)
        return train;
    else
        return train->m_pNextCarriage;
}

/*******************************************************/
void __fastcall MoveLargeCarsApart (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    float &x = ((float *) ScriptParams)[1];
    float &y = ((float *) ScriptParams)[2];
    float &z = ((float *) ScriptParams)[3];
    if (scr->CheckName ("driv2"))
        {
            if (int (x) == -1894)
                {
                    y = 603.7217f;
                }
            else if (int (x) == -1899)
                {
                    x = -1927.569f;
                    z = 34.75f;
                }
            else if (int (x) == -1898)
                {
                    x = -1909.035f;
                    y = 570.5948f;
                    z = 34.75f;
                }
        }
    else if (scr->CheckName ("cat1"))
        {
            if (int (x) == 363)
                {
                    x = 343.4054f;
                    y = -94.06548f;
                    z = 3.949051f;
                }
            else if (int (x) == 370)
                {
                    x = 371.5868f;
                    y = -91.75879f;
                    z = 3.386068f;
                }
            else if (int (x) == 376)
                {
                    x = 389.1082f;
                    y = -65.94323f;
                    z = 3.655376f;
                }
        }
    else if (scr->CheckName ("zero4"))
        {
            y -= 8.0f;
        }
}

/*******************************************************/
void __fastcall FixStuckAtDohertyGarage (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("scrash3")
        || (scr->CheckName ("steal5") && ScriptParams[1] == 1489))
        {
            if (ScriptParams[0] == GetGlobalVar<int> (3))
                {
                    if (timerStartTime < 0.0f)
                        {
                            timerStartTime = clock ();
                        }
                    timerCurrent = clock () - timerStartTime;
                    if ((int) timerCurrent >= 10000)
                        {
                            CVector newCoords = {-2047.5f, 178.5f, 27.8f};
                            if (scr->CheckName ("steal5"))
                                newCoords = {-2045.4f, 178.3f, 27.6f};
                            CRunningScript::SetCharCoordinates (
                                FindPlayerPed (), {-2047.5f, 178.5f, 27.8f}, 1,
                                1);
                            timerStartTime = -1.0f;
                        }
                }
        }
}

/*******************************************************/
void __fastcall ReduceCarWeight (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("heist5"))
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[0] >> 8));
            vehicle->m_fMass  = 500.0f;
            vehicle->m_pHandling->fMass = 500.0f;
        }
}

/*******************************************************/
void __fastcall OverrideTaxiCheck (CRunningScript *scr, void *edx, char flag)
{
    if (scr->CheckName ("taxiodd") && FindPlayerVehicle ()
        && ScriptVehicleRandomizer::GetInstance ()->m_Config.Taxi)
        flag = true;
    scr->UpdateCompareFlag (flag);
}

/*******************************************************/
double __fastcall IgnoreHeightLimit (float a1, void *edx, float a2)
{
    if (CRunningScripts::CheckForRunningScript ("zero4"))
        return -1.0;
    return CallMethodAndReturn<double, 0x6D2600> (a1, a2);
}

/*******************************************************/
void __fastcall DetectVehicleDestruction (CRunningScript *scr, void *edx,
                                          short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("des3"))
        {
            if (ScriptParams[0] == ScriptVehicleRandomizer::mTempVehHandle)
                {
                    if (FindPlayerVehicle ())
                        {
                            Scrpt::CallOpcode (0x3c0, "store_car_char_is_in",
                                               GlobalVar (3), GlobalVar (9002));
                            if (GetGlobalVar<int> (9002)
                                == ScriptVehicleRandomizer::mTempVehHandle)
                                {
                                    Scrpt::CallOpcode (0x792, "disembark_actor",
                                                       GlobalVar (3));
                                }
                        }
                }
        }
}

/*******************************************************/
void __fastcall CheckDes3HeliDriver (CRunningScript *scr, void *edx,
                                     short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("des3"))
        {
            if (CModelInfo::IsCarModel (ScriptVehicleRandomizer::GetInstance ()
                                            ->GetNewCarForCheck ())
                && ScriptParams[0] == ScriptVehicleRandomizer::mTempVehHandle)
                {
                    if (!ScriptVehicleRandomizer::mDes3Stuck)
                        {
                            Scrpt::CallOpcode (
                                0x519, "freeze_car",
                                ScriptVehicleRandomizer::mTempVehHandle, 0);
                        }
                    else if (ScriptVehicleRandomizer::mDes3Stuck)
                        {
                            Scrpt::CallOpcode (
                                0x519, "freeze_car",
                                ScriptVehicleRandomizer::mTempVehHandle, 1);
                            if (FindPlayerVehicle ())
                                {
                                    Scrpt::CallOpcode (0x3c0,
                                                       "store_car_char_is_in",
                                                       GlobalVar (3),
                                                       GlobalVar (9002));
                                    if (GetGlobalVar<int> (9002)
                                        == ScriptVehicleRandomizer::
                                            mTempVehHandle)
                                        {
                                            ScriptVehicleRandomizer::mDes3Stuck
                                                = false;
                                        }
                                }
                        }
                }
        }
    else if (scr->CheckName ("heist2") || scr->CheckName ("heist3"))
        ScriptVehicleRandomizer::mTempVehHandle = ScriptParams[0];
}

/*******************************************************/
void __fastcall GetCurrentPlayerRCVeh (CRunningScript *scr, void *edx,
                                       short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("zero4"))
        ScriptVehicleRandomizer::mTempVehHandle = ScriptParams[1];
}

/*******************************************************/
void __fastcall ActivateZero4SelfDestruct (CRunningScript *scr, void *edx,
                                           short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("zero4"))
        {
            static bool firstRun          = false;
            static int  timerStartMessage = 0.0f;
            if (timerStartMessage < 0.0f)
                timerStartMessage = clock ();
            int timerMessageCurrent = clock () - timerStartMessage;
            if ((int) timerMessageCurrent >= 40000 || !firstRun)
                {
                    firstRun            = true;
                    timerStartMessage   = -1.0f;
                    std::string tempKey = "Z4_H1";
                    Scrpt::CallOpcode (0xbb, "print_text_lowpriority",
                                       tempKey.c_str (), 20000, 1);
                }
            short isKeyPressed = CallAndReturn<short, 0x485B10> (0, 11);
            if (ScriptVehicleRandomizer::mTempVehHandle != -1)
                if (isKeyPressed == 255)
                    Scrpt::CallOpcode (0x20b, "explode_car",
                                       ScriptVehicleRandomizer::mTempVehHandle);
        }
}

/*******************************************************/
char *
ReplaceMessageText (char *str, int time, short flag, char bAddToPreviousBrief)
{
    if (time == 20000 && CRunningScripts::CheckForRunningScript ("zero4"))
        {
            std::string text = "Press ~k~~CONVERSATION_YES~ to self-destruct "
                               "your vehicle if you are stuck.";
            return CallAndReturn<char *, 0x69F0B0> (text.c_str (), time, flag,
                                                    bAddToPreviousBrief);
        }
    else
        {
            return CallAndReturn<char *, 0x69F0B0> (str, time, flag,
                                                    bAddToPreviousBrief);
        }
}

/*******************************************************/
void __fastcall AddZero1Immunities (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("zero1") && ScriptParams[0] != GetGlobalVar<int> (3)
        && !ScriptVehicleRandomizer::GetInstance ()->m_Config.OldAirRaid)
        ScriptParams[4] = 1;
}

/*******************************************************/
void __fastcall FixStuckVehicles (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("heist3") && ScriptParams[0] == GetGlobalVar<int> (3))
        {
            float angleCheck = ((float *) ScriptParams)[1];
            if (int (angleCheck) == 223)
                {
                    Scrpt::CallOpcode (
                        0xab, "move_car",
                        ScriptVehicleRandomizer::GetInstance ()->mTempVehHandle,
                        2109.004f, 2039.756f, 10.8125f);
                }
        }
}

/*******************************************************/
void __fastcall IncreaseStowawayRadius (CRunningScript *scr, void *edx,
                                        short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("desert9"))
        {
            float xRadius = ((float *) ScriptParams)[2];
            float yRadius = ((float *) ScriptParams)[3];
            float zRadius = ((float *) ScriptParams)[4];
            if (int (xRadius) == 4 && int (yRadius) == 1 && int (zRadius) == 1)
                ((float *) ScriptParams)[2] = 8.0f;
        }
}

/*******************************************************/
void __fastcall StartEOTL3HeliRotors (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("finalec")
        && ScriptVehicleRandomizer::mTempVehHandle != -1)
        {
            if (ScriptParams[0] == 8000)
                Scrpt::CallOpcode (0x825, "start_rotors_instantly",
                                   ScriptVehicleRandomizer::mTempVehHandle);
        }
}

/*******************************************************/
void __fastcall SlowDownEOTL3FireTruckAtCatch (CRunningScript *scr, void *edx,
                                               short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("finalec"))
        {
            int playerCar
                = ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ();
            if (CModelInfo::IsHeliModel (playerCar) || playerCar == 408
                || playerCar == 414 || playerCar == 418 || playerCar == 423
                || playerCar == 428 || playerCar == 431 || playerCar == 443
                || playerCar == 455 || playerCar == 456 || playerCar == 457
                || playerCar == 478 || playerCar == 498 || playerCar == 499
                || playerCar == 508 || playerCar == 524 || playerCar == 578
                || playerCar == 588 || playerCar == 609)
                {
                    ScriptVehicleRandomizer::mEOTL3Slow = true;
                }
        }
}

/*******************************************************/
void __fastcall SpeedUpEOTL3FireTruckAfterCatch (CRunningScript *scr, void *edx,
                                                 short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("finalec") && ScriptParams[1] == 1)
        ScriptVehicleRandomizer::mEOTL3Slow = false;
}

/*******************************************************/
void __fastcall OverrideVigilanteCheck (CRunningScript *scr, void *edx,
                                        char flag)
{
    if (scr->CheckName ("copcar"))
        {
            if (FindPlayerVehicle ())
                {
                    int validVehicle = false;

                    Scrpt::CallOpcode (0x3c0, "store_car_char_is_in",
                                       GlobalVar (3), GlobalVar (9005));
                    if (GetGlobalVar<int> (9002) == GetGlobalVar<int> (9005)
                        || GetGlobalVar<int> (9003) == GetGlobalVar<int> (9005)
                        || GetGlobalVar<int> (9004) == GetGlobalVar<int> (9005))
                        {
                            validVehicle = true;
                            flag         = true;
                        }

                    if ((flag && !validVehicle)
                        || (FindPlayerVehicle ()->m_nModelIndex == 425
                            && !validVehicle))
                        ChangePlayerVehicle (124);
                }
        }
    scr->UpdateCompareFlag (flag);
}

/*******************************************************/
void
ScriptVehicleRandomizer::Initialise ()
{
    RegisterHooks (
        {{HOOK_CALL, 0x486DB1, (void *) &MoveFlyingSchoolTrigger},
         {HOOK_CALL, 0x47F688, (void *) &Ryder2IsCutsceneActive},
         {HOOK_CALL, 0x53BCD9, (void *) &InitialiseCacheForRandomization}});

    if (!ConfigManager::ReadConfig (
            "ScriptVehicleRandomizer",
            std::pair ("EnableExtraTimeForSchools",
                       &m_Config.MoreSchoolTestTime),
            std::pair ("RandomizeTrains", &m_Config.RandomizeTrains),

            std::pair ("LowriderMissions", &m_Config.SkipLowriderCheck),
            std::pair ("WuZiMu", &m_Config.SkipWuZiMuCheck),
            std::pair ("SweetsGirl", &m_Config.SkipSweetsGirlCheck),
            std::pair ("CourierMissions", &m_Config.SkipCourierCheck),
            std::pair ("BMXChallenge", &m_Config.SkipBMXChallengeCheck),
            std::pair ("NRG500Challenge", &m_Config.SkipNRGChallengeCheck),
            std::pair ("ChiliadChallenge", &m_Config.SkipChiliadCheck),

            std::pair ("TaxiMissions", &m_Config.Taxi),
            std::pair ("Firefighting", &m_Config.Firefighting),
            std::pair ("Vigilante", &m_Config.Vigilante),
            std::pair ("Burglary", &m_Config.Burglary),
            std::pair ("Pimping", &m_Config.Pimping),
            std::pair ("Paramedic", &m_Config.Paramedic),
            std::pair ("Courier", &m_Config.Courier),
            std::pair ("BikeChallenges", &m_Config.Bike),

            std::pair ("SlowPlanesInAirRaid", &m_Config.OldAirRaid),

            std::pair ("ForcedVehicleId", &m_Config.ForcedVehicle),
            std::pair ("UseGenericPatterns", &m_Config.GenericPatterns),
            std::pair ("SkipChecks", &m_Config.SkipChecks)))
        return;

    TrafficRandomizer::GetInstance ()->MakeRCsEnterable ();

    RegisterHooks (
        {{HOOK_CALL, 0x467B01, (void *) &RandomizeCarForScript},
         {HOOK_CALL, 0x48AAB8, (void *) &RandomizeRCVehicleForScript},

         // Change speed of vehicle recordings
         {HOOK_CALL, 0x498AA8, (void *) &SlowDownAndromedaInStoaway},

         {HOOK_CALL, 0x47F070, (void *) &RevertVehFixes},
         {HOOK_CALL, 0x5DFE79, (void *) &FixEOTLPosition},
         {HOOK_CALL, 0x469612, (void *) &FixCarChecks},
         {HOOK_CALL, 0x47C12B, (void *) &FixCarChecks},
         {HOOK_CALL, 0x4958C6, (void *) &PopDoorFix},
         {HOOK_CALL, 0x495B38, (void *) &PopPanelFix},
         {HOOK_CALL, 0x48C1FA, (void *) &PopBootFix},
         {HOOK_CALL, 0x495902, (void *) &FixDoorFix},
         {HOOK_CALL, 0x495B74, (void *) &FixPanelFix},
         {HOOK_CALL, 0x4985DA, (void *) &VehicleUpdateFix},
         {HOOK_CALL, 0x49128C, (void *) &FixGTAMadman},
         {HOOK_CALL, 0x475BBC, (void *) &FixGearUp},

         // Move locations of triggers offset from a vehicle
         {HOOK_CALL, 0x489835, (void *) &FixMaddDogg},

         // Move locations of objects attached to vehicle
         {HOOK_CALL, 0x495429, (void *) &FixMaddDoggBoxes},

         // T-Bone Mendez - can pick up packages from cars
         {HOOK_CALL, 0x48ABB0, (void *) &AlwaysPickUpPackagesTBone},

         // Change locations of actors attached to vehicles
         {HOOK_CALL, 0x48A7B8, (void *) &FixHeightInDrugs4Auto},

         // Skip Pilot School landing gear requirement for vehicles without gear
         {HOOK_CALL, 0x476BCB, (void *) &IgnoreLandingGearCheck},

         // When player in vehicle, car fly / / activate RC Heli magnet
         {HOOK_CALL, 0x469602, (void *) &IsPlayerInVehicleCheck},
         {HOOK_CALL, 0x48A0F6, (void *) &IsPlayerInVehicleCheck},

         // Additional flying car hooks for weird missions
         {HOOK_CALL, 0x48551A, (void *) &FlyingCarsForVB},
         {HOOK_CALL, 0x46C1AB, (void *) &FlyingCarsForFlightSchool},
         {HOOK_CALL, 0x48C02F, (void *) &WaterCarsForBoatSchool},
         {HOOK_CALL, 0x47F042, (void *) &ResetFlyingCar},
         {HOOK_CALL, 0x468E9A, (void *) &InitialiseExtraText},
         {HOOK_CALL, 0x618E97, (void *) &InitialiseExtraText},
         {HOOK_CALL, 0x5BA167, (void *) &InitialiseExtraText},

         // Makes SAM sites attack cars when using flying cars
         {HOOK_CALL, 0x48B33B, (void *) &ActivateSAMForFlyingCars},
         {HOOK_CALL, 0x5A0846, (void *) &ActivateSAMForFlyingCars},
         {HOOK_CALL, 0x5A085E, (void *) &ActivateSAMForFlyingCars},

         // Allows vehicles other than forklift to raise door in Breaking the Bank
         {HOOK_CALL, 0x47884F, (void *) &FixRaiseDoorHeist9},
         {HOOK_CALL, 0x47CAAD, (void *) &FixRaiseDoorHeist9PosCheck},

         // Allows hooking any vehicles for Tanker Commander / Highjack / Trucking
         {HOOK_CALL, 0x46C2A3, (void *) &TrailerAttachmentCheck},
         {HOOK_CALL, 0x4720BF, (void *) &HasTrailerForceAttached},

         // Fixes cars going inside the floor after Cesar jumps in Highjack
         {HOOK_CALL, 0x49557F, (void *) &FixToreno2},

         // Hooks for heli magnet sections in Quarry / Robbing Uncle Sam
         // Mostly responsible for creating magnet object and tracking boxes / barrels you pick up
         {HOOK_CALL, 0x47CC68, (void *) &QuarryAttachMagnet},
         {HOOK_CALL, 0x487D29, (void *) &Quarry2StoreBarrelHandles},
         {HOOK_CALL, 0x485067, (void *) &Quarry6StoreBarrelHandles},
         {HOOK_CALL, 0x47F90C, (void *) &Ryder2AttachMagnet},
         {HOOK_CALL, 0x47CCF2, (void *) &Ryder2StoreBoxHandles},
         {HOOK_CALL, 0x469773, (void *) &Ryder2IncreaseRadius},
         {HOOK_CALL, 0x4831FC, (void *) &Ryder2CheckBoxForDamage},
         {HOOK_CALL, 0x48322F, (void *) &Ryder2CheckBoxForDamage2},
         {HOOK_CALL, 0x4698C6, (void *) &Ryder2WasBoxDestroyedSomehow},
         {HOOK_CALL, 0x483584, (void *) &Quarry2RemoveSafeBarrel},
         {HOOK_CALL, 0x48563A, (void *) &Ryder2ReplaceHelp},

         // Resets variables and removes references on mission end
         {HOOK_CALL, 0x4685A8, (void *) &ResetEndOfMissionStuff},
         
         // Resets variables on mission start + checks for vehicle sub-mission
         {HOOK_CALL, 0x489955, (void *) &SetThingsForMissionStart},

         // Fixes misplaced buoys in Boat School
         {HOOK_CALL, 0x482C6B, (void *) &FixBoatSchoolObjectPlacements},

         // Adds failsafe to cutscenes of entering garage for large vehicles (e.g. Dumper)
         {HOOK_CALL, 0x49220E, (void *) &FixStuckAtDohertyGarage},

         // Allows non-taxis to do the Taxi sub-mission
         {HOOK_CALL, 0x4912E8, (void *) &OverrideTaxiCheck},

         // Moves vehicles in T-Bone Mendez / Local Liquor Store so they aren't
         // constantly stuck in each other (so as to add more variety)
         {HOOK_CALL, 0x467DB9, (void *) &MoveLargeCarsApart},

         // Reduces weight of car in Up Up and Away so it can be more easily carried
         {HOOK_CALL, 0x48954E, (void *) &ReduceCarWeight},

         // Checks for heli you protect in Interdiction being destroyed
         // Prevents the player from dying if they happen to be in the vehicle when it is
         {HOOK_CALL, 0x467B1E, (void *) &DetectVehicleDestruction},

         // Stops heli you protect in Interdiction from driving off the cliff if
         // it becomes a car
         {HOOK_CALL, 0x469B71, (void *) &CheckDes3HeliDriver},

         // Hooks for self-destruct feature in New Model Army
         // Stores player's vehicle, adds code to blow it up, and adds message
         {HOOK_CALL, 0x46D549, (void *) &GetCurrentPlayerRCVeh},
         {HOOK_CALL, 0x487A50, (void *) &ActivateZero4SelfDestruct},
         {HOOK_CALL, 0x468137, (void *) &ReplaceMessageText},

         // Makes Zero immune to collisions in Air Raid so planes don't crush him
         {HOOK_CALL, 0x47F893, (void *) &AddZero1Immunities},

         // Fixes some cars getting stuck in the floor in Key To Her Heart
         {HOOK_CALL, 0x47CB48, (void *) &FixStuckVehicles},

         // Increases radius of trigger to enter Stowaway plane for larger vehicles
         {HOOK_CALL, 0x48774C, (void *) &IncreaseStowawayRadius},

         // Instantly starts rotors if you get a heli in EOTL3 so you can catch the truck
         {HOOK_CALL, 0x46817E, (void *) &StartEOTL3HeliRotors},

         // Hooks to track when the sequence of catching Sweet begins / completes.
         // This is used to slow the vehicle recording down at that point so
         // slow vehicles are able to catch him.
         {HOOK_CALL, 0x484CC0, (void *) &SlowDownEOTL3FireTruckAtCatch},
         {HOOK_CALL, 0x4953A1, (void *) &SpeedUpEOTL3FireTruckAfterCatch},

         // For every vehicle spawned, saves the thread it came from to a variable.
         {HOOK_CALL, 0x467AB7, (void *) &::UpdateLastThread}});

    // Hooks function for plane / heli height checks
    // Used in New Model Army so you can use regular helis
    for (int address : {0x6D8A2F, 0x6D97B9, 0x6D97B9, 0x6D9813, 0x6D9E33,
                        0x6D9E56, 0x6CC234, 0x6CC234, 0x6CC2A0})
        injector::MakeCALL (address, (void *) &IgnoreHeightLimit);

    // Numerous hooks for increasing times for the four schools so slow vehicles can do the tests
    // Also moves corona in Barrel Roll / Loop-The-Loop to give more room for slow vehicles
    if (m_Config.MoreSchoolTestTime)
        RegisterHooks (
            {{HOOK_CALL, 0x48CFA9, (void *) &ProcessModifiedSchoolTimes},
             {HOOK_CALL, 0x48A664, (void *) &DisplayCorrectSchoolTime},
             {HOOK_CALL, 0x47D3CC, (void *) &ProcessModifiedSchoolTimesFlying},
             {HOOK_CALL, 0x475DE8, (void *) &DisplayCorrectFlyingSchoolTime},
             {HOOK_CALL, 0x48B904, (void *) &ProcessModifiedSchoolTimesBoat},
             {HOOK_CALL, 0x4730F8, (void *) &DisplayCorrectBoatTimePart1},
             {HOOK_CALL, 0x473140, (void *) &DisplayCorrectBoatTimePart2},
             {HOOK_CALL, 0x481A0C, (void *) &DisplayCorrectBoatTimeOnTV},
             {HOOK_CALL, 0x497ED7, (void *) &MoveFlyingSchoolCorona},
             {HOOK_CALL, 0x47CFE0, (void *) &MoveFlyingSchoolBlip}});

    // Randomizes train into one of small length or less to the original
    // If train is smaller, anything that would have applied to a now non-existent
    // carriage moves to the next available existing one.
    if (m_Config.RandomizeTrains)
        RegisterHooks (
            {{HOOK_CALL, 0x497F89, (void *) &RandomizeTrainForScript},
             {HOOK_CALL, 0x46BB6C, (void *) &IgnoreTrainCarriages},
             {HOOK_CALL, 0x490558, (void *) &FixSnailTrailTrain},
             {HOOK_JUMP, 0x6F5E70, (void *) &ChangeLastTrainCarriage}});

    if (m_Config.SkipLowriderCheck)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x478528, (void *) &IgnoreLowriderCheck}});
        }

    if (m_Config.SkipWuZiMuCheck)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x48B0EB, (void *) &IgnoreWuZiMuCheck}});
        }

    if (m_Config.SkipSweetsGirlCheck)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x48B10F, (void *) &IgnoreSweetGirlCarCheck}});
        }

    if (m_Config.Vigilante)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x48DAA4, (void *) &OverrideVigilanteCheck}});
        }

    if (!ConfigManager::ReadConfig ("MissionRandomizer"))
        {
            RegisterHooks (
                {{HOOK_CALL, 0x489929, (void *) &StoreStartedMission}});
        }

    Logger::GetLogger ()->LogMessage ("Intialised ScriptVehicleRandomizer");
}

/*******************************************************/
void
ScriptVehicleRandomizer::InitialiseCache ()
{
    if (ConfigManager::ReadConfig ("ScriptVehicleRandomizer")
        || ConfigManager::ReadConfig ("ParkedCarRandomizer"))
        CacheSeats ();

    if (ConfigManager::ReadConfig ("ScriptVehicleRandomizer")
        && !m_Config.SkipChecks)
        {
            this->CachePatterns ();
            Logger::GetLogger ()->LogMessage (
                "Initialised Script Vehicles cache");
        }
}

/*******************************************************/
void
ScriptVehicleRandomizer::CachePatterns ()
{
    FILE *vehPatternsFile
        = OpenRainbomizerFile ("Vehicle_Patterns.txt", "r", "data/");
    if (vehPatternsFile)
        {
            char line[2048] = {0};
            while (fgets (line, 2048, vehPatternsFile))
                {
                    if (line[0] == '#' || strlen (line) < 10)
                        continue;

                    char threadName[64]  = {0};
                    char vehicleName[64] = {0};
                    char flags[256]      = {0};
                    int  seats           = 0;
                    char cars            = 'N';
                    char bikes           = 'N';
                    char bicycles        = 'N';
                    char quadbikes       = 'N';
                    char planes          = 'N';
                    char helicopters     = 'N';
                    char boats           = 'N';
                    char trains          = 'N';
                    char trailers        = 'N';

                    Vector3 altCoords = {0.0, 0.0, 0.0};

                    sscanf (line,
                            "%s %s %d %c %c %c %c %c %c %c %c %c %s "
                            "%f %f %f",
                            threadName, vehicleName, &seats, &cars, &bikes,
                            &bicycles, &quadbikes, &planes, &helicopters,
                            &boats, &trains, &trailers, flags, &altCoords.x,
                            &altCoords.y, &altCoords.z);

                    for (int i = 0; i < 64; i++)
                        {
                            threadName[i]  = NormaliseChar (threadName[i]);
                            vehicleName[i] = NormaliseChar (vehicleName[i]);
                        }

                    ScriptVehiclePattern pattern;
                    pattern.SetOriginalVehicle (vehicleName);
                    pattern.SetSeatsCheck (seats);
                    pattern.SetThreadName (threadName);

                    pattern.SetAllowedTypes (
                        {cars == 'Y', bikes == 'Y', bicycles == 'Y',
                         quadbikes == 'Y', planes == 'Y', helicopters == 'Y',
                         boats == 'Y', trains == 'Y', trailers == 'Y'});

                    pattern.SetMovedTypes (
                        {cars == 'C', bikes == 'C', bicycles == 'C',
                         quadbikes == 'C', planes == 'C', helicopters == 'C',
                         boats == 'C', trains == 'C', trailers == 'C'});

                    pattern.SetMovedCoordinates (altCoords);
                    pattern.ParseFlags (flags);

                    pattern.Cache ();

                    mPatterns.push_back (pattern);
                }
            Logger::GetLogger ()->LogMessage (
                "Cached Script Vehicle Patterns.");
        }
    else if (!vehPatternsFile)
        {
            // Log a message if file wasn't found
            Logger::GetLogger ()->LogMessage (
                "Failed to read file: rainbomizer/data/Vehicle_Patterns.txt");
            return;
        }
}

/*******************************************************/
void
ScriptVehicleRandomizer::DestroyInstance ()
{
    if (ScriptVehicleRandomizer::mInstance)
        delete ScriptVehicleRandomizer::mInstance;
}

/*******************************************************/
ScriptVehicleRandomizer *
ScriptVehicleRandomizer::GetInstance ()
{
    if (!ScriptVehicleRandomizer::mInstance)
        {
            ScriptVehicleRandomizer::mInstance = new ScriptVehicleRandomizer ();
            atexit (&ScriptVehicleRandomizer::DestroyInstance);
        }
    return ScriptVehicleRandomizer::mInstance;
}
