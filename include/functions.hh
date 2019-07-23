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

/* These contain game functions that might be called by other modules */

struct cVehicleParams;
struct CBaseModelInfo;
struct CClumpModelInfo;
struct CVector;
struct CBox;
struct CColModel;

enum eVehicleClass
{
    VEHICLE_AUTOMOBILE = 0,
    VEHICLE_MTRUCK,
    VEHICLE_QUAD,
    VEHICLE_HELI,
    VEHICLE_PLANE,
    VEHICLE_BOAT,
    VEHICLE_TRAIN,
    VEHICLE_FHELI,
    VEHICLE_FPLANE,
    VEHICLE_BIKE,
    VEHICLE_BMX,
    VEHICLE_TRAILER,
};

struct cVehicleParams
{
public:
    unsigned int   m_nVehicleSubclass;
    unsigned int   m_nVehicleClass;
    char           field_8;
    char           pad1[3];
    int            field_C;
    void *         m_pVehicle;
    void *         m_pTransmission;
    unsigned int   m_nModelType;
    float          m_fVelocity;
    short          m_nGasState;
    short          m_nBreakState;
    float          m_fVelocityAbsolute;
    float          m_fZVelocity;
    float          m_fVelocityPercentage;
    int            field_30;
    float          field_34;
    char           m_nCurrentGear;
    bool           m_bHandbrakeOn;
    char           pad2[2];
    float          m_fVelocityChangingPercentage;
    float          m_fWheelSpinForAudio;
    unsigned short m_nNumberOfGears;
    unsigned char  m_nWheelsOnGround;
    char           field_47;
    int            field_48;
};

struct CCarGenerator
{
    void CheckForBlockage (int model_id);
};

struct CCarCtrl
{
    static void *CreateCarForScript (int modelId, float X, float Y, float Z,
                                     char a5);
};

struct CRunningScript
{
public:
    CRunningScript *m_pNext;
    CRunningScript *m_pPrev;
    char            m_szName[8];
    unsigned char * m_pBaseIP;
    unsigned char * m_pCurrentIP;
    unsigned char * m_apStack[8];
    unsigned short  m_nSP;

private:
    char _pad3A[2];

public:
    int  m_aLocalVars[32];
    int  m_anTimers[2];
    bool m_bIsActive;
    bool m_bCondResult;
    bool m_bUseMissionCleanup;
    bool m_bIsExternal;
    bool m_bTextBlockOverride;

private:
    char _padC9[3];

public:
    void CollectParameters (short num);
	bool CheckName(const char* name);
	void ProcessCommands1526to1537(int opcode);
	void UpdateCompareFlag(char flag);
};

struct CAudioEngine
{
    void PreloadMissionAudio (unsigned char slot, int id);
};

struct CKeyGen
{
    static int GetUppercaseKey (const char *string);
};

struct TKey
{
    char *pos;
    int   hash;
};

struct CKeyArray
{
    TKey *data;
    short size;
};

struct CData
{
    char *data;
    int   size;
};

struct CText
{
    CKeyArray tKeyMain;
    CData     tDataMain;
    CKeyArray tKeyMission;
    CData     tDataMission;

    void  Load (char a2);
    char *Get (char *key);
    char  LoadMissionText (const char *table);
};

struct CPool
{
    void **      m_pObjects;
    void *GetAt (signed int handle, int size);
};

struct CPad
{
    static void DoCheats ();
};

struct CVehicleRecording
{
    static void SetPlaybackSpeed (void *a1, float a2);
};

struct CPopulation
{
    static void *AddPed (int type, int model, float *posn, bool unk);
};

struct CCarAI
{
    static void AddPoliceCarOccupants (void *vehicle, char a3);
};

struct CModelInfo
{
    static bool IsBoatModel (int modelId);
    static bool IsCarModel (int modelId);
    static bool IsTrainModel (int modelId);
    static bool IsHeliModel (int modelId);
    static bool IsPlaneModel (int modelId);
    static bool IsBikeModel (int modelId);
    static bool IsFakePlaneModel (int modelId);
    static bool IsMonsterTruckModel (int modelId);
    static bool IsQuadBikeModel (int modelId);
    static bool IsBmxModel (int modelId);
    static bool IsTrailerModel (int modelId);
    static bool IsPoliceModel (int modelId);
    static int  LoadVehicleColours ();
    static int  GetMaximumNumberOfPassengersFromNumberOfDoors (int modelIndex);
};

struct CAEVehicleAudioEntity
{
    void ProcessTrainTrackSound (void *vehicle_info);
};

struct CVector
{
    float x;
    float y;
    float z;
};

struct CBox
{
public:
    CVector m_vecMin;
    CVector m_vecMax;
};

struct CColModel
{
    CBox m_boundBox;
};

struct CVehicle
{
    int GetVehicleAppearence ();
};

struct CBaseModelInfo
{
public:
    char             pad[0x14];
    CColModel *      m_pColModel;
    float            m_fDrawDistance;
    struct RwObject *m_pRwObject;
};

struct CClumpModelInfo : public CBaseModelInfo
{
public:
    CBox *GetBoundingBox ();
};

struct CLoadedCarGroup
{
public:
    unsigned short m_members[20];

private:
    unsigned short unk_2c;

public:
    int CountMembers ();
    int PickRandomCar (bool checkUsage, bool arg2);
};

struct CStreaming
{
    static CLoadedCarGroup *ms_nVehiclesLoaded;

    static int  GetDefaultCopCarModel (int a1);
    static void RequestModel (int model, int flags);
    static void LoadAllRequestedModels (bool bOnlyPriority);
    static void RemoveModel (int model);
    static void SetMissionDoesntRequireModel (int index);
    static void SetIsDeletable (int model);
};

struct CStreamingInfo
{
public:
    short         m_nNextIndex; // ms_pArrayBase array index
    short         m_nPrevIndex; // ms_pArrayBase array index
    short         m_nNextIndexOnCd;
    unsigned char m_nFlags; // see eStreamingFlags
    unsigned char m_nImgId;
    unsigned int  m_nCdPosn;
    unsigned int  m_nCdSize;
    unsigned char m_nLoadState; // see eStreamingLoadState
private:
    char __pad[3];
};

struct RwRGBA
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct CMatrix
{
    CVector      right;
    unsigned int flags;
    CVector      up;
    unsigned int pad1;
    CVector      at;
    unsigned int pad2;
    CVector      pos;
    unsigned int pad3;

    void Attach (CMatrix *attach, char link);
};

struct tTransmissionGear
{
    float m_fMaxVelocity;
    float m_fChangeUpVelocity;
    float m_fChangeDownVelocity;
};

class cTransmission
{
public:
    tTransmissionGear m_aGears[6];
    unsigned char     m_nDriveType;
    unsigned char     m_nEngineType;
    unsigned char     m_nNumberOfGears;
    char              field_4B;
    unsigned int      m_nHandlingFlags;
    float             m_fEngineAcceleration;
    float             m_fEngineInertia;
    float             m_fMaxGearVelocity;
    int               field_5C;
    float             m_fMinGearVelocity;
    float             m_fCurrentSpeed;
};

struct tHandlingData
{
    int           m_nVehicleId;
    float         m_fMass;
    float         field_8;
    float         m_fTurnMass;
    float         m_fDragMult;
    CVector       m_vecCentreOfMass;
    unsigned char m_nPercentSubmerged;
    float         m_fBuoyancyConstant;
    float         m_fTractionMultiplier;
    cTransmission m_transmissionData;
    float         m_fBrakeDeceleration;
    float         m_fBrakeBias;
    char          m_bABS;
    char          field_9D;
    char          field_9E;
    char          field_9F;
    float         m_fSteeringLock;
    float         m_fTractionLoss;
    float         m_fTractionBias;
    float         m_fSuspensionForceLevel;
    float         m_fSuspensionDampingLevel;
    float         m_fSuspensionHighSpdComDamp;
    float         m_fSuspensionUpperLimit;
    float         m_fSuspensionLowerLimit;
    float         m_fSuspensionBiasBetweenFrontAndRear;
    float         m_fSuspensionAntiDiveMultiplier;
    float         m_fCollisionDamageMultiplier;
    int           m_nModelFlags;
    int           m_nHandlingFlags;
    float         m_fSeatOffsetDistance;
    unsigned int  m_nMonetaryValue;
    unsigned char m_nFrontLights;
    unsigned char m_nRearLights;
    unsigned char m_nAnimGroup;
};

struct tBikeHandlingData
{
    unsigned char data[0x40];
};

struct tPlaneHandlingData
{
    unsigned char data[0x58];
};

struct tBoatHandlingData
{
    unsigned char data[0x3C];
};

struct cHandlingDataMgr
{
    int                unkFields[5];
    tHandlingData      vehicleHandling[210];
    tBikeHandlingData  bikeHandling[13];
    tPlaneHandlingData planeHandling[24];
    tBoatHandlingData  boatHandling[12];

    int LoadHandlingData ();
};

struct cSimpleTransform
{
	CVector m_vPosn;
	float m_fAngle;
};

CVector FindPlayerCoors(int playerId=0);
float Dist(CVector a, CVector b);

struct CEntity
{
	cSimpleTransform* GetPosition();
};

CMatrix *
RwFrameGetLTM (void *frame);

int random (int max, int min = 0);

extern CStreamingInfo * ms_aInfoForModel;
extern CBaseModelInfo **ms_modelInfoPtrs;
extern RwRGBA *         ms_vehicleColourTable;
extern int *            ScriptParams;
extern CPool*           ms_pPedPool;
