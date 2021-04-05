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
#include <unordered_map>
#include "scm_patterns.hh"
#include "util/scrpt.hh"
#include "injector/calling.hpp"
#include "util/text.hh"
#include <chrono>
#include "traffic.hh"

ScriptVehicleRandomizer *ScriptVehicleRandomizer::mInstance = nullptr;

const int MODEL_DUMPER   = 406;
const int MODEL_FIRELA = 0x220;
const int MODEL_PCJ      = 461;
const int MODEL_FAGGIO   = 462;
const int MODEL_FREEWAY  = 463;
const int MODEL_SANCHZ = 468;
const int MODEL_WALTON   = 478;
const int MODEL_BMX      = 481;
const int MODEL_GREENWOO = 492;
const int MODEL_BOXVILLE = 498;
const int MODEL_MTBIKE   = 510;
const int MODEL_HYDRA    = 520;
const int MODEL_NRG      = 522;
const int MODEL_CEMENT   = 524;
const int MODEL_FORKLIFT = 530;
const int MODEL_VORTEX   = 539;
const int MODEL_ANDROM   = 592;

const int waterCarsCheatActive  = 0x969130 + 34;
const int flyingCarsCheatActive = 0x969130 + 48;

static const struct ScriptVehicleRandomizer::RCHeliMagnet emptyTemplate;
static ScriptVehicleRandomizer::RCHeliMagnet ryder2;
static ScriptVehicleRandomizer::RCHeliMagnet quarry2;
static ScriptVehicleRandomizer::RCHeliMagnet quarry6;

static bool     ignoreNextHook            = false;  // For Flying School times
static bool     currentTrailerAttached = false;     // For attaching any vehicle in Trucking missions
static bool     wasFlyingCarsOn = false;            // Stores state of flying cars before starting a mission that forces them

// Store original train type + randomized train type for random trains
static int      lastTrainOldType       = -1;
static int      lastTrainNewType          = -1;
static std::vector<int> snailTrailTrain;

static int currentTextBoxForMission = 0;

static ScriptVehicleRandomizer::BoatSchoolTimes boatSchool;

// Key is traintype ID, value is number of carriages for that train type
// Used when randomizing trainType to get number of carriages of new train
// If new train has less carriages, need to ensure that now nonexistent carriages are ignored
std::unordered_map<int, int> trainTypes
    = {{0, 5}, {1, 3}, {2, 3},  {3, 4},  {4, 3},  {5, 4},  {6, 3},  {7, 3},
       {8, 2}, {9, 1}, {10, 2}, {11, 3}, {12, 5}, {13, 6}, {14, 1}, {15, 1}}; 

static float timerStartTime = -1.0f;
static float timerCurrent = -1.0f;

template <typename T>
T &
GetGlobalVar(uint32_t index)
{
    return *reinterpret_cast<T *> (&ScriptSpace[index]);
}

/*******************************************************/
void
SlowDownAndromedaInStoaway (uint8_t *vehicle, float speed)
{
    //uint16_t modelIndex = *reinterpret_cast<uint16_t *> (vehicle + 0x22);

    if (speed < 1.1 && speed > 0.9
        && ScriptVehicleRandomizer::GetInstance()->mLastThread == "desert9")
        speed = 0.7;

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
    if (ScriptVehicleRandomizer::GetInstance ()->GetIfPlayerInFlyingCar())
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
        || (scr->CheckName("cprace") && (ScriptSpace[352] >= 19 && ScriptSpace[352] <= 24)))
        DisableFlyingCars ();
    else if (scr->CheckName ("boat"))
        injector::WriteMemory (waterCarsCheatActive, 0);
}

/*******************************************************/
void __fastcall FlyingCarsForVB (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("mansio2"))
    {
        if (ScriptParams[0] < 0.5f)
            SetFlyingCar (true);
        else if (ScriptParams[0] > 0.5f)
            SetFlyingCar (false);
    }
}

/*******************************************************/
void __fastcall FlyingCarsForFlightSchool (CRunningScript *scr, void *edx, short count) 
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
    if (ScriptParams[1] == MODEL_SANCHZ // For Kickstart
        || (ScriptParams[1] == MODEL_FORKLIFT && scr->CheckName("heist9")) // For Breaking the Bank
        || ScriptParams[1] == MODEL_DUMPER) // For Explosive Situation
        {
            int newVeh
                = ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ();
            if (newVeh != -1)
                ScriptParams[1] = newVeh;
        }
    else if (ScriptParams[1] == MODEL_CEMENT && FindPlayerVehicle ()) // For Deconstruction
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (ScriptParams[1] == MODEL_HYDRA && FindPlayerVehicle ()) // For Vertical Bird
    {
        int playerVeh = FindPlayerVehicle ()->m_nModelIndex;
        if (CModelInfo::IsPlaneModel (playerVeh) || CModelInfo::IsHeliModel (playerVeh) 
            || CModelInfo::IsCarModel(playerVeh) || CModelInfo::IsQuadBikeModel(playerVeh) 
            || CModelInfo::IsMonsterTruckModel(playerVeh))
            {
                ScriptParams[1] = playerVeh;
            }
    }
    else if (scr->CheckName ("oddveh") && (ScriptParams[1] == MODEL_BMX 
        || ScriptParams[1] == MODEL_FREEWAY || ScriptParams[1] == MODEL_FAGGIO))
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_MTBIKE)
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (scr->CheckName ("oddveh") && ScriptParams[1] == MODEL_NRG)
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
ApplyFixesBasedOnModel (int model, int newModel)
{
    if (model == MODEL_FIRELA)
        ScriptVehicleRandomizer::GetInstance ()->ApplyEOTLFixes (newModel);
    else if ((model == MODEL_SANCHZ && ScriptVehicleRandomizer::GetInstance()->mLastThread == "kicksta") 
        || (model == MODEL_FORKLIFT && ScriptVehicleRandomizer::GetInstance()->mLastThread == "heist9")
        || (model == MODEL_DUMPER && ScriptVehicleRandomizer::GetInstance()->mLastThread == "casino2")
        || (model == MODEL_FORKLIFT && ScriptVehicleRandomizer::GetInstance()->mLastThread == "ryder2")
        || (model == MODEL_WALTON && ScriptVehicleRandomizer::GetInstance()->mLastThread == "doc2")
        || (model == MODEL_ANDROM && ScriptVehicleRandomizer::GetInstance ()->mLastThread == "desert9")
        || (model == MODEL_GREENWOO && ScriptVehicleRandomizer::GetInstance()->mLastThread == "drugs4")
        || (model == MODEL_VORTEX && ScriptVehicleRandomizer::GetInstance ()->mLastThread == "boat")
        || (model == MODEL_BOXVILLE && ScriptVehicleRandomizer::GetInstance ()->mLastThread == "guns1")
        || (model == MODEL_PCJ && ScriptVehicleRandomizer::GetInstance ()->mLastThread == "toreno2"))
            ScriptVehicleRandomizer::GetInstance ()->ApplyCarCheckFix (newModel);
        
}

/*******************************************************/
void
ApplyFixesBasedOnMission ()
{
    currentTrailerAttached = false;
    timerStartTime         = -1.0f;
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "sweet6")
            Scrpt::CallOpcode (0x8, "add_int", GlobalVar (1920), 1);

    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "dskool" && 
        ScriptVehicleRandomizer::m_Config.MoreSchoolTestTime)
    {
        int currentTest = ScriptSpace[247];
        if (currentTest == 1)
                ScriptSpace[253] = 16000; // Timer value
        else if (currentTest == 2)
            ScriptSpace[253] = 26000;
        else if (currentTest == 3 || currentTest == 4)
            ScriptSpace[253] = 14000;
        else if (currentTest == 5)
                ScriptSpace[253] = 15000;
        else if (currentTest == 8 || currentTest == 9)
                ScriptSpace[253] = 31000;
        else if (currentTest == 10)
                ScriptSpace[253] = 12000;
        else if (currentTest == 12 || currentTest == 13)
                ScriptSpace[253] = 14000;
    }   

    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "desert5")
            ignoreNextHook = false;
}

/*******************************************************/
void *
RandomizeCarForScript (int model, float x, float y, float z, bool createdBy)
{
    int newModel = ScriptVehicleRandomizer::GetInstance
                           ()->ProcessVehicleChange (model,
                                                                         x, y,
                                                                         z);
    Logger::GetLogger ()->LogMessage ("Vehicle Spawned: "
                                      + std::to_string (newModel));
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "quarry"
        && (ScriptSpace[8171] == 1 || ScriptSpace[8171] == 5) && model == 486)
        newModel = 486;

    ApplyFixesBasedOnModel (model, newModel);
    ApplyFixesBasedOnMission ();

    // Load the new vehicle. Fallback to the original if needed
    if (StreamingManager::AttemptToLoadVehicle (newModel) == ERR_FAILED)
        newModel = model;

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
RandomizeRCVehicleForScript (float x, float y, float z, float angle, short model)
{
    int newModel
        = ScriptVehicleRandomizer::GetInstance ()->ProcessVehicleChange (model,
                                                                         x, y,
                                                                         z);

    ApplyFixesBasedOnModel (model, newModel);
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
    ScriptVehicleRandomizer::GetInstance ()->UpdateLastThread (
        script->m_szName);
    return 0;
}

/*******************************************************/
int
ScriptVehicleRandomizer::ProcessVehicleChange (int id, float &x, float &y,
                                               float &z)
{
    Vector3 pos = {x, y, z};
    for (auto &pattern : mPatterns)
        {
            if (pattern.MatchVehicle(id, mLastThread, pos))
                {
                    int newVehID = pattern.GetRandom (pos);
                    x            = pos.x;
                    y            = pos.y;
                    z            = pos.z;
                    return newVehID;
                }
        }
    return random(400, 611);
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
    if (!CheckForCAutomobile (vehicle))
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
void __fastcall ProcessModifiedSchoolTimes (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("dskool"))
    {
        ScriptVehicleRandomizer::GetInstance ()->SaveTestTime (ScriptSpace[253]);
        if (ScriptSpace[247] == 6 || ScriptSpace[247] == 7)
            ScriptSpace[253] -= 20000;
        else if (ScriptSpace[247] == 16)
            ScriptSpace[253] -= 120000;
    }
}

/*******************************************************/
void __fastcall ProcessModifiedSchoolTimesFlying (CRunningScript *scr, void *edx, short count)
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
void __fastcall ProcessModifiedSchoolTimesBoat (CRunningScript *scr,
                                                  void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat") && ScriptSpace[8189] != 4)
    {
        int currentTest = ScriptSpace[8189];
        ScriptVehicleRandomizer::GetInstance ()->SaveTestTime (ScriptSpace[8197]);
            Scrpt::CallOpcode (0x14F, "stop_timer", GlobalVar (8197));
            Scrpt::CallOpcode (0x396, "pause_timer", 1);
            boatSchool.oldRecord = -1;
            boatSchool.newRecordAchieved = true;
            if (currentTest == 1)
            {
                boatSchool.oldRecord = ScriptSpace[1963];
                ScriptSpace[8197] -= 18000;
            }
            else if (currentTest == 2)
            {
                boatSchool.oldRecord = ScriptSpace[1964];
                ScriptSpace[8197] -= 50000;
            }
            else if (currentTest == 3)
            {
                boatSchool.oldRecord = ScriptSpace[1965];
                ScriptSpace[8197] -= 120000;
            }
            else if (currentTest == 5)
            {
                boatSchool.oldRecord = ScriptSpace[1967];
                ScriptSpace[8197] -= 120000;
            }
    }
}

/*******************************************************/
void __fastcall DisplayCorrectSchoolTime (CRunningScript *scr, void *edx,
                                            short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("dskool") && ScriptSpace[247] == 6 
        || ScriptSpace[247] == 7 || ScriptSpace[247] == 16)
    {
        int actualTime
                = ScriptVehicleRandomizer::GetInstance ()->GetOriginalTestTime();
        int seconds = actualTime / 1000;
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
        int seconds = actualTime / 1000;
        ScriptSpace[301] = seconds;
    }
}

/*******************************************************/
void __fastcall DisplayCorrectBoatTimeOnTV (CRunningScript *scr, void *edx,
                                             short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat") && ScriptSpace[8189] != 4)
    {
        if (((float *) ScriptParams)[0] >= 321.0f && ((float *) ScriptParams)[0] <= 323.0f 
                && ((float *) ScriptParams)[1] >= 30.0f && ((float *) ScriptParams)[1] <= 32.0f)
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
                    ScriptSpace[1964] -= 50000;
                }
                else if (currentTest == 3 && ScriptSpace[1965] != 180000) 
                {
                    boatSchool.tempActualTime = ScriptSpace[1965];
                    ScriptSpace[1965] -= 120000;
                }
                else if (currentTest == 5 && ScriptSpace[1967] != 200000) 
                {
                    boatSchool.tempActualTime = ScriptSpace[1967];
                    ScriptSpace[1967] -= 120000;
                }
        }
        else if ((((float *) ScriptParams)[0] >= 322.0f
            && ((float *) ScriptParams)[0] <= 324.0f
            && ((float *) ScriptParams)[1] >= 109.0f
             && ((float *) ScriptParams)[1] <= 111.0f)
            || (((float *) ScriptParams)[0] >= 259.0f
                   && ((float *) ScriptParams)[0] <= 261.0f
                   && ((float *) ScriptParams)[1] >= 64.0f
                   && ((float *) ScriptParams)[1] <= 66.0f))
        {
                if (!boatSchool.newRecordAchieved)
                    ((float *) ScriptParams)[1] = -100.0f;
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
                boatSchool.type = 3;
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
                else if (currentTest == 1 && actualTime >= boatSchool.oldRecord)
                {
                    boatSchool.newRecordAchieved = false;
                    ScriptSpace[8195] = 0;
                    ScriptSpace[1963] = boatSchool.oldRecord;
                }
                else if (currentTest == 2 && actualTime < boatSchool.oldRecord)
                    ScriptSpace[1964] = actualTime;
                else if (currentTest == 2 && actualTime >= boatSchool.oldRecord)
                {
                    boatSchool.newRecordAchieved = false;
                    ScriptSpace[8195] = 0;
                    ScriptSpace[1964] = boatSchool.oldRecord;
                }
                else if (currentTest == 3 && actualTime < boatSchool.oldRecord)
                    ScriptSpace[1965] = actualTime;
                else if (currentTest == 3 && actualTime >= boatSchool.oldRecord)
                {
                    boatSchool.newRecordAchieved = false;
                    ScriptSpace[8195] = 0;
                    ScriptSpace[1965] = boatSchool.oldRecord;
                }
                else if (currentTest == 5 && actualTime < boatSchool.oldRecord)
                    ScriptSpace[1967] = actualTime;
                else if (currentTest == 5 && actualTime >= boatSchool.oldRecord)
                {
                    boatSchool.newRecordAchieved = false;
                    ScriptSpace[8195] = 0;
                    ScriptSpace[1967] = boatSchool.oldRecord;
                }
                boatSchool.overallTime = (float) actualTime / 1000.0f;
                boatSchool.type = 2;
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
                        ((float *) ScriptParams)[0] += 50.0f;
                    else if (currentTest == 3)
                        ((float *) ScriptParams)[0] += 2.0f;
                    else if (currentTest == 5)
                        ((float *) ScriptParams)[0] += 2.0f;
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
                            boatSchool.damage           = 0;
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
int __fastcall AlwaysPickUpPackagesTBone(CVehicle* currentVeh, void* edx) 
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "driv2")
        return 2;
    else
        return CallMethodAndReturn<int, 0x6D1080> (currentVeh);
}

/*******************************************************/
int
InitialiseCacheForRandomization (void *fileName)
{
    Logger::GetLogger ()->LogMessage ("SCM Cache Hook");
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
    else if (scr->CheckName ("toreno2")
             && !CModelInfo::IsBikeModel(ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()) 
        && !CModelInfo::IsQuadBikeModel(ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()))
    {
        if (((float *) ScriptParams)[3] > -11.0f
                && ((float *) ScriptParams)[3] < -9.0f)
            {
                ((float *) ScriptParams)[1] -= 5.0f;
                ((float *) ScriptParams)[2] -= 4.0f;
            }
    }
}

/*******************************************************/
void __fastcall FixMaddDoggBoxes (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("doc2") 
        && ScriptVehicleRandomizer::GetInstance()->GetNewCarForCheck() != MODEL_WALTON)
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
            ((float *) ScriptParams)[3]   = ms_modelInfoPtrs[vehicle->m_nModelIndex]
                                  ->m_pColModel->m_boundBox.m_vecMin.y;
        }
    else if (scr->CheckName ("des10"))
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            if (lastTrainNewType != 0 && lastTrainNewType != 3 && lastTrainNewType != 6 
                && lastTrainNewType != 10 && lastTrainNewType != 12 && lastTrainNewType != 13)
                SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
        }
    //else if (scr->CheckName("ryder3"))
    //{
    //        if (((float *) ScriptParams)[2] < 1.0f
    //            && ((float *) ScriptParams)[3] < 1.0f
    //            && ((float *) ScriptParams)[4] < 1.0f
    //            && ((float *) ScriptParams)[5] < 1.0f
    //            && ((float *) ScriptParams)[6] < 1.0f
    //            && ((float *) ScriptParams)[7] < 1.0f)
    //            ((float *) ScriptParams)[4] -= 50.0f;
    //}
}

/*******************************************************/
void __fastcall FixHeightInDrugs4Auto (CRunningScript *scr, void *edx,
                                       short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("drugs4")
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_GREENWOO
        && ScriptParams[0] == ScriptSpace[3])
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0.2);
        }
    else if (scr->CheckName ("des10"))
    {
        CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
            + 0xA18 * (ScriptParams[1] >> 8));
        if (lastTrainNewType != 0 && lastTrainNewType != 3
                && lastTrainNewType != 6 && lastTrainNewType != 10
                && lastTrainNewType != 12 && lastTrainNewType != 13)
                SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
    }
    else if (scr->CheckName ("ryder3"))
    {
        if (ScriptParams[0] != GetGlobalVar<int>(3))
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                + 0xA18 * (ScriptParams[1] >> 8));
            SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
        }
    }
}

/*******************************************************/
int
MoveFlyingSchoolCorona (float x, float y, float z, float dirX, float dirY, float dirZ, float radius, int type)
{
    if (ScriptVehicleRandomizer::GetInstance()->mLastThread == "desert5")
    {
            if (int (x) == 377 && int (z) == 80)
                x += 300.0f;
    }
    return CallAndReturn<int, 0x4935A0> (x, y, z, dirX, dirY, dirZ, radius,
                                         type);
}

/*******************************************************/
int
MoveFlyingSchoolBlip (int blipType, float x, float y, float z, int a5, int display)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "desert5")
    {
            if (int (x) == 377 && int (z) == 80)
                x += 300.0f;
    }
    return CallAndReturn<int, 0x583820> (blipType, x, y, z, a5, display);
}

/*******************************************************/
void __fastcall MoveFlyingSchoolTrigger (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("desert5"))
    {
        float coordCompareX = ((float *) ScriptParams)[1];
        float coordCompareZ = ((float *) ScriptParams)[3];
        if (int (coordCompareX) == 377 && int (coordCompareZ) == 80)
            ((float *) ScriptParams)[1] += 300.0f;
    }
    else if (scr->CheckName ("guns1")
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != 431
             && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
                    != 437)
    {
        float xRadius = ((float *) ScriptParams)[4];
        float yRadius = ((float *) ScriptParams)[5];
        float zRadius = ((float *) ScriptParams)[6];
        if (int (xRadius) == 2 && int (yRadius) == 3 && int (zRadius) == 5)
        {
            ((float *) ScriptParams)[5] += 2.0f;
        }
    }
}

/*******************************************************/
void __fastcall IgnoreLandingGearCheck (CRunningScript *scr, void *edx, int a2, short count)
{
    ScriptParams[0] = 1;
    CallMethod<0x464370> (scr, a2, count);
}

/*******************************************************/
void __fastcall IgnoreLowriderCheck (CRunningScript *scr, void *edx, char result)
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
    if (scr->CheckName("bcesar"))
    {
        GetGlobalVar<int> (49) = 0;
    }
}

/*******************************************************/
void __fastcall IgnoreSweetGirlCarCheck (CRunningScript *scr, void *edx, short count)
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
int __fastcall ActivateSAMForFlyingCars(CVehicle* veh, void* edx) 
{
    if (ScriptVehicleRandomizer::GetInstance ()->GetIfPlayerInFlyingCar ())
        return 5;
    else
        return CallMethodAndReturn<int, 0x6D1080> (veh); // Gets vehicle type as normal
}

/*******************************************************/
void __fastcall InitialiseExtraText (CText *text, void *edx, char a2)
{
    text->Load (a2);
    GxtManager::Initialise (text);
}

/*******************************************************/
void __fastcall Ryder2AttachMagnet (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2") && 
        ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck () != MODEL_FORKLIFT)
    {
            if (ScriptParams[1] == 1 && ScriptParams[2] == 0 && ScriptParams[3] == 0
            && ScriptParams[4] == 0 && ScriptParams[5] == 0)
            {
                ryder2.vehID = ScriptParams[0];
                Scrpt::CallOpcode (0x107, "create_object", 3056, 2791.426f,
                                   -2418.577f, 6.66f, GlobalVar (10947));
                Scrpt::CallOpcode (0x681, "attach_object", GlobalVar (10947), ryder2.vehID,
                                   0.0f, 0.0f, -0.2f, 0.0f, 0.0f, 0.0f);
                Scrpt::CallOpcode (0x382, "set_object_collision",
                                   GlobalVar (10947), 0);
            }
    }
}

/*******************************************************/
void
RCHeliPlayerJustEntered ()
{
    ryder2.isPlayerInVeh = true;
    CVector playerPos    = FindPlayerCoors ();
    playerPos.z += 1.0f;
    CRunningScript::SetCharCoordinates (FindPlayerPed (),
                                        {playerPos.x, playerPos.y, playerPos.z},
                                        1, 1);
    Scrpt::CallOpcode (0x382, "set_object_collision", GlobalVar (10947), 1);
    Logger::GetLogger ()->LogMessage ("Moved heli, set magnet collision");
    if (ryder2.objectAttached != -1)
        {
            Scrpt::CallOpcode (0x382, "set_object_collision",
                               ryder2.objHandles[ryder2.objectAttached], 1);
        }
    Scrpt::CallOpcode (0x825, "start_rotors_instantly", ryder2.vehID);
}

/*******************************************************/
void
RCHeliCheckForObjects ()
{
    if (ryder2.fakeColHandle != ryder2.invalidHandle
        && ryder2.fakeColHandle != 0)
        Scrpt::CallOpcode (0x382, "set_object_collision", ryder2.fakeColHandle,
                           0);
    Scrpt::CallOpcode (0x407, "get_offset_from_car_in_world", ryder2.vehID,
                       0.0f, 0.0f, -1.0f, GlobalVar (69), GlobalVar (70),
                       GlobalVar (71));
    CVector playerPos    = FindPlayerCoors ();
    short   isKeyPressed = CallAndReturn<short, 0x485B10> (0, 17);
    if (isKeyPressed == 255 && ryder2.objectAttached == -1
        && ryder2.pickUpObjectTimer > 50)
        {
            for (int i = 0; i < 17; i++)
                {
                    if (ryder2.objHandles[i] != 0)
                        {
                            Scrpt::CallOpcode (0x1bb, "get_object_coords",
                                               ryder2.objHandles[i],
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
                                    Scrpt::CallOpcode (0x681, "attach_object",
                                                       ryder2.objHandles[i],
                                                       ryder2.vehID, 0.0f, 0.0f,
                                                       -1.4f, 0.0f, 0.0f, 0.0f);
                                    Scrpt::CallOpcode (0x97b,
                                                       "play_audio_at_object",
                                                       ryder2.objHandles[i],
                                                       1009);
                                    CVector playerPos = FindPlayerCoors ();
                                    playerPos.z += 0.3f;
                                    CRunningScript::SetCharCoordinates (
                                        FindPlayerPed (),
                                        {playerPos.x, playerPos.y, playerPos.z},
                                        1, 1);
                                    ryder2.objectAttached    = i;
                                    ryder2.pickUpObjectTimer = 0;
                                    break;
                                }
                        }
                }
        }
    else if (isKeyPressed == 255 && ryder2.objectAttached > -1
             && ryder2.pickUpObjectTimer > 50)
        {
            Scrpt::CallOpcode (0x682, "detach_object",
                               ryder2.objHandles[ryder2.objectAttached], 0.0f,
                               0.0f, 0.0f, 1);
            ryder2.objectAttached    = -1;
            ryder2.pickUpObjectTimer = 0;
        }
}

/*******************************************************/
void
RCHeliPlayerJustExited ()
{
    ryder2.isPlayerInVeh = false;
    ryder2.invalidHandle = ryder2.fakeColHandle;
    Scrpt::CallOpcode (0x382, "set_object_collision", GlobalVar (10947), 0);
    if (ryder2.objectAttached != -1)
        {
            Scrpt::CallOpcode (0x382, "set_object_collision",
                               ryder2.objHandles[ryder2.objectAttached], 0);
        }
}

/*******************************************************/
void __fastcall IsPlayerInVehicleCheck (CRunningScript *scr, void *edx,
                                        char flag)
{
    if (scr->CheckName ("desert6") || scr->CheckName ("casino9") 
        || scr->CheckName ("casin10") || scr->CheckName ("heist2") 
        || (scr->CheckName ("cprace") && (ScriptSpace[352] >= 19 && ScriptSpace[352] <= 24)))
            SetFlyingCar (flag);
    else if (scr->CheckName ("ryder2") && ScriptParams[1] == ryder2.vehID
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ()
               != MODEL_FORKLIFT)
    {
            ryder2.pickUpObjectTimer++;

            if (flag && ryder2.isPlayerInVeh)
            {
                RCHeliCheckForObjects ();
            }
            else if (flag && !ryder2.isPlayerInVeh)
            {
                RCHeliPlayerJustEntered ();
            }
            else if (!flag && ryder2.isPlayerInVeh)
            {
                RCHeliPlayerJustExited ();
        }
    }
    scr->UpdateCompareFlag(flag);
}

/*******************************************************/
void __fastcall Ryder2IsCutsceneActive(CRunningScript* scr, void* edx, short count)
{
    scr->CollectParameters(count);
    if (scr->CheckName("ryder2"))
    {
        ryder2.isCutsceneActive = ScriptParams[0];
        if (ryder2.objectAttached > -1)
        {
            ryder2.objHandles[ryder2.objectAttached] = 0;
            ryder2.objectAttached = -1;
        }
    }
}

/*******************************************************/
void __fastcall Ryder2StoreBoxHandles(CRunningScript* scr, void* edx, short count)
{
    scr->CollectParameters(count);
    if (scr->CheckName ("ryder2"))
    {
            if (ryder2.currentObj == 17)
                {
                    ryder2.fakeColHandle = ScriptParams[0];
                    ryder2.currentObj    = 17;
                }

            if (ryder2.isCutsceneActive)
                {
                    ryder2.atMagnetSection            = true;
                    ryder2.objHandles[ryder2.currentObj] = ScriptParams[0];
                    ryder2.currentObj++;
                }
    }
}

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

/*******************************************************/
void Ryder2ReplaceHelp (char *str, bool quickMessage, bool permanent, bool addToBrief)
{
    if (ScriptVehicleRandomizer::GetInstance ()->mLastThread == "ryder2"
        && ryder2.isPlayerInVeh)
    {
        std::string newText;
        if (currentTextBoxForMission == 0)
        {
            newText = "Your helicopter is fitted with a magnet which can pick up objects.";
        }
        else if (currentTextBoxForMission == 1)
        {
            newText = "Press the Fire Button to pick up and drop the boxes.";
        }
        else if (currentTextBoxForMission == 2)
        {
            newText = "Once a box is attached, carefully place it in the back "
                      "of Ryder's vehicle.";
        }
        if (currentTextBoxForMission != 2)
            currentTextBoxForMission++;
        else
            currentTextBoxForMission = 0;
        Call<0x588BE0> (newText.c_str (), quickMessage, permanent, addToBrief);
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
        && ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck () != MODEL_FORKLIFT)
    {
        ((float *) ScriptParams)[4] = 3.0f;
        ((float *) ScriptParams)[6] = 2.75f;
    }
}

/*******************************************************/
void __fastcall Ryder2ForgetMagnet (CRunningScript *scr, void *edx,
                                        short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2") && 
        ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck () != MODEL_FORKLIFT && ryder2.atMagnetSection)
    {
        Scrpt::CallOpcode (0x1c4, "remove_references_to_object",
                           GlobalVar (10947)); // Stops magnet from persisting in save forever
    }
}

// Reset variables for RC box magnet section at start of Robbing Uncle Sam
/*******************************************************/
void __fastcall Ryder2ResetVariables (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("ryder2"))
    {
            ryder2 = emptyTemplate;
        Logger::GetLogger ()->LogMessage (
            "Test Default Values: " + std::to_string (ryder2.pickUpObjectTimer)
            + ", " + std::to_string (emptyTemplate.pickUpObjectTimer) + ", Object Attached: "
        + std::to_string(ryder2.objectAttached) + ", " + std::to_string(emptyTemplate.objectAttached));
    }
}

/*******************************************************/
void __fastcall FixRaiseDoorHeist9 (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("heist9")
        && (ScriptParams[1] == 1165 || ScriptParams[1] == 1166) 
        && ScriptVehicleRandomizer::GetInstance()->GetNewCarForCheck() != MODEL_FORKLIFT)
    {
            GetGlobalVar<float>(7937) = 1003.0f;
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
        if (playerPos.z >= 950.0f && playerPos.y >= 1570.7f && playerPos.y <= 1577.0f)
        {
            GetGlobalVar<float>(74) = 1573.0f;
        }
    }
}

/*******************************************************/
void __fastcall TrailerAttachmentCheck (CRunningScript *scr, void *edx,
                                        char flag)
{
    if (scr->CheckName ("cat3") || scr->CheckName("toreno2") || scr->CheckName("truck"))
    {
            if (!flag && !currentTrailerAttached && FindPlayerVehicle ())
                {
                    CVehicle *mainVehicle
                        = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                        + 0xA18 * (ScriptParams[0] >> 8));
                    CVehicle *trailerVehicle
                        = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                        + 0xA18 * (ScriptParams[1] >> 8));
                    Scrpt::CallOpcode (0x174, "get_car_heading",
                                       ScriptParams[0], GlobalVar (69));
                    Scrpt::CallOpcode (0x174, "get_car_heading",
                                       ScriptParams[1], GlobalVar (70));
                    Scrpt::CallOpcode (0x407, "get_offset_from_car_in_world", ScriptParams[0],
                        0.0f, -(ms_modelInfoPtrs[mainVehicle->m_nModelIndex]
                            ->m_pColModel->m_boundBox.m_vecMin.y), 0.0f,
                        GlobalVar (8924), GlobalVar (8925), GlobalVar (8926));
                    Scrpt::CallOpcode (
                        0x407, "get_offset_from_car_in_world", ScriptParams[1],
                        0.0f, -(ms_modelInfoPtrs[trailerVehicle->m_nModelIndex]
                            ->m_pColModel->m_boundBox.m_vecMax.y), 0.0f, 
                        GlobalVar (8927), GlobalVar (8928), GlobalVar (8929));
                    float angleCompare = GetGlobalVar<float> (69) - GetGlobalVar<float>(70);
                    float xCompare = GetGlobalVar<float>(8924) - GetGlobalVar<float>(8927);
                    float yCompare = GetGlobalVar<float> (8925) - GetGlobalVar<float>(8928);
                    float zCompare = GetGlobalVar<float> (8926) - GetGlobalVar<float>(8929);
                    if (angleCompare >= -10.0f && angleCompare <= 10.0f
                        && xCompare >= -1.0f && xCompare <= 1.0f
                        && yCompare >= -1.0f && yCompare <= 1.0f
                        && zCompare >= -2.5f && zCompare <= 2.5f)
                        {
                            Scrpt::CallOpcode (0x893, "attach_trailer",
                                               ScriptParams[0],
                                               ScriptParams[1]);
                            currentTrailerAttached = true;
                            flag                   = 1;
                        }
                }
            else if (!flag && currentTrailerAttached)
                currentTrailerAttached = false;
    }
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
void __fastcall FixToreno2 (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("toreno2"))
        ScriptParams[4] = 0.0f;
}

/*******************************************************/
void __fastcall FixBoatSchoolObjectPlacements (CRunningScript *scr, void *edx,
                                               short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("boat"))
    {
            int currentVehicle = ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ();
            if (ScriptSpace[8189] == 5
                     && !CModelInfo::IsCarModel (currentVehicle)
                     && !CModelInfo::IsQuadBikeModel (currentVehicle))
                ((float *) ScriptParams)[2] += 60.0f;
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
            while (lastTrainNewType == 1 || lastTrainNewType == 2
               || lastTrainNewType == 4 || lastTrainNewType == 5
               || lastTrainNewType == 7 || lastTrainNewType == 11)
        {
                lastTrainNewType = random (0, 15);
        }
    }
    eLoadError loadState1, loadState2 = ERR_LOADED;
    ScriptParams[0] = lastTrainNewType;
    Logger::GetLogger ()
        ->LogMessage ("Last Train Old Type: "
                          + std::to_string (lastTrainOldType));
    Logger::GetLogger ()->LogMessage ("Last Train New Type: "
                                      + std::to_string (lastTrainNewType));

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
        ScriptParams[0] = lastTrainOldType;
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
        if (snailTrailTrain.empty() || snailTrailTrain[0] != ScriptParams[0])
        {
            snailTrailTrain.clear ();
            snailTrailTrain.push_back (ScriptParams[0]);
            snailTrailTrain.push_back (lastTrainNewType);
            snailTrailTrain.push_back (lastTrainOldType);
        }
        else if (snailTrailTrain[0] == ScriptParams[0] && ScriptParams[1] == 2)
        {
            lastTrainNewType = snailTrailTrain[1];
            lastTrainOldType = snailTrailTrain[2];
            snailTrailTrain.clear ();
        }
    }
    int numOfCarriagesNew = trainTypes[lastTrainNewType];
    int numOfCarriagesOld = trainTypes[lastTrainOldType];
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

/*******************************************************/
void __fastcall FixSnailTrailTrain (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("scrash2"))
    {
        if (lastTrainNewType != 1 && lastTrainNewType != 2
                && lastTrainNewType != 4 && lastTrainNewType != 5
                && lastTrainNewType != 7 && lastTrainNewType != 11)
        Scrpt::CallOpcode (0x362, "remove_actor_from_car", ScriptParams[0], 843.377f, -1389.283f, -1.269f);
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
void __fastcall FixStuckAtDohertyGarage (CRunningScript *scr, void *edx,
                                         short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("scrash3"))
    {
        if (ScriptParams[0] == GetGlobalVar<int>(3))
        {
            if (timerStartTime < 0.0f)
            {
                timerStartTime = clock ();
            }
            timerCurrent = clock () - timerStartTime;
            if ((int)timerCurrent >= 6000)
            {
                CRunningScript::SetCharCoordinates (
                        FindPlayerPed (),
                        {-2047.5f, 178.5f, 27.8f}, 1, 1);
                timerStartTime = -1.0f;
            }
        }
    }
}

/*******************************************************/
void
ScriptVehicleRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("ScriptVehicleRandomizer", 
        std::pair ("EnableExtraTimeForSchools", &m_Config.MoreSchoolTestTime),
        std::pair ("LowriderMissions", &m_Config.SkipLowriderCheck),
        std::pair ("WuZiMu", &m_Config.SkipWuZiMuCheck),
        std::pair ("SweetsGirl", &m_Config.SkipSweetsGirlCheck),
        std::pair ("CourierMissions", &m_Config.SkipCourierCheck),
        std::pair("NRG500Challenge", &m_Config.SkipNRGChallengeCheck),
        std::pair("ChiliadChallenge", &m_Config.SkipChiliadCheck)))
    return;

    TrafficRandomizer::GetInstance ()->MakeRCsEnterable ();

    RegisterHooks (
        {{HOOK_CALL, 0x467B01, (void *) &RandomizeCarForScript},
         {HOOK_CALL, 0x48AAB8, (void *) &RandomizeRCVehicleForScript},
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
         {HOOK_CALL, 0x53BCD9, (void *) &InitialiseCacheForRandomization},
         {HOOK_CALL, 0x489835, (void *) &FixMaddDogg},
         {HOOK_CALL, 0x495429, (void *) &FixMaddDoggBoxes},
         {HOOK_CALL, 0x48ABB0, (void *) &AlwaysPickUpPackagesTBone},
         {HOOK_CALL, 0x48A7B8, (void *) &FixHeightInDrugs4Auto},
         {HOOK_CALL, 0x476BCB, (void *) &IgnoreLandingGearCheck},
         {HOOK_CALL, 0x469602, (void *) &IsPlayerInVehicleCheck},
         {HOOK_CALL, 0x48A0F6, (void *) &IsPlayerInVehicleCheck},
         {HOOK_CALL, 0x48551A, (void *) &FlyingCarsForVB},
         {HOOK_CALL, 0x46C1AB, (void *) &FlyingCarsForFlightSchool},
         {HOOK_CALL, 0x48C02F, (void *) &WaterCarsForBoatSchool},
         {HOOK_CALL, 0x47F042, (void *) &ResetFlyingCar},
         {HOOK_CALL, 0x468E9A, (void *) &InitialiseExtraText},
         {HOOK_CALL, 0x618E97, (void *) &InitialiseExtraText},
         {HOOK_CALL, 0x5BA167, (void *) &InitialiseExtraText},
         {HOOK_CALL, 0x48B33B, (void *) &ActivateSAMForFlyingCars},
         {HOOK_CALL, 0x5A0846, (void *) &ActivateSAMForFlyingCars},
         {HOOK_CALL, 0x5A085E, (void *) &ActivateSAMForFlyingCars},
         {HOOK_CALL, 0x47884F, (void *) &FixRaiseDoorHeist9},
         {HOOK_CALL, 0x47CAAD, (void *) &FixRaiseDoorHeist9PosCheck},
         {HOOK_CALL, 0x46C2A3, (void *) &TrailerAttachmentCheck},
         {HOOK_CALL, 0x4720BF, (void *) &HasTrailerForceAttached},
         {HOOK_CALL, 0x49557F, (void *) &FixToreno2},
         {HOOK_CALL, 0x47F90C, (void *) &Ryder2AttachMagnet},
         {HOOK_CALL, 0x47F688, (void *) &Ryder2IsCutsceneActive},
         {HOOK_CALL, 0x47CCF2, (void *) &Ryder2StoreBoxHandles},
         {HOOK_CALL, 0x469773, (void *) &Ryder2IncreaseRadius},
         {HOOK_CALL, 0x46DD99, (void *) &Ryder2ForgetMagnet},
         {HOOK_CALL, 0x4831FC, (void *) &Ryder2CheckBoxForDamage},
         {HOOK_CALL, 0x48322F, (void *) &Ryder2CheckBoxForDamage2},
         {HOOK_CALL, 0x4698C6, (void *) &Ryder2WasBoxDestroyedSomehow},
         {HOOK_CALL, 0x48563A, (void *) &Ryder2ReplaceHelp},
         {HOOK_CALL, 0x47C233, (void *) &Ryder2ResetVariables},
         {HOOK_CALL, 0x482C6B, (void *) &FixBoatSchoolObjectPlacements},
         {HOOK_CALL, 0x497F89, (void *) &RandomizeTrainForScript},
         {HOOK_CALL, 0x46BB6C, (void *) &IgnoreTrainCarriages},
         {HOOK_CALL, 0x490558, (void *) &FixSnailTrailTrain},
         {HOOK_CALL, 0x49220E, (void *) &FixStuckAtDohertyGarage},
         {HOOK_JUMP, 0x6F5E70, (void *) &ChangeLastTrainCarriage},
         {HOOK_CALL, 0x467AB7, (void *) &::UpdateLastThread}});

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
            {HOOK_CALL, 0x47CFE0, (void *) &MoveFlyingSchoolBlip},
            {HOOK_CALL, 0x486DB1, (void *) &MoveFlyingSchoolTrigger}});

    if (m_Config.SkipLowriderCheck)
    {
        RegisterHooks ({{HOOK_CALL, 0x478528, (void *) &IgnoreLowriderCheck}}); 
    }

    if (m_Config.SkipWuZiMuCheck)
    {
        RegisterHooks ({{HOOK_CALL, 0x48B0EB, (void *) &IgnoreWuZiMuCheck}});
    }

    if (m_Config.SkipSweetsGirlCheck)
    {
        RegisterHooks ({{HOOK_CALL, 0x48B10F, (void *) &IgnoreSweetGirlCarCheck}});
    }

    Logger::GetLogger ()->LogMessage ("Intialised ScriptVehicleRandomizer");
}

/*******************************************************/
void
ScriptVehicleRandomizer::InitialiseCache ()
{
    CacheSeats ();
    this->CachePatterns ();

    Logger::GetLogger ()->LogMessage ("Initialised Script Vehicles cache");
}

/*******************************************************/
void
ScriptVehicleRandomizer::CachePatterns ()
{
    FILE *vehPatternsFile = OpenRainbomizerFile ("Vehicle_Patterns.txt", "r", "data/");
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
                            &bicycles, &quadbikes, &planes, &helicopters, &boats,
                            &trains, &trailers, flags, &altCoords.x, &altCoords.y, &altCoords.z);

                    for (int i = 0; i < 64; i++)
                    {
                        threadName[i] = NormaliseChar (threadName[i]);
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
                         boats == 'C',  trains == 'C', trailers == 'C'});

                    pattern.SetMovedCoordinates (altCoords);
                    pattern.ParseFlags (flags);

                    pattern.Cache ();

                    mPatterns.push_back(pattern);
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
