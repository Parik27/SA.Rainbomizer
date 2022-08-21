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

#include <cstdint>
#include <iterator>

#pragma once

/* These contain game functions that might be called by other modules */

struct cVehicleParams;
struct CBaseModelInfo;
struct CClumpModelInfo;
struct CVector;
struct CBox;
struct CColModel;
struct CPhysical;
struct CPool;
struct CEntity;
struct tHandlingData;
struct tFlyingHandlingData;
struct CAnimBlendAssociation;
struct RpClump;

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

enum ePedType
{
    PED_TYPE_PLAYER1,
    PED_TYPE_PLAYER2,
    PED_TYPE_PLAYER_NETWORK,
    PED_TYPE_PLAYER_UNUSED,
    PED_TYPE_CIVMALE,
    PED_TYPE_CIVFEMALE,
    PED_TYPE_COP,
    PED_TYPE_GANG1,
    PED_TYPE_GANG2,
    PED_TYPE_GANG3,
    PED_TYPE_GANG4,
    PED_TYPE_GANG5,
    PED_TYPE_GANG6,
    PED_TYPE_GANG7,
    PED_TYPE_GANG8,
    PED_TYPE_GANG9,
    PED_TYPE_GANG10,
    PED_TYPE_DEALER,
    PED_TYPE_MEDIC,
    PED_TYPE_FIREMAN,
    PED_TYPE_CRIMINAL,
    PED_TYPE_BUM,
    PED_TYPE_PROSTITUTE,
    PED_TYPE_SPECIAL,
    PED_TYPE_MISSION1,
    PED_TYPE_MISSION2,
    PED_TYPE_MISSION3,
    PED_TYPE_MISSION4,
    PED_TYPE_MISSION5,
    PED_TYPE_MISSION6,
    PED_TYPE_MISSION7,
    PED_TYPE_MISSION8
};

enum eWeatherRegion
{
    WEATHER_REGION_DEFAULT,
    WEATHER_REGION_LA,
    WEATHER_REGION_SF,
    WEATHER_REGION_LV,
    WEATHER_REGION_DESERT
};

inline static int *ms_numPedsLoaded = reinterpret_cast<int *> (0x8E4BB0);
inline static int *ms_pedsLoaded    = reinterpret_cast<int *> (0x8E4C00);

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

struct RwTexture
{
    char field_0x00[0x58];
};

struct RpLight
{
    char field_0x00[0x40];
};

struct CCamera
{
    char field_0x00[0xd78];
};

struct CTrain
{
    char    field_0x00[0x5d0];
    CTrain *m_pPrevCarriage;
    CTrain *m_pNextCarriage;
    char    __pad1[0xd4];
};

struct CCarGenerator
{
    int16_t m_nModelId;
    void    DoInternalProcessing ();
    void    CheckForBlockage (int model_id);
};

struct Message
{
    int     m_pText;
    int16_t m_dwFlag;
    char    _pad1[2];
    int     m_dwTime;
    int     m_dwStartTime;
    int     m_dwNumber[6];
    char *  m_pString;
    char    m_bPreviousBrief;
    char    _pad2[3];
};

struct CHud
{
    static void SetMessage (char *str);
};

struct CCoronas
{
    static void RegisterCorona (int ID, CEntity *attachTo, char red, char green,
                                char blue, char alpha, CVector const &posn,
                                float radius, float farClip, int coronaType,
                                int flareType, bool enableReflection,
                                bool checkObstacles, int _param_not_used,
                                float angle, bool longDistance, float nearClip,
                                char fadeState, float fadeSpeed,
                                bool onlyFromBelow, bool reflectionDelay);
    static void RegisterCorona (int ID, CEntity *attachTo, char red, char green,
                                char blue, char alpha, CVector const &posn,
                                float radius, float farClip, RwTexture *texture,
                                int flareType, bool enableReflection,
                                bool checkObstacles, int _param_not_used,
                                float angle, bool longDistance, float nearClip,
                                char fadeState, float fadeSpeed,
                                bool onlyFromBelow, bool reflectionDelay);
};

struct CSprite
{
    static void RenderOneXLUSprite (float x, float y, float z, float halfw,
                                    float halfh, char r, char g, char b, char a,
                                    float rhw, char intensity, char udir,
                                    char vdir);
    static void RenderOneXLUSprite_Triangle (float arg1, float arg2, int arg3,
                                             int arg4, int arg5, int arg6,
                                             float arg7, char r, char g, char b,
                                             char a, float arg12, char arg13);
    static void RenderOneXLUSprite_Rotate_Aspect (float x, float y, float z,
                                                  float halfw, float halfh,
                                                  char r, char g, char b,
                                                  char a, float rhw,
                                                  float arg11, char intensity);
    static void RenderBufferedOneXLUSprite (float x, float y, float z, float w,
                                            float h, char r, char g, char b,
                                            char a, float recipNearZ,
                                            char arg11);
    static void RenderBufferedOneXLUSprite_Rotate_Aspect (
        float x, float y, float z, float w, float h, char r, char g, char b,
        char a, float recipNearZ, float angle, char arg12);
    static void RenderBufferedOneXLUSprite_Rotate_Dimension (
        float x, float y, float z, float w, float h, char r, char g, char b,
        char a, float recipNearZ, float angle, char arg12);
};

struct CMessages
{
    static char *AddMessage (char *string, int time, int16_t flags,
                             int8_t bAddToPreviousBrief);
};

struct CCarCtrl
{
    static void *CreateCarForScript (int modelId, float X, float Y, float Z,
                                     char a5);
    static int   ChooseModel (int *type);
};

struct CCivilianPed
{
    void CivilianPed (ePedType type, unsigned int modelIndex);
};

struct CVector
{
    float x;
    float y;
    float z;
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
    void SetRotateZOnly (float angle);
};

struct CMatrixLink
{
    CMatrix matrix;
};

struct cSimpleTransform
{
    CVector m_vPosn;
    float   m_fAngle;
};

struct CAEPedAudioEntity
{
    char field_0x00[0x15c];
};

struct CAEPedSpeechAudioEntity
{
    char field_0x00[0x100];
};

struct CEntity
{
    int              vtable;
    cSimpleTransform m_SimpleTransform;
    CMatrixLink *    m_pMatrix;
    union
    {
        struct RwObject *m_pRwObject;
        struct RpClump * m_pRwClump;
        struct RpAtomic *m_pRwAtomic;
    };

    char  __pad01C[4];
    short m_nRandomSeed;
    short m_nModelIndex;
    char  __pad024[0x14];

    cSimpleTransform *GetPosition ();
    int               SetHeading (float heading);
    int               GetHeading ();
};

struct CPhysical : public CEntity
{
    char    __pad038[0xc];
    CVector m_vecMoveSpeed;
    CVector m_vecTurnSpeed;
    char    __pad05c[0xdc];
};

struct CPed : public CPhysical
{
    CAEPedAudioEntity       m_pedAudio;
    CAEPedSpeechAudioEntity m_pedSpeech;
    char                    __pad394[216];
    int                     flags[4];
    char                    __pad47C[272];
    CEntity *               m_pVehicle;
    int                     field_590;
    int                     field_594;
    int                     m_nPedType;

    int   GiveWeapon (int weapon, int ammo, int slot);
    void  SetCurrentWeapon (int slot);
    void *CCopPed__CCopPed (int type);
    void  SetModelIndex (int modelIndex);
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
    int            m_nWakeTime;
    unsigned short m_nLogicalOp;
    bool           m_bNotFlag;
    bool           m_bWastedBustedCheck;
    bool           m_bWastedOrBusted;

private:
    char _padD5[3];

public:
    unsigned char *m_pSceneSkipIP;
    bool           m_bIsMission;

private:
    char _padDD[3];

public:
    char *GetPointerToScriptVariable (int a2);
    void  ReadTextLabelFromScript (char *text, char length);
    void  CollectParameters (short num);
    bool  CheckName (const char *name);
    void  ProcessCommands1526to1537 (int opcode);
    char  ProcessCommands0to99 (int opcode);
    void  UpdateCompareFlag (char flag);
    int   EndThread ();
    char  ProcessOneCommand ();

    void Init ();

    static void SetCharCoordinates (CPed *ped, CVector pos, bool bWarpGang,
                                    bool bOffset);
};

struct CIplStore
{
    static int     FindIplSlot (char *name);
    static CPool *&ms_pPool;
};

struct CRunningScripts
{
    static CRunningScript *&pActiveScript;
    static bool             CheckForRunningScript (const char *thread);
};

struct CAEFrontendAudioEntity
{
    char field_0x00[0x9c];
};

struct CAudioEngine
{
    char                   field_0x00[0xb4];
    CAEFrontendAudioEntity m_FrontendAudio;

    void PreloadMissionAudio (unsigned char slot, int id);
    bool GetMissionAudioLoadingStatus (unsigned char id);
    bool IsMissionAudioSampleFinished (unsigned char slot);
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
    char      field_20;
    char      haveTabl;
    char      cderrorInitialized;
    char      missionLoaded;

    void  Load (char a2);
    char *Get (char *key);
    char  LoadMissionText (const char *table);
};

struct IplDef
{
    unsigned char __pad00[0x2D];
    bool          field2D;
    bool          m_bLoadRequest;
    bool          m_bDisableDynamicStreaming;
    bool          field30;
    bool          field31;
    unsigned char __pad32[2];
};

struct CPool
{
    char *m_pObjects;
    char *m_byteMap;
    int   m_nSize;

    template <typename T>
    T *
    GetAt (signed int handle)
    {
        return (T *) (this->m_pObjects + sizeof (T) * handle);
    }
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
    static bool IsRCModel (int modelId);
    static int  LoadVehicleColours ();
    static int  GetMaximumNumberOfPassengersFromNumberOfDoors (int modelIndex);
    static int  GetModelInfo (const char *modelName, short *index);
};

struct CAEVehicleAudioEntity
{
    void ProcessTrainTrackSound (void *vehicle_info);
};

struct CAEAudioUtility
{
    static char GetBankAndSoundFromScriptSlotAudioEvent (int *      wavId,
                                                         int *      out_sfx,
                                                         int *      out_bank,
                                                         signed int slot);
};

struct BankLkup
{
    uint8_t  sfxIndex;
    char     pad[3];
    uint32_t m_dwOffset;
    uint32_t m_dwLength;
};

struct PakFile
{
    uint32_t soundBufferOffset;
    int32_t  sampleRate;
    int16_t  loopOffset;
    uint16_t soundHeadroom;
};

struct CAEMp3BankLoader
{
    void *    m_pBankSlotsInfo;
    BankLkup *m_pBankLkups;
    char *    m_pPakFileNames;
    uint16_t  m_nNumBankSlotInfos;
    uint16_t  m_nNumBankLkups;
    uint16_t  m_nNumPakFiles;

    int  GetLoopOffset (unsigned short sfxId, short bankSlotInfoId);
    char Initialise ();
};

struct CGenericGameStorage
{
    static char *MakeValidSaveFileName (int saveNum);
    static int   SaveDataToWorkBuffer (void *pSource, int size);
    static int   LoadDataFromWorkBuffer (void *pSource, int size);
    static char  GenericSave ();
    static int & length;
};

struct CStats
{
    static double GetStatValue (short id);
    static void   IncrementStat (short id, float val);
};

struct CBox
{
public:
    CVector m_vecMin;
    CVector m_vecMax;
};

struct CColData
{
    uint8_t   pad[0x10];
    uintptr_t m_pLines;
};

struct CColModel
{
    CBox      m_boundBox;
    uint8_t   m_Sphere[0x14];
    CColData *m_pColData;
};

struct CVehicleModelInfo
{
    char field_0x00[0x32];
    char m_szGameName[8];
    char _pad[0x2ce];
};

struct CVehicle
{
    uint8_t              __pad[0x22];
    uint16_t             m_nModelIndex;
    uint8_t              __pad24[104];
    float                m_fMass;
    float                m_fTurnMass;
    uint8_t              __pad94[752];
    tHandlingData *      m_pHandling;
    tFlyingHandlingData *m_pFlyingHandling;
    uint8_t              m_nHandlingFlags[4];
    uint8_t              __pad390[208];
    CPed *               m_pDriver;
    uint8_t              __pad464[72];
    uint8_t              m_nPacMansCollected;
    int                  GetVehicleAppearence ();
    void                 AutomobilePlaceOnRoadProperly ();
    void                 BikePlaceOnRoadProperly ();
    char                 SetGearUp ();
    void                 SetDriver (CPed *driver);
};

struct CPickups
{
    static int GenerateNewOne (float x, float y, float z, unsigned int modelId,
                               char pickupType, int ammo,
                               unsigned int moneyPerDay, char isEmpty,
                               char *message);
};

struct CBaseModelInfo
{
public:
    char             pad[0x10];
    CColModel *      m_pColModel;
    float            m_fDrawDistance;
    struct RwObject *m_pRwObject;

    virtual void    Destructor ();
    virtual void    AsAtomicModelInfoPtr ();
    virtual void    AsDamageAtomicModelInfoPtr ();
    virtual void    AsLodAtomicModelInfoPtr ();
    virtual uint8_t GetModelType ();
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
    static void RemoveLeastUsedModel (int flags);
    static void RequestSpecialModel (int slot, const char *modelName,
                                     int flags);
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

struct RwRGBAReal
{
    float r;
    float g;
    float b;
    float a;
};

struct tTransmissionGear
{
    float m_fMaxVelocity;
    float m_fChangeUpVelocity;
    float m_fChangeDownVelocity;
};

struct CWeaponInfo
{
public:
    unsigned int m_eWeaponFire;
    float        m_fTargetingRange;
    float        m_fFiringRange;
    long         m_nModel1;
    long         m_nModel2;
    unsigned int m_nWeaponSlot;
    unsigned int m_nFlags;
    unsigned int m_nAssocGroupId;
    short        m_nAmmoClipSize;
    short        m_nDamage;
    CVector      m_vecFireOffset;
    unsigned int m_nSkillLevel;
    unsigned int m_nRequiredStatValue;
    float        m_fAccuracy;
    float        m_fMoveSpeed;
    float        m_fAnimLoopStart;
    float        m_fAnimLoopEnd;
    float        m_fAnimFrameFire;
    float        m_fAnim2LoopStart;
    float        m_fAnim2LoopEnd;
    float        m_fAnim2FrameFire;
    float        m_fAnimBreakout;
    float        m_fSpeed;
    float        m_fRadius;
    float        m_fLifeSpan;
    float        m_fSpread;
    int          m_nUndefined;
};

struct RwV3d
{
    float x;
    float y;
    float z;
};

struct RtQuat
{
    RwV3d imag;
    float real;
};

struct CFileObjectInstance
{
    RwV3d  position;
    RtQuat rotation;
    int    modelId;
    int    interiorId;
    int    lod;
};

struct CTransmissionGear
{
    int   m_fMaxVelocity;
    float m_fChangeUpVelocity;
    float m_fChangeDownVelocity;
};

struct cTransmission
{
    CTransmissionGear m_aGears[6];
    char              m_nDriveType;
    char              m_nEngineType;
    char              m_nNumberOfGears;
    char              field_4B;
    int               m_dwHandlingFlags;
    float             m_fEngineAcceleration;
    float             m_fEngineInertia;
    float             m_fMaxGearVelocity;
    int               field_5C;
    float             m_fMinGearVelocity;
    float             m_fCurrentSpeed;
};

struct tHandlingData
{
    int           index;
    float         fMass;
    float         field_8;
    float         fTurnMass;
    float         fDragMult;
    RwV3d         centreOfMass;
    char          nPercentSubmerged;
    char          field_21;
    char          field_22;
    char          field_23;
    float         fBuoyancyConstant;
    int           fTractionMultiplier;
    cTransmission transmissionData;
    float         fBrakeDeceleration;
    int           fBrakeBias;
    char          bABS;
    char          field_9D;
    char          field_9E;
    char          field_9F;
    float         fSteeringLock;
    int           fTractionLoss;
    int           fTractionBias;
    float         fSuspensionForceLevel;
    int           fSuspensionDampingLevel;
    int           fSuspensionHighSpdComDamp;
    float         fSuspensionUpperLimit;
    float         fSuspensionLowerLimit;
    int           fSuspensionBiasBetweenFrontAndRear;
    int           fSuspensionAntiDiveMultiplier;
    float         fCollisionDamageMultiplier;
    unsigned int  modelFlags;
    int           handlingFlags;
    int           fSeatOffsetDistance;
    int           nMonetaryValue;
    char          frontLights;
    char          rearLights;
    char          animGroup;
    char          field_DF;
};

struct CHandlingBike
{
    int   index;
    int   leanFwdCOM;
    int   leanFwdForce;
    int   leanBakCOM;
    int   leanBakForce;
    int   maxLean;
    float fullAnimLean;
    int   desLean;
    int   speedSteer;
    int   slipSteer;
    int   noPlayerCOMz;
    float wheelieAng;
    float stoppieAng;
    int   wheelieSteer;
    int   wheelieStabMult;
    int   stoppieStabMult;
};

struct CHandlingFlying
{
    int   index;
    int   thrust;
    int   thrustFallOff;
    int   yaw;
    int   yawStab;
    int   sideSlip;
    int   roll;
    int   rollStab;
    int   pitch;
    int   pitchStab;
    int   formLift;
    int   attackLift;
    int   gearUpR;
    int   gearDownL;
    int   windMult;
    int   moveRes;
    RwV3d turnRes;
    RwV3d speedRes;
};

struct tBoatHandlingData
{
    int   index;
    float thrustY;
    float thrustZ;
    float thrustAppZ;
    float aqPlaneForce;
    float aqPlaneLimit;
    float aqPlaneOffset;
    float waveAudioMult;
    int   look_L_R_BehindCamHeight;
    RwV3d moveRes;
    RwV3d turnRes;
};

struct cHandlingDataMgr
{
    int               LoadHandlingData ();
    float             field_0;
    float             field_4;
    int               field_8;
    float             field_C;
    int               field_10;
    tHandlingData     vehicleHandling[210];
    CHandlingBike     bikeHandling[13];
    CHandlingFlying   flyingHandling[24];
    tBoatHandlingData boatHandling[12];
};

enum eFontAlignment
{
    ALIGN_CENTRE,
    ALIGN_LEFT,
    ALIGN_RIGHT
};

struct CRGBA
{
    union
    {
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
        uint32_t rgba;
    };

    CRGBA () : r (255), g (255), b (255), a (255) {}

    CRGBA (int r, int g, int b, int a = 255)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
};

struct CFont
{
    static void SetOrientation (eFontAlignment alignment);
    static void SetColor (CRGBA col);
    static void SetAlphaFade (float alpha);
    static void SetFontStyle (short style);
    static void SetWrapx (float value);
    static void SetDropShadowPosition (short value);
    static void SetBackground (bool enable, bool includeWrap);
    static void SetBackgroundColor (CRGBA col);
    static void SetScaleForCurrentlanguage (float w, float h);
    static void SetJustify (bool on);
    static void PrintString (float x, float y, char *text);
};

struct CFileMgr
{
    static char *ms_dirName;
};

struct CRect
{
    float left;
    float bottom;
    float right;
    float top;
};

struct CMenuManager
{
    char  field_0x00[0x3c];
    float m_dwBrightness;
};

struct CPlayerInfo
{
    CPed *m_pPed;
    char  field_0x00[0x18c];
};

struct CObject : public CEntity
{
};

#pragma pack(push, 1)
class CCompressedVector
{
private:
    uint16_t x;
    uint16_t y;
    uint16_t z;

public:
    float
    GetX ()
    {
        return x / 8.0f;
    }

    float
    GetY ()
    {
        return y / 8.0f;
    }

    float
    GetZ ()
    {
        return z / 8.0f;
    }

    void
    SetX (float x)
    {
        this->x = x * 8.0f;
    }

    void
    SetY (float y)
    {
        this->y = y * 8.0f;
    }

    void
    SetZ (float z)
    {
        this->z = z * 8.0f;
    }

    CCompressedVector () = default;
    CCompressedVector (float x, float y, float z)
    {
        SetX (x);
        SetY (y);
        SetZ (z);
    }
};
#pragma pack(pop)

struct CPickup
{
    float             m_fRevenueValue;
    CObject *         m_pObject;
    int               m_dwAmmo;
    int               m_dwRegenerationTime;
    CCompressedVector m_vPos;
    short             m_nMoneyPerDay;
    short             m_wModelId;
    short             m_wReferenceIndex;
    char              m_PickupType;
    char              m_nFlags;
    char              _pad[2];
};

struct CZone
{
    char  name[8];
    char  m_szTextKey[8];
    short x1;
    short y1;
    short z1;
    short x2;
    short y2;
    short z2;
    char  __pad[4];
};

struct RsGlobalType
{
    char *appName;
    int   MaximumWidth;
    int   MaximumHeight;
};

template <typename T> struct ListItem_c
{
    T *next;
    T *prev;
};

struct FxSystemBP_c
{
    ListItem_c<FxSystemBP_c> m_link;
    uint32_t                 m_nKey;
    float                    m_fLength;
    float                    m_fLoop;
};

template <typename T> struct List_c
{
    T *      last;
    T *      first;
    uint32_t count;
};

struct FxManager_c
{
    List_c<FxSystemBP_c> SystemBlueprints;
};

union ShoppingHash
{
    int32_t hash;
    int32_t weaponType;
    int16_t modelId;
    char    data[6];
};

struct CShoppingItem
{
    int  textureName;
    int  unk;
    int  modelName;
    int  modelType;
    char gxtEntry[8];
};

static_assert (sizeof (CShoppingItem) == 24, "incorrect size");

struct CShopping
{
    static void LoadShop (const char *name); // 0x49BBE0
    static void LoadShoppingType (const char *type);

    static int &m_nCurrentShoppingType;
    static int &m_nTotalItems;

    static CShoppingItem *m_aShoppingItems;
};

struct CEnterExit
{
    char     m_szInteriorName[8];
    CRect    m_rEnterRect;
    float    m_fEnterZ;
    float    m_fEnterAngle;
    CVector  m_vExitPos;
    float    m_fExitAngle;
    uint16_t m_wFlags;
    int8_t   m_bInteriorId;
    int8_t   m_bSkyColour;
    int8_t   m_bTimeOff;
    int8_t   m_bTimeOn;
    int8_t   m_nNumSpawnedPedsInInterior;
    int8_t   padding0037[2];

    static CPool *&mp_poolEntryExits;
    static bool    GetInteriorStatus (const char *name);
};

static_assert (sizeof (CEnterExit) == 0x3C, "CEnterExit is the wrong size");

struct CWorld
{
    static double FindGroundZedForCoord (float x, float y);
};

CVector   FindPlayerCoors (int playerId = -1);
CVehicle *FindPlayerVehicle (int playerId = -1, bool bIncludeRemote = false);
CPed *    FindPlayerPed (int playerId = -1);
CEntity * FindPlayerEntity (int playerId = -1);
float     Dist (CVector a, CVector b);

struct C3dMarker
{
    char  __pad00[0x58];
    CRGBA colour;
};

struct CGame
{
    static unsigned char &bMissionPackGame;
    static int            Init2 (void *fileName);
    static int            Init3 (void *fileName);
};

struct CTimeCycleCurrent
{
    float m_fAmbientRed;
    float m_fAmbientGreen;
    float m_fAmbientBlue;
    float m_fAmbientRed_Obj;
    float m_fAmbientGreen_Obj;
    float m_fAmbientBlue_Obj;
    float m_fDirectionalRed;
    float m_fDirectionalGreen;
    float m_fDirectionalBlue;
    short m_wSkyTopRed;
    short m_wSkyTopGreen;
    short m_wSkyTopBlue;
    short m_wSkyBottomRed;
    short m_wSkyBottomGreen;
    short m_wSkyBottomBlue;
    short m_wSunCoreRed;
    short m_wSunCoreGreen;
    short m_wSunCoreBlue;
    short m_wSunCoronaRed;
    short m_wSunCoronaGreen;
    short m_wSunCoronaBlue;
    float m_fSunSize;
    float m_fSpriteSize;
    float m_fSpriteBrightness;
    short m_wShadowStrength;
    short m_wLightShadowStrength;
    short m_wPoleShadowStrength;
    char  _padding0[2];
    float m_fFarClip;
    float m_fFogSt;
    float m_fLightOnGround;
    short m_wLowCloudsRed;
    short m_wLowCloudsGreen;
    short m_wLowCloudsBlue;
    short m_wBottomCloudsRed;
    short m_wBottomCloudsGreen;
    short m_wBottomCloudsBlue;
    float m_fWaterRed;
    float m_fWaterGreen;
    float m_fWaterBlue;
    float m_fWaterAlpha;
    float m_fRGB1_R;
    float m_fRGB1_G;
    float m_fRGB1_B;
    float m_fAlpha1;
    float m_fRGB2_R;
    float m_fRGB2_G;
    float m_fRGB2_B;
    float m_fAlpha2;
    float m_fCloudAlpha1;
    long  m_dwCloudAlpha2;
    short m_wCloudAlpha3;
    char  _padding1[2];
    float m_fIllumination;
    char  field_A8[4];

    static CTimeCycleCurrent *GetInfo (CTimeCycleCurrent *timecyc,
                                       int weatherID, int timeID);
};

struct CAnimBlock
{
    char szName[16];
    bool bLoaded;
    bool pad;
};

struct CAnimBlendAssocGroup
{
    CAnimBlock *pAnimBlock;
    void *      ppAssociations;
    uint32_t    iNumAnimations;
    uint32_t    iIDOffset;
    uint32_t    groupId;

    inline static CAnimBlendAssocGroup *&ms_aAnimAssocGroups
        = *(CAnimBlendAssocGroup **) 0xB4EA34;

    inline static int &ms_numAnimAssocDefinitions = *(int *) 0xB4EA28;

    CAnimBlendAssociation *CopyAnimation (int Id);
};

struct CAnimationStyleDescriptor
{
    char                  groupName[16];
    char                  blockName[16];
    uint32_t              field_20;
    uint32_t              animsCount;
    char **               animNames;
    uint32_t              animDesc;
    static unsigned char &bMissionPackGame;
    static int            Init3 (void *fileName);
};

void GivePlayerRemoteControlledCar (float x, float y, float z, float angle,
                                    short model);

CMatrix *              RwFrameGetLTM (void *frame);
CAnimBlendAssociation *RpAnimBlendClumpExtractAssociations (RpClump *clump);
void                   RpAnimBlendClumpGiveAssociations (RpClump *              clump,
                                                         CAnimBlendAssociation *association);
RpLight *              RpLightSetColor (RpLight *light, RwRGBAReal *colour);

int    random (int max);
int    random (int min, int max);
double randomNormal (double mean, double stddev);
float  randomFloat (float min, float max);

template <typename T>
auto &
GetRandomElement (T &container)
{
    auto it = std::begin (container);
    std::advance (it, random (std::size (container) - 1));

    return *it;
}

template <typename T>
auto &
GetRandomElement (T *container, uint32_t size)
{
    return container[random (size - 1)];
}

CMatrix *RwFrameGetLTM (void *frame);

extern CPickup *        aPickups;
extern CStreamingInfo * ms_aInfoForModel;
extern CBaseModelInfo **ms_modelInfoPtrs;
extern RwRGBA *         ms_vehicleColourTable;
extern int *            ScriptParams;
extern int *            ScriptSpace;
extern CPool *&         ms_pPedPool;
extern CPool *&         ms_pVehiclePool;
extern CWeaponInfo *    aWeaponInfos;
extern RsGlobalType *   RsGlobal;
extern float *          ms_fTimeStep;
