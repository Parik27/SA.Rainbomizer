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

ScriptVehicleRandomizer *ScriptVehicleRandomizer::mInstance = nullptr;

const int MODEL_FIRELA = 0x220;

/*******************************************************/
void
SlowDownAndromedaInStoaway (uint8_t *vehicle, float speed)
{
    printf ("%f\n", speed);
    uint16_t modelIndex = *reinterpret_cast<uint16_t *> (vehicle + 0x22);

    if (speed < 1.1 && speed > 0.9
        && (CModelInfo::IsHeliModel (modelIndex)
            || CModelInfo::IsPlaneModel (modelIndex)))
        speed = 0.7;

    printf ("%d - %f\n", modelIndex, speed);
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
RevertEOTLFix (int index)
{
    if (index == MODEL_FIRELA)
        ScriptVehicleRandomizer::GetInstance ()->ApplyEOTLFixes (MODEL_FIRELA);

    CStreaming::SetMissionDoesntRequireModel (index);
}

/*******************************************************/
void *
FixCarDoorCrash (uint8_t *vehicle, float speed)
{
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
void *
RandomizeCarForScript (int model, float x, float y, float z, bool createdBy)
{
    int newModel
        = ScriptVehicleRandomizer::GetInstance ()->GetRandomIDBasedOnVehicle (
            model);

    if (model == MODEL_FIRELA)
        {
            ScriptVehicleRandomizer::GetInstance ()->ApplyEOTLFixes (newModel);
        }

    while (ms_aInfoForModel[newModel].m_nLoadState != 1)
        {
            CStreaming::RequestModel (newModel, 12);
            CStreaming::LoadAllRequestedModels (false);
        }

    uint8_t *vehicle = (uint8_t *) CCarCtrl::CreateCarForScript (newModel, x, y,
                                                                 z, createdBy);

    if (CModelInfo::IsPoliceModel (newModel))
        {
            uint32_t *door_lock
                = reinterpret_cast<uint32_t *> (vehicle + 0x4F8);
            *door_lock = 1;
        }
    printf ("%x\n", vehicle);
    if (model == 581)
        {
            float *health = reinterpret_cast<float *> (vehicle + 0x4C0);
            *health       = 3402823466.0;
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
                            bool cont = true;
                            for (auto allowed : pattern.allowed)
                                {
                                    if (DoesVehicleMatchPattern (i, allowed))
                                        {
                                            cont = false;
                                            break;
                                        }
                                }
                            for (auto disallowed : pattern.denied)
                                {
                                    if (DoesVehicleMatchPattern (i, disallowed))
                                        {
                                            cont = true;
                                        }
                                }

                            bool shouldFreeModel = false;
                            while (ms_aInfoForModel[i].m_nLoadState != 1)
                                {
                                    CStreaming::RequestModel (i, 12);
                                    CStreaming::LoadAllRequestedModels (false);
                                    shouldFreeModel = true;
                                }

                            if ((CModelInfo::
                                         GetMaximumNumberOfPassengersFromNumberOfDoors (
                                             id)
                                     > CModelInfo::
                                         GetMaximumNumberOfPassengersFromNumberOfDoors (
                                             i)
                                 && !(pattern.flags & NO_SEAT_CHECK))
                                || cont)
                                {
                                    if (shouldFreeModel)
                                        CStreaming::RemoveModel (i);

                                    continue;
                                }

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
void
ScriptVehicleRandomizer::Initialise ()
{
    RegisterHooks ({{HOOK_CALL, 0x467B01, (void *) &RandomizeCarForScript},
                    {HOOK_CALL, 0x498AA8, (void *) &SlowDownAndromedaInStoaway},
                    {HOOK_CALL, 0x47F070, (void *) &RevertEOTLFix},
                    {HOOK_CALL, 0x5DFE79, (void *) &FixEOTLPosition}});

    Logger::GetLogger ()->LogMessage ("Intialised ScriptVehicleRandomizer");

    this->mPatterns = {

        {.pattern = 487,
         .allowed = {VEHICLE_ALL},
         .denied  = {},
         .flags   = NO_SEAT_CHECK},

        {.pattern = 406, .allowed = {406}, .denied = {}, .flags = 0},

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
