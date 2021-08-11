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

HandlingRandomizer *HandlingRandomizer::mInstance = nullptr;
const int           baseHandlingAddress           = 0xC2B9DC;

/*******************************************************/
void __fastcall RandomizeHandling (CVehicle *vehicle, void *edx, CPed *ped) 
{
    if (vehicle->m_nPacMansCollected != 100 && !CModelInfo::IsBoatModel(vehicle->m_nModelIndex) 
        && !CModelInfo::IsHeliModel(vehicle->m_nModelIndex) && 
        !CModelInfo::IsPlaneModel (vehicle->m_nModelIndex)
        && !CModelInfo::IsTrainModel (vehicle->m_nModelIndex))
    {
        vehicle->m_pHandling->index = injector::ReadMemory<float> (
                baseHandlingAddress + (0xe0 * random (209)));
        vehicle->m_pHandling->fMass = injector::ReadMemory<float> 
            (baseHandlingAddress + 4 + (0xe0 * random(209)));
        vehicle->m_pHandling->field_8 = injector::ReadMemory<float> (
            baseHandlingAddress + 8 + (0xe0 * random (209)));
        vehicle->m_pHandling->fTurnMass = injector::ReadMemory<float> (
            baseHandlingAddress + 12 + (0xe0 * random (209)));
        vehicle->m_pHandling->fDragMult = injector::ReadMemory<float> (
            baseHandlingAddress + 16 + (0xe0 * random (209)));
        vehicle->m_pHandling->centreOfMass = injector::ReadMemory<RwV3d> (
            baseHandlingAddress + 20 + (0xe0 * random (209)));
        vehicle->m_pHandling->nPercentSubmerged = injector::ReadMemory<char> (
            baseHandlingAddress + 32 + (0xe0 * random (209)));
        vehicle->m_pHandling->field_21 = injector::ReadMemory<char> (
            baseHandlingAddress + 33 + (0xe0 * random (209)));
        vehicle->m_pHandling->field_22 = injector::ReadMemory<char> (
            baseHandlingAddress + 34 + (0xe0 * random (209)));
        vehicle->m_pHandling->field_23 = injector::ReadMemory<char> (
            baseHandlingAddress + 35 + (0xe0 * random (209)));
        vehicle->m_pHandling->fBuoyancyConstant = injector::ReadMemory<float> (
            baseHandlingAddress + 36 + (0xe0 * random (209)));
        vehicle->m_pHandling->fTractionMultiplier = injector::ReadMemory<int> (
            baseHandlingAddress + 40 + (0xe0 * random (209)));
        vehicle->m_pHandling->transmissionData = injector::ReadMemory<cTransmission> (
            baseHandlingAddress + 44 + (0xe0 * random (209)));
        vehicle->m_pHandling->fBrakeDeceleration = injector::ReadMemory<float> (
            baseHandlingAddress + 148 + (0xe0 * random (209)));
        vehicle->m_pHandling->fBrakeBias = injector::ReadMemory<int> (
            baseHandlingAddress + 152 + (0xe0 * random (209)));
        vehicle->m_pHandling->bABS = injector::ReadMemory<char> (
            baseHandlingAddress + 156 + (0xe0 * random (209)));
        vehicle->m_pHandling->field_9D = injector::ReadMemory<char> (
            baseHandlingAddress + 157 + (0xe0 * random (209)));
        vehicle->m_pHandling->field_9E = injector::ReadMemory<char> (
            baseHandlingAddress + 158 + (0xe0 * random (209)));
        vehicle->m_pHandling->field_9F = injector::ReadMemory<char> (
            baseHandlingAddress + 159 + (0xe0 * random (209)));
        vehicle->m_pHandling->fSteeringLock = injector::ReadMemory<float> (
            baseHandlingAddress + 160 + (0xe0 * random (209)));
        vehicle->m_pHandling->fTractionLoss = injector::ReadMemory<int> (
            baseHandlingAddress + 164 + (0xe0 * random (209)));
        vehicle->m_pHandling->fTractionBias = injector::ReadMemory<int> (
            baseHandlingAddress + 168 + (0xe0 * random (209)));
        vehicle->m_pHandling->fSuspensionForceLevel = injector::ReadMemory<float> (
            baseHandlingAddress + 172 + (0xe0 * random (209)));
        vehicle->m_pHandling->fSuspensionDampingLevel = injector::ReadMemory<int> (
            baseHandlingAddress + 176 + (0xe0 * random (209)));
        vehicle->m_pHandling->fSuspensionHighSpdComDamp
            = injector::ReadMemory<int> (baseHandlingAddress + 180
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->fSuspensionUpperLimit
            = injector::ReadMemory<float> (baseHandlingAddress + 184
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->fSuspensionLowerLimit
            = injector::ReadMemory<float> (baseHandlingAddress + 188
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->fSuspensionBiasBetweenFrontAndRear
            = injector::ReadMemory<int> (baseHandlingAddress + 192
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->fSuspensionAntiDiveMultiplier
            = injector::ReadMemory<int> (baseHandlingAddress + 196
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->fCollisionDamageMultiplier
            = injector::ReadMemory<float> (baseHandlingAddress + 200
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->modelFlags
            = injector::ReadMemory<unsigned int> (baseHandlingAddress + 204
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->handlingFlags = injector::ReadMemory<int> (
            baseHandlingAddress + 208 + (0xe0 * random (209)));
        vehicle->m_pHandling->fSeatOffsetDistance = injector::ReadMemory<int> (
            baseHandlingAddress + 212 + (0xe0 * random (209)));
        vehicle->m_pHandling->nMonetaryValue
            = injector::ReadMemory<int> (baseHandlingAddress + 216
                                           + (0xe0 * random (209)));
        vehicle->m_pHandling->frontLights = injector::ReadMemory<char> (
            baseHandlingAddress + 220 + (0xe0 * random (209)));
        vehicle->m_pHandling->rearLights = injector::ReadMemory<char> (
            baseHandlingAddress + 221 + (0xe0 * random (209)));
        //vehicle->m_pHandling->animGroup = injector::ReadMemory<char> (
        //    baseHandlingAddress + 222 + (0xe0 * random (209)));
        vehicle->m_pHandling->field_DF = injector::ReadMemory<char> (
            baseHandlingAddress + 223 + (0xe0 * random (209)));
        vehicle->m_nPacMansCollected = 100;
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
