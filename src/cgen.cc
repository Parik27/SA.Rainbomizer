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

#include "cgen.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include "traffic.hh"
#include "config.hh"
#include "scm.hh"
#include <array>

ParkedCarRandomizer *ParkedCarRandomizer::mInstance = nullptr;

/*******************************************************/
void
ParkedCarRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig (
            "ParkedCarRandomizer",
            std::pair ("RandomizeFixedSpawns", &m_Config.RandomizeFixedSpawns),
            std::pair ("RandomizeRandomSpawns",
                       &m_Config.RandomizeRandomSpawns),
            std::pair ("RandomizeToSameType", &m_Config.UseSameType)))
        return;

    if (m_Config.RandomizeRandomSpawns)
        RegisterHooks ({{HOOK_CALL, 0x6F3583, (void *) &RandomizeRandomSpawn}});

    if (m_Config.RandomizeFixedSpawns)
        RegisterHooks ({{HOOK_CALL, 0x6F3EC1, (void *) &RandomizeFixedSpawn}});

    Logger::GetLogger ()->LogMessage ("Intialised ParkedCarRandomizer");
}

/*******************************************************/
void
ParkedCarRandomizer::DestroyInstance ()
{
    if (ParkedCarRandomizer::mInstance)
        delete ParkedCarRandomizer::mInstance;
}

/*******************************************************/
ParkedCarRandomizer *
ParkedCarRandomizer::GetInstance ()
{
    if (!ParkedCarRandomizer::mInstance)
        {
            ParkedCarRandomizer::mInstance = new ParkedCarRandomizer ();
            atexit (&ParkedCarRandomizer::DestroyInstance);
        }
    return ParkedCarRandomizer::mInstance;
}

/*******************************************************/
int
GetRandomCarOfType (int originalCar)
{
    for (auto &i : ScriptVehicleRandomizer::carTypes)
        {
            if (std::find (i.begin (), i.end (), originalCar) != i.end ())
                {
                    return GetRandomElement (i);
                }
        }
    return random (400, 611);
}

/*******************************************************/
/* This function hooks the CheckForBlockage function of a car generator
   to change the model index of a car generator. */
/*******************************************************/
void __fastcall RandomizeFixedSpawn (CCarGenerator *gen)
{
    auto oldModel = gen->m_nModelId;

    if (!ParkedCarRandomizer::m_Config.UseSameType)
        gen->m_nModelId = random (400, 611);
    else
        gen->m_nModelId = GetRandomCarOfType (oldModel);

    gen->DoInternalProcessing ();
    gen->m_nModelId = oldModel;
}

/*******************************************************/
/* This function is supposed to return a random car based on the ones
   defined in the zone. It is redirected to the random traffic function */
/*******************************************************/
int __fastcall RandomizeRandomSpawn (void *group, void *edx, char a2, char a3)
{
    return RandomizeTrafficCars (nullptr);
}
