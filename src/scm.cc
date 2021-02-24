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

ScriptVehicleRandomizer *ScriptVehicleRandomizer::mInstance = nullptr;

const int MODEL_DUMPER   = 406;
const int MODEL_FIRELA = 0x220;
const int MODEL_SANCHZ = 468;
const int MODEL_HYDRA    = 520;
const int MODEL_CEMENT   = 524;
const int MODEL_FORKLIFT = 530;
int       ignoreNextHook = false;

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

/*******************************************************/
void __fastcall FixCarChecks (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[1] == MODEL_SANCHZ
        || (ScriptParams[1] == MODEL_FORKLIFT
            && ScriptVehicleRandomizer::GetInstance ()->mLastThread == "heist9")
        || ScriptParams[1] == MODEL_DUMPER)
        {
            int newVeh
                = ScriptVehicleRandomizer::GetInstance ()->GetNewCarForCheck ();
            if (newVeh != -1)
                ScriptParams[1] = newVeh;
        }
    else if (ScriptParams[1] == MODEL_CEMENT && FindPlayerVehicle ())
        ScriptParams[1] = FindPlayerVehicle ()->m_nModelIndex;
    else if (ScriptParams[1] == MODEL_HYDRA && FindPlayerVehicle ())
    {
            int playerVeh = FindPlayerVehicle ()->m_nModelIndex;
        if ((CModelInfo::IsPlaneModel (playerVeh) && playerVeh != 539)
            || CModelInfo::IsHeliModel (playerVeh))
            ScriptParams[1] = playerVeh;
    }
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
        || (model == MODEL_DUMPER && ScriptVehicleRandomizer::GetInstance()->mLastThread == "casino2"))
        ScriptVehicleRandomizer::GetInstance ()->ApplyCarCheckFix (newModel);
}

/*******************************************************/
void
ApplyFixesBasedOnMission ()
{
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
    {
            injector::WriteMemory (0x969130 + 48, 1); // Enable Flying Cars
    }
}

/*******************************************************/
void *
RandomizeCarForScript (int model, float x, float y, float z, bool createdBy)
{
    /*int newModel = 500;*/ 
    int newModel = ScriptVehicleRandomizer::GetInstance
                           ()->ProcessVehicleChange (model,
                                                                         x, y,
                                                                         z);
    //Logger::GetLogger ()->LogMessage ("Vehicle Spawned: "
    //                                  + std::to_string (newModel));

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
    for (auto pattern : mPatterns)
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
    if (scr->CheckName ("desert5"))
    {
        if (ScriptParams[1] == 0 && !ignoreNextHook)
        {
                ScriptVehicleRandomizer::GetInstance ()->SaveTestTime (
                    ScriptSpace[253]);
                Scrpt::CallOpcode (0x396, "pause_timer", 1);
                //Logger::GetLogger ()->LogMessage ("Saving flight school time: "
                //                                  + std::to_string(ScriptSpace[253]));
                ScriptSpace[253] -= 120000;
                //Logger::GetLogger ()->LogMessage ("New flight school time: "
                //                                  + std::to_string(ScriptSpace[253]));
                ignoreNextHook = true;
        }
    }
}

/*******************************************************/
void __fastcall CheckFlightSchoolTestStart (CRunningScript *scr, void *edx,
                                            short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("desert5"))
        if (ScriptParams[1] == 16)
                ignoreNextHook = false;
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
        //ScriptParams[3]  = seconds;
        //ScriptParams[4]  = miliseconds;
    }
}

/*******************************************************/
char __fastcall DisplayCorrectFlyingSchoolTime (CRunningScript *scr, void *edx, char* textptr, char length)
{
    if (scr->CheckName ("desert5"))
    {
        int actualTime = ScriptVehicleRandomizer::GetInstance ()
            ->GetOriginalTestTime ();
        int seconds = actualTime / 1000;
        ScriptSpace[301] = seconds;
        //Logger::GetLogger ()->LogMessage ("Set flight school time");
    }
    return CallMethodAndReturn<char, 0x463D50> (scr, textptr, length);
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
void __fastcall CheckFlightSchoolEnd (CRunningScript *scr, void *edx,
                                      short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("desert5"))
        injector::WriteMemory (0x969130 + 48, 0); // Disable Flying Cars
}

/*******************************************************/
void __fastcall FixMaddDogg (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);

    // Madd Dogg - move boxes
    if (scr->CheckName ("doc2"))
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[0] >> 8));

            SetMaddDoggOffset (vehicle, (float *) &ScriptParams[3], 0.5);
        }
    // HIghjack - Adjust truck requirements
    else if (scr->CheckName ("toreno2"))
        {
            if (fabs (((float *) ScriptParams)[2] - 4.0) < 0.01)
                ((float *) ScriptParams)[2] = 104.0;
        }
}

/*******************************************************/
void __fastcall FixMaddDoggBoxes (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (scr->CheckName ("doc2"))
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[1] >> 8));
            SetMaddDoggOffset (vehicle, (float *) &ScriptParams[4], 0);
        }
}

/*******************************************************/
int
MoveFlyingSchoolCorona (float x, float y, float z, float dirX, float dirY, float dirZ, float radius, int type)
{
    if (ScriptVehicleRandomizer::GetInstance()->mLastThread == "desert5")
    {
            if (int (x) == 377 && int (z) == 80)
                x += 250.0f;
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
                x += 250.0f;
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
            ((float *) ScriptParams)[1] += 250.0f;
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
        CallMethod<0x4859D0> (scr, result);
    }
}

/*******************************************************/
void
ScriptVehicleRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("ScriptVehicleRandomizer", 
        std::pair ("EnableExtraTimeForSchools", &m_Config.MoreSchoolTestTime),
        std::pair ("LowriderMissions", &m_Config.SkipLowriderCheck)))
    return;

    RegisterHooks (
        {{HOOK_CALL, 0x467B01, (void *) &RandomizeCarForScript},
         {HOOK_CALL, 0x498AA8, (void *) &SlowDownAndromedaInStoaway},
         {HOOK_CALL, 0x47F070, (void *) &RevertVehFixes},
         {HOOK_CALL, 0x5DFE79, (void *) &FixEOTLPosition},
         {HOOK_CALL, 0x469612, (void *) &FixCarChecks},
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
         {HOOK_CALL, 0x476BCB, (void *) &IgnoreLandingGearCheck},
         {HOOK_CALL, 0x467AB7, (void *) &::UpdateLastThread}});

    if (m_Config.MoreSchoolTestTime)
        RegisterHooks (
                {{HOOK_CALL, 0x48CFA9, (void *) &ProcessModifiedSchoolTimes},
            {HOOK_CALL, 0x48A664, (void *) &DisplayCorrectSchoolTime},
            {HOOK_CALL, 0x47D3CC, (void *) &ProcessModifiedSchoolTimesFlying},
            {HOOK_CALL, 0x4696BC, (void *) &CheckFlightSchoolTestStart},
            {HOOK_CALL, 0x48F7CC, (void *) &DisplayCorrectFlyingSchoolTime},
            {HOOK_CALL, 0x497ED7, (void *) &MoveFlyingSchoolCorona},
            {HOOK_CALL, 0x47CFE0, (void *) &MoveFlyingSchoolBlip},
            {HOOK_CALL, 0x486DB1, (void *) &MoveFlyingSchoolTrigger},
            {HOOK_CALL, 0x46C1AB, (void *) &CheckFlightSchoolEnd}});

    if (m_Config.SkipLowriderCheck)
        RegisterHooks ({{HOOK_CALL, 0x478528, (void *) &IgnoreLowriderCheck}});

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

                    //mModels.push_back (std::vector<std::string> ());
                    //line[strcspn (line, "\n")] = 0;
                    //mModels.back ().push_back (line);
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
