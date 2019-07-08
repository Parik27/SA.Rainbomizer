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

#include "base.hh"
#include <deque>

// Hook Functions
/*
 - GetDefaultPoliceCar
 - StreamingRequestModel
 - ChooseModel
 - PopulationAddPed
 - ChooseCarModelToLoad
 - Update GetNewVehicle jump table
 - AddPoliceCarOccupants
 - ChoosePoliceModel
 */

class cVehicleParams;
class CAEVehicleAudioEntity;

// Hooked Functions
int   RandomizePoliceCars ();
int   RandomizeTrafficCars (int *type);
int   RandomizeCarToLoad ();
int   FixEmptyPoliceCars (uint8_t *vehicle, char a3);
void *RandomizeCarPeds (int type, int model, float *pos, bool unk);

void __fastcall FixFreightTrainCrash (CAEVehicleAudioEntity *audio, void *edx,
                                      cVehicleParams *vehicle_params);

int ChoosePoliceVehicleBasedOnModel (int model);

/// Randomizes cars that spawn in traffic including the police cars
class TrafficRandomizer
{
    std::deque<int> mMostRecentSpawnedVehicles;
    std::deque<int> mMostRecentLoadedVehicles;

    bool                      mInitialVehiclesLoaded = false;
    int                       mForcedCar             = 0;
    static TrafficRandomizer *mInstance;

    TrafficRandomizer (){};
    static void DestroyInstance ();

    void FixTrainSpawns ();

public:
    /// Initialises Hooks/etc.
    void Initialise ();

    /// Exception Handling
    static void ExceptionHandlerCallback (_EXCEPTION_POINTERS *ep);

    /// Sets an overridden car for always spawning in traffic
    void
    SetForcedRandomCar (int car)
    {
        mForcedCar = car;
    };

    /// Gets instance for TrafficRandomizer
    static TrafficRandomizer *GetInstance ();

    friend int RandomizeTrafficCars (int *type);
    friend int RandomizeCarToLoad ();
    friend int RandomizePoliceCars ();
};
