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
#include "loader.hh"
#include <cmath>

ScriptVehicleRandomizer *ScriptVehicleRandomizer::mInstance = nullptr;

const int MODEL_FIRELA = 0x220;
const int MODEL_SANCHZ = 468;
const int MODEL_RC_BARON = 464;

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
void __fastcall FixJBCarHealth (CRunningScript *scr, void *edx, short vehicle)
{
    scr->CollectParameters (vehicle);
    int *ScriptParams = (int *) 0xA43C78;

    CPool *vehiclePool = (CPool *) 0xB74494;

    uint8_t *veh
        = ((uint8_t *) vehiclePool->m_pObjects) + 0xA18 * (ScriptParams[0]);
    float *health = reinterpret_cast<float *> (veh + 0x4C0);

    printf ("%d %d\n", ScriptParams[0], ScriptParams[1]);
    printf ("%x\n", veh);
    printf ("%f\n", *health);

    Logger::GetLogger ()->LogMessage (std::to_string (*health));
    if (*health > -1)
        {
            float *scriptParamHealth = (float *) ScriptParams + 1;
            Logger::GetLogger ()->LogMessage (
                std::to_string (*scriptParamHealth));
            *scriptParamHealth = 0.0;
            Logger::GetLogger ()->LogMessage (
                std::to_string (*scriptParamHealth));
        }
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
ApplyFixedBasedOnModel (int model, int newModel)
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
        = ScriptVehicleRandomizer::GetInstance ()->GetRandomIDBasedOnVehicle (
            model);

    ApplyFixedBasedOnModel (model, newModel);

    // Load the new vehicle. Fallback to the original if needed
    auto err = StreamingManager::AttemptToLoadVehicle (newModel);
    if (err == ERR_FAILED)
        newModel = model;
	
    // Freefall fix
    if (x > 1279.6 && x < 1279.7)
        {
            while (newModel == MODEL_RC_BARON)
                {
                    newModel = ScriptVehicleRandomizer::GetInstance ()
                                   ->GetRandomIDBasedOnVehicle (model);
                }
            x += 50;
        }

    // Dam and Blast + Saint Mark's Bistro Fix
    if (x > 1477.4 && x < 1479.8)
        {
            while (newModel == MODEL_RC_BARON)
                {
                    newModel = ScriptVehicleRandomizer::GetInstance ()->GetRandomIDBasedOnVehicle (model);
                }
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
int
GetVehicleSeats (int vehicle)
{
    uint8_t *modelInfo = (uint8_t *) ms_modelInfoPtrs[vehicle];
    uint8_t  numDoors  = modelInfo[0x4C];

    return numDoors;
}

/*******************************************************/
bool
ScriptVehicleRandomizer::DoesVehicleMatchPatternOR (int model,
                                                    const std::vector<int> &ors)
{
    for (auto pattern : ors)
        if (DoesVehicleMatchPattern (model, pattern))
            return true;
    return false;
}

/*******************************************************/
bool
ScriptVehicleRandomizer::DoesVehicleMatchPatternAND (
    int model, const std::vector<int> &ands)
{
    for (auto pattern : ands)
        if (!DoesVehicleMatchPattern (model, pattern))
            return false;
    return true;
}

/*******************************************************/
bool
ScriptVehicleRandomizer::CheckIfVehicleMatchesPattern (
    int model, const ScriptPatterns &pattern)
{
    return DoesVehicleMatchPatternOR (model, pattern.allowed)
           && !DoesVehicleMatchPatternOR (model, pattern.denied);
}

/*******************************************************/
eDoorCheckError
ScriptVehicleRandomizer::DoesVehicleHaveEnoughDoors (int modelA, int orig)
{
    eLoadError err = StreamingManager::AttemptToLoadVehicle (modelA);

    // The load was unsuccessful so can't be sure
    if (err == ERR_FAILED)
        return ERR_UNSURE;

    if (CModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors (orig)
        > CModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors (modelA))
        {
            if (!ERR_ALREADY_LOADED)
                CStreaming::SetIsDeletable (modelA);

            return ERR_FALSE;
        }

    return ERR_TRUE;
}

/*******************************************************/
int
ScriptVehicleRandomizer::GetRandomIDBasedOnVehicle (int id)
{
    // return 522;
    for (auto pattern : mPatterns)
        {
            if (DoesVehicleMatchPattern (id, pattern.pattern))
                {
                    for (int i = random (611, 400), j = 0; j < 1500;
                         i = random (611, 400), j++)
                        {
                            if (!CheckIfVehicleMatchesPattern (i, pattern))
                                continue;

                            if (pattern.flags & NO_SEAT_CHECK)
                                return i;

                            auto err = DoesVehicleHaveEnoughDoors (i, id);

                            if (err == ERR_UNSURE)
                                {
                                    Logger::GetLogger ()->LogMessage (
                                        "Unable to spawn a "
                                        "truly random vehicle");

                                    puts ("UNSURE");
                                    return StreamingManager::
                                        GetRandomLoadedVehicle ();
                                }

                            else if (err == ERR_FALSE)
                                continue;

                            return i;
                        }
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
    uint8_t  numDoors  = modelInfo[0x4C];

    if (pattern == VEHICLE_ALL)
        return true;

    if (pattern < 12)
        {
            if (type == pattern)
                return true;
        }
    else if (pattern < 17)
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
void
ScriptVehicleRandomizer::Initialise ()
{
    RegisterHooks ({{HOOK_CALL, 0x467B01, (void *) &RandomizeCarForScript},
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
                    {HOOK_CALL, 0x49128C, (void *) &FixGTAMadman}});

    Logger::GetLogger ()->LogMessage ("Intialised ScriptVehicleRandomizer");

    this->mPatterns = {

        {.pattern = 487,
         .allowed = {VEHICLE_ALL},
         .denied  = {},
         .flags   = NO_SEAT_CHECK},

        {.pattern = 406, .allowed = {406}, .denied = {}, .flags = 0},

        {.pattern = 486, .allowed = {486}, .denied = {}, .flags = 0},

        {.pattern = 581,
         .allowed = {VEHICLE_APPEARANCE_BIKE, 594},
         .denied  = {},
         .flags   = 0},

        {.pattern = 539, .allowed = {VEHICLE_BOAT, 464, 501, 465}},
        {.pattern = 521, .allowed = {VEHICLE_APPEARANCE_BIKE}},
        {.pattern = 522, .allowed = {VEHICLE_APPEARANCE_BIKE}},

        {.pattern = 524, .allowed = {524}, .denied = {}, .flags = 0},

        {.pattern = 501, .allowed = {465, 501}, .denied = {}, .flags = 0},

        {.pattern = 530, .allowed = {530}, .denied = {}, .flags = 0},

        {.pattern = 514, .allowed = {514, 515, 403}, .denied = {}, .flags = 0},

        {.pattern = 515, .allowed = {514, 515, 403}, .denied = {}, .flags = 0},

        {.pattern = 403, .allowed = {514, 515, 403}, .denied = {}, .flags = 0},
	    
	 {.pattern = 478,
         .allowed = {	602, 496, 401, 518, 527, 419, 587, 533, 526, 474, 600, 445,507,
			439, 549, 491, 422, 605, 572, 478, 554, 536, 575, 534, 534, 567,
			535, 576, 412, 402, 542, 603, 475, 429, 541, 415, 480, 562, 434,
			494, 502, 503, 411, 559, 560, 506, 541, 558, 555, 477, 546, 550},
         .denied = {},
         .flags  = 0},

        {.pattern = MODEL_FIRELA,
         .allowed = {VEHICLE_ALL},
         .denied  = {VEHICLE_TRAIN, VEHICLE_BOAT},
         .flags   = NO_SEAT_CHECK},

        {.pattern = 567,
         .allowed = {536, 575, 534, 567, 535, 576, 412, 566},
         .denied  = {},
         .flags   = NO_SEAT_CHECK},

        {.pattern = 584,
         .allowed = {435, 450, 591, 584},
         .denied  = {},
         .flags   = 0},

        {.pattern = 435,
         .allowed = {435, 450, 591, 584},
         .denied  = {},
         .flags   = 0},

        {.pattern = 450,
         .allowed = {435, 450, 591, 584},
         .denied  = {},
         .flags   = 0},

        {.pattern = 591,
         .allowed = {435, 450, 591, 584},
         .denied  = {},
         .flags   = 0},

        {.pattern = VEHICLE_BMX,
         .allowed = {VEHICLE_APPEARANCE_AUTOMOBILE, VEHICLE_APPEARANCE_BIKE},
         .denied  = {428, 443, 406},
         .flags   = NO_SEAT_CHECK},

        {.pattern = VEHICLE_APPEARANCE_AUTOMOBILE,
         .allowed = {VEHICLE_APPEARANCE_AUTOMOBILE, VEHICLE_APPEARANCE_BIKE},
         .denied  = {},
         .flags   = 0},

        {.pattern = 425, .allowed = {425, 520}, .denied = {}, .flags = 0},
        {.pattern = 520, .allowed = {425, 520}, .denied = {}, .flags = 0},

        {.pattern = VEHICLE_APPEARANCE_HELI,
         .allowed = {VEHICLE_APPEARANCE_HELI},
         .denied  = {539},
         .flags   = 0},

        {.pattern = VEHICLE_APPEARANCE_PLANE,
         .allowed = {VEHICLE_APPEARANCE_PLANE},
         .denied  = {539},
         .flags   = 0},

        {.pattern = VEHICLE_APPEARANCE_BOAT,
         .allowed = {VEHICLE_APPEARANCE_BOAT, 539},
         .denied  = {},
         .flags   = 0},

        {.pattern = VEHICLE_APPEARANCE_BIKE,
         .allowed = {VEHICLE_APPEARANCE_BIKE, VEHICLE_APPEARANCE_AUTOMOBILE},
         .denied  = {},
         .flags   = 0},
    };
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
