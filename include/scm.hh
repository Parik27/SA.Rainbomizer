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

enum class CarType
{
    CAR,
    BIKE,
    PLANE,
    HELI,
    BOAT,
    TRAIN,
    TRAILER
};

#include <vector>
#include <cstdint>
#include <cstdio>
#include <string>
#include <map>
#include <unordered_map>
#include "scm_patterns.hh"
#include <array>

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
void ChangePlayerVehicle (int mission);

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
    static ScriptVehicleRandomizer *                mInstance;
    static inline std::vector<ScriptVehiclePattern> mPatterns;

    ScriptVehicleRandomizer (){};
    static void DestroyInstance ();

    bool mPosFixEnabled          = false;
    int  mVehicleDrivingOverride = -1;
    int  mOriginalTestTime       = 0;
    bool mInFlyingCar            = false;

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
        bool Vigilante;
        bool Burglary;
        bool Pimping;
        bool Paramedic;
        bool Courier;
        bool Bike;

        bool OldAirRaid;

        int  ForcedVehicle;
        bool GenericPatterns;
        bool SkipChecks;

    } m_Config;

    static inline std::array<std::vector<int>, 7> carTypes;

    uint8_t     mSeatsCache[212];
    std::string mLastThread = "";

    static inline std::vector<std::string> recognisedScripts
        = {"initial", "initil2", "intro",   "none",    "dual",    "shtr",
           "grav",    "otb",     "pool2",   "lowr",    "zero5",   "intro1",
           "intro2",  "sweet1",  "sweet1b", "sweet3",  "sweet2",  "sweet4",
           "hoods5",  "sweet6",  "sweet7",  "crash4",  "crash1",  "drugs3",
           "guns1",   "ryder3",  "ryder2",  "twar7",   "smoke2",  "smoke3",
           "drugs1",  "music1",  "music2",  "music3",  "music5",  "cprace",
           "cesar1",  "drugs4",  "la1fin2", "bcrash1", "catalin", "cat1",
           "cat2",    "cat3",    "cat4",    "catcut",  "truth1",  "truth2",
           "bcesar4", "garag1",  "decon",   "scrash3", "scrash2", "wuzi1",
           "farlie4", "driv6",   "wuzi2",   "wuzi5",   "syn1",    "syn2",
           "syn3",    "synd4",   "syn6",    "syn7",    "syn5",    "driv2",
           "driv3",   "steal1",  "steal2",  "steal4",  "steal5",  "dskool",
           "zero1",   "zero2",   "zero4",   "toreno1", "toreno2", "des3",
           "desert4", "desert6", "desert9", "maf4",    "des10",   "desert5",
           "casino1", "casino2", "casino3", "casino7", "casino4", "casino5",
           "casino6", "casino9", "casin10", "vcrash1", "vcr2",    "doc2",
           "heist1",  "heist3",  "heist2",  "heist4",  "heist5",  "heist9",
           "mansio1", "mansio2", "mansio3", "manson5", "grove1",  "grove2",
           "riot1",   "riot2",   "finalea", "finaleb", "finalec", "shrange",
           "gymls",   "gymsf",   "gymlv",   "truck",   "quarry",  "boat",
           "bskool",  "taxiodd", "ambulan", "firetru", "copcar",  "burgjb",
           "freight", "pimp",    "blood",   "kicksta", "tria",    "bcour",
           "mtbiker", "stunt",   "buypro1", "plchute", "parach",  "bcesar2",
           "cokec",   "bandit",  "roulete", "otb_msc", "arcade",  "vending",
           "fodvend", "gateos",  "gymbike", "gymbenc", "gymtrea", "gymdumb",
           "bball",   "vidpok",  "blackj",  "wof",     "dealer",  "hmles",
           "pool",    "lowrcon", "burglar", "gfmeet",  "gfdate",  "gfsex",
           "casamb",  "bar",     "pshop",   "otbshp",  "strip",   "planes",
           "trains",  "rcshop",  "dance",   "skbrain", "ffpnc",   "barstaf",
           "bouncer", "otbstaf", "pchair",  "pcustom", "otbwtch", "otbslp",
           "otbtill", "fboothr", "fboothl", "barguy",  "pedroul", "pedcard",
           "pedslot", "dancer",  "stripw",  "stripm",  "browse",  "copsit",
           "coplook", "ticket",  "shopper", "amunat",  "tatto",   "barb",
           "warrobe", "cloth",   "jfud",    "carmod",  "crane1",  "crane2",
           "crane3",  "carprk1", "impnd",   "valet",   "photo",   "prisonr",
           "camera",  "debt",    "hotdogv"};

    static inline std::map<int, std::string> oddMissions{
        {121, "taxiodd"}, {122, "ambulan"}, {123, "firetru"},
        {124, "copcar"},  {125, "burgjb"},  {127, "pimp"},
        {131, "bcour"},   {132, "mtbiker"}, {133, "stunt"}};

    static inline int mCurrentMissionRunning = -1;
    static inline int mCourierMissionType    = -1;
    static inline int mTempVehHandle         = -1;

    static inline bool mDes3Stuck           = false;
    static inline bool mEOTL3Slow           = false;
    static inline int  mNextVigilanteCarNum = 0;
    static inline bool mVigilanteSlotsTaken = false;

    static inline int mFallbackVeh = -1;

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
