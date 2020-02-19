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

ScriptVehicleRandomizer *ScriptVehicleRandomizer::mInstance = nullptr;

const int MODEL_FIRELA = 0x220;
const int MODEL_SANCHZ = 468;

/*******************************************************/
void
SlowDownAndromedaInStoaway (uint8_t *vehicle, float speed)
{
    uint16_t modelIndex = *reinterpret_cast<uint16_t *> (vehicle + 0x22);

    if (speed < 1.1 && speed > 0.9
        && (CModelInfo::IsHeliModel (modelIndex)
            || CModelInfo::IsPlaneModel (modelIndex)))
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
        ScriptVehicleRandomizer::GetInstance ()->ApplyKSTFix (-1);

    CStreaming::SetMissionDoesntRequireModel (index);
}

/*******************************************************/
void __fastcall FixKSTCarCheck (CRunningScript *scr, void *edx, short count)
{
    scr->CollectParameters (count);
    if (ScriptParams[1] == MODEL_SANCHZ)
        {
            int newBike
                = ScriptVehicleRandomizer::GetInstance ()->GetKSTBike ();
            if (newBike != -1)
                ScriptParams[1] = newBike;
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
    else if (model == MODEL_SANCHZ)
        ScriptVehicleRandomizer::GetInstance ()->ApplyKSTFix (newModel);
}

/*******************************************************/
void *
RandomizeCarForScript (int model, float x, float y, float z, bool createdBy)
{
    int newModel
        = ScriptVehicleRandomizer::GetInstance ()->ProcessVehicleChange (model,
                                                                         x, y,
                                                                         z);

    ApplyFixesBasedOnModel (model, newModel);

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
    auto config = ConfigManager::GetInstance ()->GetConfigs ().scriptVehicle;
    for (auto pattern : mPatternCache)
        {
            if (DoesVehicleMatchPattern (id, pattern.pattern)
                && CompareCoordinates (x, y, z, pattern.coords[0],
                                       pattern.coords[1], pattern.coords[2])
                && (pattern.thread == "" || pattern.thread == mLastThread))
                {
                    // Update x,y,z coordinates
                    x += pattern.move[0];
                    y += pattern.move[1];
                    z += pattern.move[2];

                    std::vector<uint16_t> vehicles;
                    if (pattern.seat_check)
                        {
                            for (auto vehicle : pattern.cars)
                                {
                                    if (mSeatsCache[id - 400]
                                        <= mSeatsCache[vehicle - 400])
                                        vehicles.push_back (vehicle);
                                }
                        }
                    else
                        vehicles = pattern.cars;

                    if (vehicles.size () > 0)
                        return vehicles[random (vehicles.size () - 1)];

                    Logger::GetLogger ()->LogMessage (
                        "Pattern yielded no valid vehicles");
                }
        }
    return id;
}

/*******************************************************/
bool
ScriptVehicleRandomizer::DoesVehicleMatchPattern (int vehicle, int pattern)
{
    uint8_t *modelInfo = (uint8_t *) ms_modelInfoPtrs[vehicle];
    uint32_t type      = *(uint32_t *) (modelInfo + 0x3C);

    if (pattern == VEHICLE_ALL)
        return true;

    if (pattern < 12)
        {
            if (type == pattern)
                return true;
        }
    else if (pattern < 18)
        {
            switch (pattern)
                {
                case VEHICLE_APPEARANCE_PLANE:
                    if (type == VEHICLE_PLANE || type == VEHICLE_FPLANE)
                        return true;
                    break;

                case VEHICLE_APPEARANCE_BOAT:
                    if (type == VEHICLE_BOAT)
                        return true;
                    break;

                case VEHICLE_APPEARANCE_HELI:
                    if (type == VEHICLE_HELI || type == VEHICLE_FHELI)
                        return true;
                    break;

                case VEHICLE_APPEARANCE_BIKE:
                    if (type == VEHICLE_BMX || type == VEHICLE_BIKE)
                        return true;
                    break;

                case VEHICLE_APPEARANCE_RC:
                    if (CModelInfo::IsRCModel (vehicle))
                        return true;
                    break;

                default:
                    if (type != VEHICLE_BMX && type != VEHICLE_BIKE
                        && type != VEHICLE_FPLANE && type != VEHICLE_HELI
                        && type != VEHICLE_BOAT && type != VEHICLE_FPLANE
                        && type != VEHICLE_PLANE && type != VEHICLE_TRAIN
                        && type != VEHICLE_TRAILER)
                        return true;
                }
        }
    else
        return pattern == vehicle;

    return false;
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

    if (scr->CheckName ("doc2"))
        {
            CVehicle *vehicle = (CVehicle *) (ms_pVehiclePool->m_pObjects
                                              + 0xA18 * (ScriptParams[0] >> 8));

            SetMaddDoggOffset (vehicle, (float *) &ScriptParams[3], 0.5);
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
void
ScriptVehicleRandomizer::Initialise ()
{

    auto config = ConfigManager::GetInstance ()->GetConfigs ().scriptVehicle;
    if (!config.enabled)
        return;

    RegisterHooks (
        {{HOOK_CALL, 0x467B01, (void *) &RandomizeCarForScript},
         {HOOK_CALL, 0x498AA8, (void *) &SlowDownAndromedaInStoaway},
         {HOOK_CALL, 0x47F070, (void *) &RevertVehFixes},
         {HOOK_CALL, 0x5DFE79, (void *) &FixEOTLPosition},
         {HOOK_CALL, 0x469612, (void *) &FixKSTCarCheck},
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
         {HOOK_CALL, 0x467AB7, (void *) &::UpdateLastThread}});

    Logger::GetLogger ()->LogMessage ("Intialised ScriptVehicleRandomizer");
}

/*******************************************************/
void
ScriptVehicleRandomizer::InitialiseCache ()
{
    this->CacheSeats ();
    this->CachePatterns ();

    Logger::GetLogger ()->LogMessage ("Initialised Script Vehicles cache");
}

/*******************************************************/
void
ScriptVehicleRandomizer::CacheSeats ()
{
    for (int i = 0; i < 212; i++)
        {
            auto err = StreamingManager::AttemptToLoadVehicle (i + 400);
            if (err != ERR_FAILED)
                {
                    mSeatsCache[i] = CModelInfo::
                        GetMaximumNumberOfPassengersFromNumberOfDoors (i + 400);

                    if (err != ERR_ALREADY_LOADED)
                        CStreaming::RemoveModel (i + 400);
                    continue;
                }

            mSeatsCache[i] = 3; // fallback (safest)
            Logger::GetLogger ()->LogMessage ("Unable to cache seats for model "
                                              + std::to_string (i));
        }
}

/*******************************************************/
void
ScriptVehicleRandomizer::CachePatterns ()
{
    auto &config = ConfigManager::GetInstance ()->GetConfigs ().scriptVehicle;
    for (auto &pattern : config.patterns)
        {
            CachedPattern cache;
            memcpy (cache.coords, pattern.coords, sizeof (cache.coords));
            memcpy (cache.move, pattern.move, sizeof (cache.move));
            cache.thread     = pattern.thread;
            cache.pattern    = pattern.vehicle;
            cache.seat_check = pattern.seat_check;

            bool vehicleMask[212] = {0};
            auto updateMask = [&] (const std::vector<int16_t> &v, bool val) {
                for (auto i : v)
                    {
                        if (i >= 400)
                            vehicleMask[i - 400] = val;
                        else
                            {
                                for (int j = 0; j < 212; j++)
                                    {
                                        if (DoesVehicleMatchPattern (j + 400,
                                                                     i))
                                            vehicleMask[j] = val;
                                    }
                            }
                    }
            };
            updateMask (pattern.allowed, true);
            updateMask (pattern.denied, false);

            // If the vehicle matched is a static vehicle, cache the seats
            if (cache.pattern >= 400 && cache.seat_check)
                {
                    for (int i = 0; i < 212; i++)
                        {
                            if (mSeatsCache[cache.pattern - 400]
                                > mSeatsCache[i])
                                vehicleMask[i] = false;
                        }
                    cache.seat_check = false;
                }

            for (int i = 0; i < 212; i++)
                {
                    if (vehicleMask[i])
                        cache.cars.push_back (i + 400);
                }

            this->mPatternCache.push_back (cache);
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
