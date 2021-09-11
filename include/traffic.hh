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

struct cVehicleParams;
struct CAEVehicleAudioEntity;
struct CPed;
struct CVehicle;

// Hooked Functions
int   RandomizePoliceCars ();
int   RandomizeTrafficCars (int *type);
int   RandomizeCarToLoad ();
void  FixEmptyPoliceCars (uint8_t *vehicle, char a3);
void *RandomizeCarPeds (int type, int model, float *pos, bool unk);
void *__fastcall FixCopCrash (CPed *ped, void *edx, int type);
void __fastcall FixFreightTrainCrash (CAEVehicleAudioEntity *audio, void *edx,
                                      cVehicleParams *vehicle_params);
void __fastcall PlaceOnRoadFix (CVehicle *vehicle, void *edx);
int ChoosePoliceVehicleBasedOnModel (int model);
template <int address>
void *__fastcall RandomizeRoadblocks (CVehicle *vehicle, void *edx, int model,
                                      char createdBy,
                                      char setupSuspensionLines);

/// Randomizes cars that spawn in traffic including the police cars
class TrafficRandomizer
{
    bool                      mInitialVehiclesLoaded = false;
    static TrafficRandomizer *mInstance;

    unsigned char mOriginalData[5];

    TrafficRandomizer (){};
    static void DestroyInstance ();

    void FixTrainSpawns ();

public:
    std::deque<int> mMostRecentSpawnedVehicles;
    std::deque<int> mMostRecentLoadedVehicles;
    int             mForcedCar = 0;

    static inline struct Config
    {
        int ForcedVehicleID;

        bool Trains;
        bool Boats;
        bool Aircraft;
        bool Cars;
        bool Bikes;
        bool Trailers;

        int DefaultModel;
    } m_Config;

    /// Initialises Hooks/etc.
    void Initialise ();

    void Install6AF420_Hook ();
    void Revert6AF420_Hook ();

    bool IsVehicleAllowed (int model);

    void MakeRCsEnterable ();

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

    friend int  RandomizeTrafficCars (int *type);
    friend int  RandomizeCarToLoad ();
    friend int  RandomizePoliceCars ();
    friend void LoadRandomVehiclesAtStart ();
};
