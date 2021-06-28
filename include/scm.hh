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
#include <map>
#include <unordered_map>
#include "scm_patterns.hh"

struct CMatrix;
struct CRunningScript;

void *RandomizeCarForScript (int model, float x, float y, float z,
                             bool createdBy);
void  SlowDownAndromedaInStoaway (uint8_t *vehicle, float speed);
void *FixCarDoorCrash (uint8_t *vehicle, int index, int door);
void  RevertVehFixes (int vehicle);
short FixCWPacker (void *script, void *edx, short count);
void __fastcall FixCarChecks (CRunningScript *scr, void *edx, short count);
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
    int         mVehicleDrivingOverride = -1;
    int         mOriginalTestTime       = 0;
    bool        mInFlyingCar            = false;

    eDoorCheckError DoesVehicleHaveEnoughDoors (int modelA, int modelB);

    void CachePatterns ();

public:

    static inline struct Config
    {
        bool MoreSchoolTestTime;
        bool RandomizeTrains;

        // Vehicle Checks
        bool SkipLowriderCheck;
        bool SkipWuZiMuCheck;
        bool SkipSweetsGirlCheck;
        bool SkipCourierCheck;
        bool SkipBMXChallengeCheck;
        bool SkipNRGChallengeCheck;
        bool SkipChiliadCheck;

        // Vehicle Side Mission Randomization
        bool Taxi;
        bool Firefighting;
        bool Burglary;
        bool Pimping;
        bool Paramedic;
        bool Courier;
        bool Bike;

    } m_Config;

    uint8_t mSeatsCache[212];
    std::string mLastThread = "";

    static inline std::map<int, std::string> oddMissions{{121, "taxiodd"}, {122, "ambulan"},
                                           {123, "firetru"}, {124, "copcar"},
                                           {125, "burgjb"},  {127, "pimp"},
                                           {131, "bcour"},   {132, "mtbiker"},
                                           {133, "stunt"}};

    static inline int mCurrentMissionRunning = -1;
    static inline int mCourierMissionType    = -1;
    static inline int mDes3HeliHandle = -1;
    static inline int mDes3Stuck          = false;

    // Struct for RC heli magnet sections
    struct RCHeliMagnet
    {
        int  currentObj              = 0;
        int  objHandles[17]          = {0};
        int  vehID                   = 0;
        bool isPlayerInVeh           = false;
        bool isCutsceneActive        = false;
        int  objectAttached          = -1;
        int  pickUpObjectTimer       = 1000;
        int  fakeColHandle           = 0;
        int  invalidHandle           = 0;
        bool atMagnetSection         = false;
        int  checkForObjectExistence = -1;
    };

    // For boat school time changes
    struct BoatSchoolTimes
    {
        float finishTime        = 0.0f;
        float damage            = 0.0f;
        float overallTime       = 0.0f;
        int   type              = -1;
        int   tempActualTime    = -1;
        int   oldRecord         = -1;
        bool  newRecordAchieved = true;
    };

    static inline CVector lastPlayerPos;

    void ApplyEOTLFixes (int newFiretruck);

    void
    SetPlayerAsInFlyingCar (bool setting)
    {
        mInFlyingCar = setting;
    }

    void
    SaveTestTime (int time)
    {
        mOriginalTestTime = time;
    }

    void
    ApplyCarCheckFix (int newVehicle)
    {
        mVehicleDrivingOverride = newVehicle;
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

    int
    GetNewCarForCheck ()
    {
        return mVehicleDrivingOverride;
    }

    int
    GetOriginalTestTime ()
    {
        return mOriginalTestTime;
    }

    bool
    GetIfPlayerInFlyingCar ()
    {
        return mInFlyingCar;
    }
};
