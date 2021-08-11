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

#include "handling.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include "config.hh"
#include "injector/injector.hpp"

#include <map>

HandlingRandomizer *HandlingRandomizer::mInstance = nullptr;

const int             NUM_HANDLINGS = 209;
static tHandlingData *s_Handlings
    = reinterpret_cast<tHandlingData *> (0xC2B9DC);

/*******************************************************/
tHandlingData *
GetRandomHandling (CVehicle *vehicle)
{
    static std::map<CVehicle *, tHandlingData> data;

    tHandlingData *handling = &data[vehicle];
    if (vehicle->m_pHandling == handling)
        return handling;

    memcpy (handling, vehicle->m_pHandling, sizeof (tHandlingData));

#define ADD_RANDOMIZED_FIELD(fieldName)                                        \
    handling->fieldName = s_Handlings[random (NUM_HANDLINGS)].fieldName

    // ADD_RANDOMIZED_FIELD (index);
    ADD_RANDOMIZED_FIELD (fMass);
    ADD_RANDOMIZED_FIELD (field_8);
    ADD_RANDOMIZED_FIELD (fTurnMass);
    ADD_RANDOMIZED_FIELD (fDragMult);
    ADD_RANDOMIZED_FIELD (centreOfMass);
    ADD_RANDOMIZED_FIELD (nPercentSubmerged);
    ADD_RANDOMIZED_FIELD (field_21);
    ADD_RANDOMIZED_FIELD (field_22);
    ADD_RANDOMIZED_FIELD (field_23);
    ADD_RANDOMIZED_FIELD (fBuoyancyConstant);
    ADD_RANDOMIZED_FIELD (fTractionMultiplier);
    ADD_RANDOMIZED_FIELD (transmissionData);
    ADD_RANDOMIZED_FIELD (fBrakeDeceleration);
    ADD_RANDOMIZED_FIELD (fBrakeBias);
    ADD_RANDOMIZED_FIELD (bABS);
    ADD_RANDOMIZED_FIELD (field_9D);
    ADD_RANDOMIZED_FIELD (field_9E);
    ADD_RANDOMIZED_FIELD (field_9F);
    ADD_RANDOMIZED_FIELD (fSteeringLock);
    ADD_RANDOMIZED_FIELD (fTractionLoss);
    ADD_RANDOMIZED_FIELD (fTractionBias);
    ADD_RANDOMIZED_FIELD (fSuspensionForceLevel);
    ADD_RANDOMIZED_FIELD (fSuspensionDampingLevel);
    ADD_RANDOMIZED_FIELD (fSuspensionHighSpdComDamp);
    ADD_RANDOMIZED_FIELD (fSuspensionUpperLimit);
    ADD_RANDOMIZED_FIELD (fSuspensionLowerLimit);
    ADD_RANDOMIZED_FIELD (fSuspensionBiasBetweenFrontAndRear);
    ADD_RANDOMIZED_FIELD (fSuspensionAntiDiveMultiplier);
    ADD_RANDOMIZED_FIELD (fCollisionDamageMultiplier);
    ADD_RANDOMIZED_FIELD (modelFlags);
    ADD_RANDOMIZED_FIELD (handlingFlags);
    ADD_RANDOMIZED_FIELD (fSeatOffsetDistance);
    ADD_RANDOMIZED_FIELD (nMonetaryValue);
    ADD_RANDOMIZED_FIELD (frontLights);
    ADD_RANDOMIZED_FIELD (rearLights);
    // ADD_RANDOMIZED_FIELD (animGroup);
    ADD_RANDOMIZED_FIELD (field_DF);

    return handling;
}

/*******************************************************/
void __fastcall RandomizeHandling (CVehicle *vehicle, void *edx, CPed *ped)
{
    if (!CModelInfo::IsBoatModel (vehicle->m_nModelIndex)
        && !CModelInfo::IsHeliModel (vehicle->m_nModelIndex)
        && !CModelInfo::IsPlaneModel (vehicle->m_nModelIndex)
        && !CModelInfo::IsTrainModel (vehicle->m_nModelIndex))
        {
            vehicle->m_pHandling = GetRandomHandling (vehicle);
        }
    vehicle->SetDriver (ped);
}

/*******************************************************/
void
HandlingRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("HandlingRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x64BB57, (void *) &RandomizeHandling}});
    Logger::GetLogger ()->LogMessage ("Intialised HandlingRandomizer");
}

/*******************************************************/
void
HandlingRandomizer::DestroyInstance ()
{
    if (HandlingRandomizer::mInstance)
        delete HandlingRandomizer::mInstance;
}

/*******************************************************/
HandlingRandomizer *
HandlingRandomizer::GetInstance ()
{
    if (!HandlingRandomizer::mInstance)
        {
            HandlingRandomizer::mInstance = new HandlingRandomizer ();
            atexit (&HandlingRandomizer::DestroyInstance);
        }
    return HandlingRandomizer::mInstance;
}
