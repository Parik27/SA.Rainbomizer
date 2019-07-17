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
    CallAndReturn<void *, 0x612710> (type, model, posn, unk);
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
CLoadedCarGroup::CountMembers ()
{
    return CallMethodAndReturn<int, 0x611C30> (this);
}

/*******************************************************/
void
CStreaming::SetIsDeletable(int model)
{
	Call<0x409C10>(model);
}

/*******************************************************/
int
CLoadedCarGroup::PickRandomCar (bool checkUsage, bool arg2)
{
    return CallMethodAndReturn<int, 0x611C50> (this, checkUsage, arg2);
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
CText::LoadMissionText (char *table)
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
void
CVehicleRecording::SetPlaybackSpeed (void *a1, float a2)
{
    Call<0x459660> (a1, a2);
}

/*******************************************************/
void
CRunningScript::CollectParameters (short num)
{
    CallMethod<0x464080> (this, num);
}

/*******************************************************/
void *
CPool::GetAtVehicle (int handle)
{
    return CallMethodAndReturn<void *, 0x4048E0> ((void *) 0xB74494, handle);
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
char *
CText::Get (char *key)
{
    return CallMethodAndReturn<char *, 0x6A0050> (this, key);
}

/*******************************************************/
int
random (int max, int min)
{
    return (rand () % (max - min + 1)) + min;
}

CStreamingInfo * ms_aInfoForModel               = (CStreamingInfo *) 0x8E4CC0;
RwRGBA *         ms_vehicleColourTable          = (RwRGBA *) 0xB4E480;
CBaseModelInfo **ms_modelInfoPtrs               = (CBaseModelInfo **) 0xA9B0C8;
int *            ScriptParams                   = (int *) 0xA43C78;
CLoadedCarGroup *CStreaming::ms_nVehiclesLoaded = (CLoadedCarGroup *) 0x8E4C24;
