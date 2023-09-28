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

#include "functions.hh"
#include "base.hh"
#include <cstring>
#include <cmath>
#include <random>
#include "config.hh"
#include <ctime>
#include "injector/calling.hpp"
#include "generalsettings.hh"

/*******************************************************/
int
CStreaming::GetDefaultCopCarModel (int a1)
{
    return CallAndReturn<int, 0x407C50> (a1);
}

/*******************************************************/
void
CStreaming::RequestModel (int model, int flags)
{
    Call<0x4087E0> (model, flags);
}

/*******************************************************/
void
CStreaming::RequestSpecialModel (int slot, const char *modelName, int flags)
{
    Call<0x409D10> (slot, modelName, flags);
}

/*******************************************************/
void *
CPopulation::AddPed (int type, int model, float *posn, bool unk)
{
    return CallAndReturn<void *, 0x612710> (type, model, posn, unk);
}

/*******************************************************/
void
CCarAI::AddPoliceCarOccupants (void *vehicle, char a3)
{
    Call<0x41C070> (vehicle, a3);
}

/*******************************************************/
bool
CModelInfo::IsBoatModel (int model)
{
    return CallAndReturn<bool, 0x4C5A70> (model);
}
/*******************************************************/
bool
CModelInfo::IsCarModel (int model)
{
    return CallAndReturn<bool, 0x4C5AA0> (model);
}
/*******************************************************/
bool
CModelInfo::IsTrainModel (int model)
{
    return CallAndReturn<bool, 0x4C5AD0> (model);
}
/*******************************************************/
bool
CModelInfo::IsHeliModel (int model)
{
    return CallAndReturn<bool, 0x4C5B00> (model);
}
/*******************************************************/
bool
CModelInfo::IsPlaneModel (int model)
{
    return CallAndReturn<bool, 0x4C5B30> (model);
}
/*******************************************************/
bool
CModelInfo::IsBikeModel (int model)
{
    return CallAndReturn<bool, 0x4C5B60> (model);
}
/*******************************************************/
bool
CModelInfo::IsFakePlaneModel (int model)
{
    return CallAndReturn<bool, 0x4C5B90> (model);
}
/*******************************************************/
bool
CModelInfo::IsMonsterTruckModel (int model)
{
    return CallAndReturn<bool, 0x4C5BC0> (model);
}
/*******************************************************/
bool
CModelInfo::IsQuadBikeModel (int model)
{
    return CallAndReturn<bool, 0x4C5BF0> (model);
}
/*******************************************************/
bool
CModelInfo::IsBmxModel (int model)
{
    return CallAndReturn<bool, 0x4C5C20> (model);
}
/*******************************************************/
bool
CModelInfo::IsTrailerModel (int model)
{
    return CallAndReturn<bool, 0x4C5C50> (model);
}

/*******************************************************/
bool
CModelInfo::IsPoliceModel (int modelId)
{
    switch (modelId)
        {
        case 427: // MODEL_ENFORCER
        case 430: // MODEL_PREDATOR
        case 432: // MODEL_RHINO
        case 433: // MODEL_BARRACKS
        case 490: // MODEL_FBIRANCH
        case 523: // MODEL_COPBIKE
        case 528: // MODEL_FBITRUCK
        case 596: // MODEL_COPCARLA
        case 597: // MODEL_COPCARSF
        case 598: // MODEL_COPCARVG
        case 599: // MODEL_COPCARRU
        case 601: // MODEL_SWATVAN
            return true;
        }
    return false;
}

/*******************************************************/
bool
CModelInfo::IsRCModel (int model)
{
    switch (model)
        {
        case 441: // MODEL_RC_BANDIT
        case 464: // MODEL_RC_BARON
        case 594: // MODEL_RC_CAM
        case 501: // MODEL_RC_GOBLIN
        case 465: // MODEL_RC_RAIDER
        case 564: // MODEL_RC_TIGER
            return true;
        }
    return false;
}

/*******************************************************/
int
CCarCtrl::ChooseModel (int *type)
{
    return CallAndReturn<int, 0x424CE0> (type);
}

/*******************************************************/
int
CPed::GiveWeapon (int weapon, int ammo, int slot)
{
    return CallMethodAndReturn<int, 0x5E6080> (this, weapon, ammo, slot);
}

/*******************************************************/
void
CPed::SetCurrentWeapon (int slot)
{
    CallMethodAndReturn<int, 0x5E61F0> (this, slot);
}

/*******************************************************/
void *
CPed::CCopPed__CCopPed (int type)
{
    return CallMethodAndReturn<void *, 0x5DDC60> (this, type);
}
/*******************************************************/
void
CPed::SetModelIndex (int modelIndex)
{
    CallMethod<0x5E4880> (this, modelIndex);
}
/*******************************************************/
int
CModelInfo::LoadVehicleColours ()
{
    return CallAndReturn<int, 0x5B6890> ();
}

/*******************************************************/
void
CStreaming::LoadAllRequestedModels (bool bOnlyPriority)
{
    Call<0x40EA10> (bOnlyPriority);
}

/*******************************************************/
void
CAEVehicleAudioEntity::ProcessTrainTrackSound (void *vehicle_info)
{
    CallMethod<0x4FA3F0> (this, vehicle_info);
}

/*******************************************************/
char
CAEAudioUtility::GetBankAndSoundFromScriptSlotAudioEvent (int *      wavId,
                                                          int *      out_slot,
                                                          int *      out_bank,
                                                          signed int slot)
{
    return CallAndReturn<char, 0x4D9CC0> (wavId, out_slot, out_bank, slot);
}

/*******************************************************/
int
CAEMp3BankLoader::GetLoopOffset (unsigned short sfxId, short bankSlotInfoId)
{
    return CallMethodAndReturn<int, 0x4E0380> (this, sfxId, bankSlotInfoId);
}

/*******************************************************/
char
CAEMp3BankLoader::Initialise ()
{
    return CallMethodAndReturn<char, 0x4E08F0> (this);
}

/*******************************************************/
void
CCarGenerator::DoInternalProcessing ()
{
    CallMethod<0x6F34D0> (this);
}

/*******************************************************/
void
CCarGenerator::CheckForBlockage (int model_id)
{
    CallMethod<0x6F32E0> (this, model_id);
}

/*******************************************************/
void *
CCarCtrl::CreateCarForScript (int modelId, float X, float Y, float Z, char a5)
{
    return CallAndReturn<void *, 0x431F80> (modelId, X, Y, Z, a5);
}

/*******************************************************/
void
GivePlayerRemoteControlledCar (float x, float y, float z, float angle,
                               short model)
{
    Call<0x45AB10> (x, y, z, angle, model);
}

/*******************************************************/
CBox *
CClumpModelInfo::GetBoundingBox ()
{
    return CallMethodAndReturn<CBox *, 0x4C5710> (this);
}

/*******************************************************/
int
CGame::Init2 (void *file)
{
    return CallAndReturn<int, 0x5BA1A0> (file);
}

/*******************************************************/
int
CGame::Init3 (void *file)
{
    return CallAndReturn<int, 0x5BA400> (file);
}

/*******************************************************/
int
CKeyGen::GetUppercaseKey (const char *string)
{
    return CallAndReturn<int, 0x53CF30> (string);
}

/*******************************************************/
int
CLoadedCarGroup::CountMembers ()
{
    return CallMethodAndReturn<int, 0x611C30> (this);
}

/*******************************************************/
void
CStreaming::RemoveLeastUsedModel (int flags)
{
    Call<0x40CFD0> (flags);
}

/*******************************************************/
void
CStreaming::SetIsDeletable (int model)
{
    Call<0x409C10> (model);
}

/*******************************************************/
int
CLoadedCarGroup::PickRandomCar (bool checkUsage, bool arg2)
{
    return this->m_members[random (CountMembers () - 1)];
}

/*******************************************************/
void
CStreaming::RemoveModel (int model)
{
    Call<0x4089A0> (model);
}

/*******************************************************/
int
CModelInfo::GetMaximumNumberOfPassengersFromNumberOfDoors (int modelIndex)
{
    return CallAndReturn<int, 0x4C89B0> (modelIndex);
}

/*******************************************************/
int
CModelInfo::GetModelInfo (const char *modelName, short *index)
{
    return CallAndReturn<int, 0x4C5940> (modelName, index);
}

/*******************************************************/
char
CText::LoadMissionText (const char *table)
{
    return CallMethodAndReturn<char, 0x69FBF0> (this, table);
}

/*******************************************************/
int
CVehicle::GetVehicleAppearence ()
{
    return CallMethodAndReturn<int, 0x6D1080> (this);
}

/*******************************************************/
char
CVehicle::SetGearUp ()
{
    return CallMethodAndReturn<char, 0x6CAC20> (this);
}

/*******************************************************/
void
CVehicle::SetDriver (CPed *driver)
{
    CallMethod<0x6D16A0> (this, driver);
}

/*******************************************************/
void
CVehicle::AutomobilePlaceOnRoadProperly ()
{
    CallMethod<0x6AF420> (this);
}

/*******************************************************/
void
CVehicle::BikePlaceOnRoadProperly ()
{
    CallMethod<0x6BEEB0> (this);
}

/*******************************************************/
void
CVehicleRecording::SetPlaybackSpeed (void *a1, float a2)
{
    Call<0x459660> (a1, a2);
}

/*******************************************************/
char *
CMessages::AddMessage (char *string, int time, int16_t flags,
                       int8_t bAddToPreviousBrief)
{
    return CallAndReturn<char *, 0x69F1E0> (string, time, flags,
                                            bAddToPreviousBrief);
}

/*******************************************************/
void
CHud::SetMessage (char *str)
{
    return Call<0x588F60> (str);
}

/*******************************************************/
int
cHandlingDataMgr::LoadHandlingData ()
{
    return CallMethodAndReturn<int, 0x5BD830> (this);
}

/*******************************************************/
void
CRunningScript::CollectParameters (short num)
{
    CallMethod<0x464080> (this, num);
}

/*******************************************************/
char *
CRunningScript::GetPointerToScriptVariable (int a2)
{
    return CallMethodAndReturn<char *, 0x464790> (this, a2);
}
/*******************************************************/
void
CCivilianPed::CivilianPed (ePedType type, unsigned int modelIndex)
{
    CallMethod<0x5DDB70> (this, type, modelIndex);
}
/*******************************************************/
int
CPickups::GenerateNewOne (float x, float y, float z, unsigned int modelId,
                          char pickupType, int ammo, unsigned int moneyPerDay,
                          char isEmpty, char *message)
{
    return CallAndReturn<int, 0x456F20> (x, y, z, modelId, pickupType, ammo,
                                         moneyPerDay, isEmpty, message);
}

/*******************************************************/
void
CStreaming::SetMissionDoesntRequireModel (int index)
{
    Call<0x409C90> (index);
}

/*******************************************************/
void
CMatrix::Attach (CMatrix *attach, char link)
{
    CallMethod<0x59BD10> (this, attach, link);
}

/*******************************************************/
void
CMatrix::SetRotateZOnly (float angle)
{
    CallMethod<0x59B020> (this, angle);
}

/*******************************************************/
CMatrix *
RwFrameGetLTM (void *frame)
{
    return CallAndReturn<CMatrix *, 0x7F0990> (frame);
}

/*******************************************************/
void
CPad::DoCheats ()
{
    Call<0x439AF0> ();
}

/*******************************************************/
CAnimBlendAssociation *
RpAnimBlendClumpExtractAssociations (RpClump *clump)
{
    return CallAndReturn<CAnimBlendAssociation *, 0x4D6BE0> (clump);
}

/*******************************************************/
void
RpAnimBlendClumpGiveAssociations (RpClump *              clump,
                                  CAnimBlendAssociation *association)
{
    Call<0x4D6C30> (clump, association);
}

/*******************************************************/
void
CAudioEngine::PreloadMissionAudio (unsigned char slot, int id)
{
    CallMethod<0x507290> (this, slot, id);
}

/*******************************************************/
bool
CAudioEngine::GetMissionAudioLoadingStatus (unsigned char id)
{
    return CallMethodAndReturn<bool, 0x5072A0> (this, id);
}

/*******************************************************/
bool
CAudioEngine::IsMissionAudioSampleFinished (unsigned char id)
{
    return CallMethodAndReturn<bool, 0x5072C0> (this, id);
}

/*******************************************************/
char *
CText::Get (char *key)
{
    return CallMethodAndReturn<char *, 0x6A0050> (this, key);
}

/*******************************************************/
bool
CRunningScript::CheckName (const char *name)
{
    if (std::string (name) == this->m_szName)
        return true;
    return false;
}

/*******************************************************/
char
CRunningScript::ProcessOneCommand ()
{
    return CallMethodAndReturn<char, 0x469EB0> (this);
}

/*******************************************************/
int
CRunningScript::EndThread ()
{
    return CallMethodAndReturn<int, 0x465AA0> (this);
}

/*******************************************************/
void
CRunningScript::Init ()
{
    CallMethod<0x4648E0> (this);
}

/*******************************************************/
void
CRunningScript::SetCharCoordinates (CPed *ped, CVector pos, bool bWarpGang,
                                    bool bOffset)
{
    injector::stdcall<void (CPed *, CVector, bool, bool)>::call<0x464DC0> (
        ped, pos, bWarpGang, bOffset);
}

/*******************************************************/
bool
CRunningScripts::CheckForRunningScript (const char *thread)
{
    for (auto script = CRunningScripts::pActiveScript; script;
         script      = script->m_pNext)
        {
            if (script->m_bIsMission && script->CheckName (thread))
                return true;
        }
    return false;
}

/*******************************************************/
char
CRunningScript::ProcessCommands0to99 (int opcode)
{
    return CallMethodAndReturn<char, 0x465E60> (this, opcode);
}

/*******************************************************/
void
CRunningScript::ProcessCommands1526to1537 (int opcode)
{
    CallMethod<0x487F60> (this, opcode);
}

/*******************************************************/
void
CRunningScript::UpdateCompareFlag (char flag)
{
    CallMethod<0x4859D0> (this, flag);
}

/*******************************************************/
void
CText::Load (char a2)
{
    CallMethod<0x6A01A0> (this, a2);
}

/*******************************************************/
double
CWorld::FindGroundZedForCoord (float x, float y)
{
    return CallAndReturn<double, 0x569660> (x, y);
}

/*******************************************************/
bool
CEnterExit::GetInteriorStatus (const char *name)
{
    for (int i = 0; i < mp_poolEntryExits->m_nSize; i++)
        {
            if ((mp_poolEntryExits->m_byteMap[i] & 0x80u) == 0)
                {
                    auto object = mp_poolEntryExits->GetAt<CEnterExit> (i);
                    if (std::string (name) == object->m_szInteriorName)
                        return object->m_wFlags & 0x4000;
                }
        }
    return true;
}

/*******************************************************/
CVector
FindPlayerCoors (int playerId)
{
    return CallAndReturn<CVector, 0x56E010> (playerId);
}

/*******************************************************/
CVehicle *
FindPlayerVehicle (int playerId, bool bIncludeRemote)
{
    return CallAndReturn<CVehicle *, 0x56E0D0> (playerId, bIncludeRemote);
}

/*******************************************************/
CPed *
FindPlayerPed (int playerId)
{
    return CallAndReturn<CPed *, 0x56E210> (playerId);
}

/*******************************************************/
CEntity *
FindPlayerEntity (int playerId)
{
    return CallAndReturn<CEntity *, 0x56E120> (playerId);
}

/*******************************************************/
float
Dist (CVector a, CVector b)
{
    return sqrt (pow (a.x - b.x, 2) + pow (a.y - b.y, 2) + pow (a.z - b.z, 2));
}

/*******************************************************/
char
CGenericGameStorage::GenericSave ()
{
    return CallAndReturn<char, 0x5D13E0> ();
}

/*******************************************************/
char *
CGenericGameStorage::MakeValidSaveFileName (int saveNum)
{
    return CallAndReturn<char *, 0x5D0E90> (saveNum);
}

/*******************************************************/
int
CGenericGameStorage::SaveDataToWorkBuffer (void *pSource, int size)
{
    return CallAndReturn<int, 0x5D1270> (pSource, size);
}

/*******************************************************/
int
CGenericGameStorage::LoadDataFromWorkBuffer (void *pSource, int size)
{
    return CallAndReturn<int, 0x5D1300> (pSource, size);
}

/*******************************************************/
cSimpleTransform *
CEntity::GetPosition ()
{
    return CallMethodAndReturn<cSimpleTransform *, 0x4043A0> (this);
}

/*******************************************************/
int
CEntity::SetHeading (float heading)
{
    return CallMethodAndReturn<int, 0x43E0C0> (this, heading);
}

/*******************************************************/
int
CEntity::GetHeading ()
{
    return CallMethodAndReturn<int, 0x420B30> (this);
}

/*******************************************************/
void
CStats::IncrementStat (short id, float val)
{
    Call<0x55C180> (id, val);
}

/*******************************************************/
void
CFont::SetFontStyle (short style)
{
    Call<0x719490> (style);
}

/*******************************************************/
void
CFont::SetWrapx (float value)
{
    Call<0x7194D0> (value);
}

/*******************************************************/
void
CFont::SetDropShadowPosition (short value)
{
    Call<0x719570> (value);
}

/*******************************************************/
void
CFont::SetBackground (bool enable, bool includeWrap)
{
    ((void (__cdecl *) (bool, bool)) 0x7195C0) (enable, includeWrap);
}

/*******************************************************/
void
CFont::SetBackgroundColor (CRGBA col)
{
    Call<0x7195E0> (col);
}

/*******************************************************/
void
CFont::SetColor (CRGBA col)
{
    Call<0x719430> (col);
}

/*******************************************************/
void
CFont::SetJustify (bool on)
{
    ((void (__cdecl *) (bool)) 0x719600) (on);
}

/*******************************************************/
void
CFont::SetOrientation (eFontAlignment alignment)
{
    ((void (__cdecl *) (eFontAlignment)) 0x719610) (alignment);
}

/*******************************************************/
void
CFont::PrintString (float x, float y, char *text)
{
    ((void (__cdecl *) (float, float, char *)) 0x71A700) (x, y, text);
}

/*******************************************************/
void
CFont::SetScaleForCurrentlanguage (float w, float h)
{
    ((void (__cdecl *) (float, float)) 0x7193A0) (w, h);
}

/*******************************************************/
void
CFont::SetAlphaFade (float alpha)
{
    ((void (__cdecl *) (float)) 0x719500) (alpha);
}

/*******************************************************/
RpLight *
RpLightSetColor (RpLight *light, RwRGBAReal *colour)
{
    return (
        (RpLight * (__cdecl *) (RpLight *, RwRGBAReal *) ) 0x751A90) (light,
                                                                      colour);
}

/*******************************************************/
double
CStats::GetStatValue (short id)
{
    return CallAndReturn<double, 0x558E40> (id);
}

/*******************************************************/
void
CShopping::LoadShop (const char *name)
{
    Call<0x49BBE0> (name);
}

/*******************************************************/
void
CShopping::LoadShoppingType (const char *name)
{
    Call<0x49B8D0> (name);
}

/*******************************************************/
CObject*
CObject::Create (int modelId)
{
    return CallAndReturn<CObject *, 0x5A1F60> (modelId);
}

/*******************************************************/
int
CIplStore::FindIplSlot (char *name)
{
    return CallAndReturn<int, 0x404AC0> (name);
}

/*******************************************************/
CTimeCycleCurrent *
CTimeCycleCurrent::GetInfo (CTimeCycleCurrent *timecyc, int weatherID,
                            int timeID)
{
    return ((CTimeCycleCurrent
             * (__fastcall *) (CTimeCycleCurrent *, int,
                               int) ) 0x55F4B0) (timecyc, weatherID, timeID);
}

/*******************************************************/
void
CCoronas::RegisterCorona (int ID, CEntity *attachTo, char red, char green,
                          char blue, char alpha, CVector const &posn,
                          float radius, float farClip, int coronaType,
                          int flareType, bool enableReflection,
                          bool checkObstacles, int _param_not_used, float angle,
                          bool longDistance, float nearClip, char fadeState,
                          float fadeSpeed, bool onlyFromBelow,
                          bool reflectionDelay)
{
    ((void (__cdecl *) (int, CEntity *, char, char, char, char, CVector const &,
                        float, float, int, int, bool, bool, int, float, bool,
                        float, char, float, bool, bool)) 0x6FC580) (
        ID, attachTo, red, green, blue, alpha, posn, radius, farClip,
        coronaType, flareType, enableReflection, checkObstacles,
        _param_not_used, angle, longDistance, nearClip, fadeState, fadeSpeed,
        onlyFromBelow, reflectionDelay);
}

/*******************************************************/
void
CCoronas::RegisterCorona (int ID, CEntity *attachTo, char red, char green,
                          char blue, char alpha, CVector const &posn,
                          float radius, float farClip, RwTexture *texture,
                          int flareType, bool enableReflection,
                          bool checkObstacles, int _param_not_used, float angle,
                          bool longDistance, float nearClip, char fadeState,
                          float fadeSpeed, bool onlyFromBelow,
                          bool reflectionDelay)
{
    ((void (__cdecl *) (int, CEntity *, char, char, char, char, CVector const &,
                        float, float, RwTexture *, int, bool, bool, int, float,
                        bool, float, char, float, bool, bool)) 0x6FC180) (
        ID, attachTo, red, green, blue, alpha, posn, radius, farClip, texture,
        flareType, enableReflection, checkObstacles, _param_not_used, angle,
        longDistance, nearClip, fadeState, fadeSpeed, onlyFromBelow,
        reflectionDelay);
}

/*******************************************************/
void
CSprite::RenderOneXLUSprite (float x, float y, float z, float halfw,
                             float halfh, char r, char g, char b, char a,
                             float rhw, char intensity, char udir, char vdir)
{
    ((void (__cdecl *) (float, float, float, float, float, char, char, char,
                        char, float, char, char,
                        char)) 0x70D000) (x, y, z, halfw, halfh, r, g, b, a,
                                          rhw, intensity, udir, vdir);
}

/*******************************************************/
void
CSprite::RenderOneXLUSprite_Triangle (float arg1, float arg2, int arg3,
                                      int arg4, int arg5, int arg6, float arg7,
                                      char r, char g, char b, char a,
                                      float arg12, char arg13)
{
    ((void (__cdecl *) (float, float, int, int, int, int, float, char, char,
                        char, char, float,
                        char)) 0x70D320) (arg1, arg2, arg3, arg4, arg5, arg6,
                                          arg7, r, g, b, a, arg12, arg13);
}

/*******************************************************/
void
CSprite::RenderOneXLUSprite_Rotate_Aspect (float x, float y, float z,
                                           float halfw, float halfh, char r,
                                           char g, char b, char a, float rhw,
                                           float arg11, char intensity)
{
    ((void (__cdecl *) (float, float, float, float, float, char, char, char,
                        char, float, float,
                        char)) 0x70D490) (x, y, z, halfw, halfh, r, g, b, a,
                                          rhw, arg11, intensity);
}

/*******************************************************/
void
CSprite::RenderBufferedOneXLUSprite (float x, float y, float z, float w,
                                     float h, char r, char g, char b, char a,
                                     float recipNearZ, char arg11)
{
    ((void (__cdecl *) (float, float, float, float, float, char, char, char,
                        char, float, char)) 0x70E4A0) (x, y, z, w, h, r, g, b,
                                                       a, recipNearZ, arg11);
}

/*******************************************************/
void
CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect (float x, float y, float z,
                                                   float w, float h, char r,
                                                   char g, char b, char a,
                                                   float recipNearZ,
                                                   float angle, char arg12)
{
    ((void (__cdecl *) (float, float, float, float, float, char, char, char,
                        char, float, float,
                        char)) 0x70E780) (x, y, z, w, h, r, g, b, a, recipNearZ,
                                          angle, arg12);
}

/*******************************************************/
void
CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension (float x, float y, float z,
                                                      float w, float h, char r,
                                                      char g, char b, char a,
                                                      float recipNearZ,
                                                      float angle, char arg12)
{
    ((void (__cdecl *) (float, float, float, float, float, char, char, char,
                        char, float, float,
                        char)) 0x70EAB0) (x, y, z, w, h, r, g, b, a, recipNearZ,
                                          angle, arg12);
}

/*******************************************************/
std::mt19937 &
rand_engine ()
{
    thread_local static std::mt19937 engine{GeneralSettings::m_Config.Seed != -1
                                                ? GeneralSettings::m_Config.Seed
                                                : (unsigned int) time (NULL)};

    return engine;
}

/*******************************************************/
int
random (int min, int max)
{
    std::uniform_int_distribution<int> dist{min, max};
    return dist (rand_engine ());
}

/*******************************************************/
int
random (int max)
{
    return random (0, max);
}

/*******************************************************/
double
randomNormal (double mean, double stddev)
{
    std::normal_distribution<double> dist{mean, stddev};
    return dist (rand_engine ());
}

/*******************************************************/
CAnimBlendAssociation *
CAnimBlendAssocGroup::CopyAnimation (int Id)
{
    return CallMethodAndReturn<CAnimBlendAssociation *, 0x4CE130> (this, Id);
}

/*******************************************************/
float
randomFloat (float min, float max)
{
    std::uniform_real_distribution<float> dist{min, max};
    return dist (rand_engine ());
}

CPickup *        aPickups                       = (CPickup *) 0x9788C0;
CStreamingInfo * ms_aInfoForModel               = (CStreamingInfo *) 0x8E4CC0;
RwRGBA *         ms_vehicleColourTable          = (RwRGBA *) 0xB4E480;
CBaseModelInfo **ms_modelInfoPtrs               = (CBaseModelInfo **) 0xA9B0C8;
int *            ScriptParams                   = (int *) 0xA43C78;
int *            ScriptSpace                    = (int *) 0xA49960;
CLoadedCarGroup *CStreaming::ms_nVehiclesLoaded = (CLoadedCarGroup *) 0x8E4C24;
CPool *&         ms_pPedPool                    = *(CPool **) 0xB74490;
CRunningScript *&CRunningScripts::pActiveScript = *(CRunningScript **) 0xA8B42C;
CWeaponInfo *    aWeaponInfos                   = (CWeaponInfo *) 0xC8AAB8;
CPool *&         CIplStore::ms_pPool            = *(CPool **) 0x8E3FB0;
RsGlobalType *   RsGlobal                       = (RsGlobalType *) 0xC17040;
float *          ms_fTimeStep                   = (float *) 0xB7CB5C;
int &            CGenericGameStorage::length    = *(int *) 0xC16EEC;
CPool *&         CEnterExit::mp_poolEntryExits  = *(CPool **) 0x96A7D8;
CPool *&         ms_pVehiclePool                = *(CPool **) 0xB74494;

int &          CShopping::m_nCurrentShoppingType = *(int *) 0xA9A7C8;
int &          CShopping::m_nTotalItems          = *(int *) 0xA9A7CC;
CShoppingItem *CShopping::m_aShoppingItems       = (CShoppingItem *) 0xA986F0;
unsigned char &CGame::bMissionPackGame           = *(unsigned char *) 0xB72910;
char *         CFileMgr::ms_dirName              = (char *) 0xB71A60;
