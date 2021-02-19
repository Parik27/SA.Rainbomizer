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

#include <vector>
#include <cstdint>
#include <cstdio>
#include <string>
#include <unordered_map>
#include "scm_patterns.hh"

struct CMatrix;
struct CRunningScript;

void *RandomizeCarForScript (int model, float x, float y, float z,
                             bool createdBy);
void *FixCarDoorCrash (uint8_t *vehicle, int index, int door);
void  RevertVehFixes (int vehicle);
short FixCWPacker (void *script, void *edx, short count);
void __fastcall FixKSTCarCheck (CRunningScript *scr, void *edx, short count);
void __fastcall FixJBCarHealth (CRunningScript *scr, void *edx, short vehicle);
void __fastcall FixEOTLPosition (CMatrix *matrix, void *edx, CMatrix *attach,
                                 char link);
void __fastcall FixGTAMadman (CRunningScript *scr, void *edx, int opcode);

enum flags
{
    NO_SEAT_CHECK = 1
};

const int VEHICLE_ALL = -1;

enum eVehicleApperance
{
    VEHICLE_APPEARANCE_AUTOMOBILE = 12,
    VEHICLE_APPEARANCE_BIKE,
    VEHICLE_APPEARANCE_HELI,
    VEHICLE_APPEARANCE_BOAT,
    VEHICLE_APPEARANCE_PLANE,
    VEHICLE_APPEARANCE_RC
};

enum eDoorCheckError
{
    ERR_FALSE,
    ERR_TRUE,
    ERR_UNSURE
};

class ScriptVehicleRandomizer
{
    static ScriptVehicleRandomizer *mInstance;
    static inline std::vector<ScriptVehiclePattern> mPatterns;

    ScriptVehicleRandomizer (){};
    static void DestroyInstance ();

    bool        mPosFixEnabled          = false;
    int         mSanchezDrivingOverride = -1;
    std::string mLastThread;

    eDoorCheckError DoesVehicleHaveEnoughDoors (int modelA, int modelB);

    void CachePatterns ();

public:

    uint8_t mSeatsCache[212];

    void SlowDownAndromedaInStoaway (uint8_t *vehicle, float speed);
    void ApplyEOTLFixes (int newFiretruck);

    void
    ApplyKSTFix (int newSanchez)
    {
        mSanchezDrivingOverride = newSanchez;
    };

    /// Returns the static instance for ScriptVehicleRandomizer.
    static ScriptVehicleRandomizer *GetInstance ();

    /// Returns a random id with which the given vehicle can be replaced
    int ProcessVehicleChange (int id, float &x, float &y, float &z);

    /// Returns if the EOTL pos fixed is enabled
    bool
    PosFixEnabled ()
    {
        return mPosFixEnabled;
    }

    /// Updates the last thread
    void
    UpdateLastThread (const std::string &thread)
    {
        mLastThread = thread;
    }

    /// Initialises Hooks/etc.
    void Initialise ();
    void InitialiseCache ();

    /// Initialises Hooks/etc.
    int
    GetKSTBike ()
    {
        return mSanchezDrivingOverride;
    }
};
