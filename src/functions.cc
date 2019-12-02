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
void *
CPed::CCopPed__CCopPed (int type)
{
    return CallMethodAndReturn<void *, 0x5DDC60> (this, type);
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
CBox *
CClumpModelInfo::GetBoundingBox ()
{
    return CallMethodAndReturn<CBox *, 0x4C5710> (this);
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
CVehicleRecording::SetPlaybackSpeed (void *a1, float a2)
{
    Call<0x459660> (a1, a2);
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
int
CPickups::GenerateNewOne (float x, float y, float z, unsigned int modelId,
                          char pickupType, int ammo, unsigned int moneyPerDay,
                          char isEmpty, char *message)
{
    return CallAndReturn<int, 0x456F20> (x, y, z, modelId, pickupType, ammo,
                                         moneyPerDay, isEmpty, message);
}

/*******************************************************/
void *
CPool::GetAt (int handle, int size)
{
    return this->m_pObjects + size * (handle >> 8);
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
int
CRunningScript::EndThread ()
{
    return CallMethodAndReturn<int, 0x465AA0> (this);
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
CVector
FindPlayerCoors (int playerId)
{
    return CallAndReturn<CVector, 0x56E010> (playerId);
}

/*******************************************************/
CPed *
FindPlayerPed (int playerId)
{
    return CallAndReturn<CPed *, 0x56E210> (playerId);
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
cSimpleTransform *
CEntity::GetPosition ()
{
    return CallMethodAndReturn<cSimpleTransform *, 0x4043A0> (this);
}

/*******************************************************/
void
CStats::IncrementStat (short id, float val)
{
    Call<0x55C180> (id, val);
}

/*******************************************************/
std::mt19937 &
rand_engine ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().general;
    thread_local static std::mt19937 engine{
        config.seed != -1 ? config.seed : (unsigned int) time (NULL)};

    return engine;
}

/*******************************************************/
int
random (int max, int min)
{
    std::uniform_int_distribution<int> dist{min, max};
    return dist (rand_engine ());
}

CStreamingInfo * ms_aInfoForModel               = (CStreamingInfo *) 0x8E4CC0;
RwRGBA *         ms_vehicleColourTable          = (RwRGBA *) 0xB4E480;
CBaseModelInfo **ms_modelInfoPtrs               = (CBaseModelInfo **) 0xA9B0C8;
int *            ScriptParams                   = (int *) 0xA43C78;
int *            ScriptSpace                    = (int *) 0xA49960;
CLoadedCarGroup *CStreaming::ms_nVehiclesLoaded = (CLoadedCarGroup *) 0x8E4C24;
CPool *          ms_pPedPool                    = (CPool *) 0xB74490;
CRunningScript *&CRunningScripts::pActiveScript = *(CRunningScript **) 0xA8B42C;
