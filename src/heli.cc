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

#include "heli.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include "injector/injector.hpp"
#include "config.hh"

PoliceHeliRandomizer *PoliceHeliRandomizer::mInstance = nullptr;

/*******************************************************/
int
GetRandomHeliID ()
{
    int HeliIDs[] = {548, 425, 417, 487, 488, 497, 563, 447, 469, 501, 465};
    return HeliIDs[random (sizeof (HeliIDs) / 4 - 1)];
}

/*******************************************************/
void
RandomizeHelisOnUnload ()
{
    PoliceHeliRandomizer::GetInstance ()->UnloadHelis ();
    PoliceHeliRandomizer::GetInstance ()->RandomizeHelis ();
}

/*******************************************************/
void
PoliceHeliRandomizer::UnloadHelis ()
{
    CStreaming::SetIsDeletable (mVCNHeli);
    CStreaming::SetIsDeletable (mPoliceHeli);
}

/*******************************************************/
void
PoliceHeliRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("PoliceHeliRandomizer"))
        return;

    RegisterHooks ({{HOOK_CALL, 0x40B88B, (void *) &RandomizeHelisOnUnload},
                    {HOOK_CALL, 0x40B845, (void *) &TurnOnRandomization}});

    ExceptionManager::GetExceptionManager ()->RegisterHandler (
        &PoliceHeliRandomizer::ExceptionHandlerCallback);

    RandomizeHelis ();
    Logger::GetLogger ()->LogMessage ("Intialised PoliceHeliRandomizer");
}

/*******************************************************/
void
TurnOnRandomization (int model, int flags)
{
    PoliceHeliRandomizer::GetInstance ()->SetRandomizationEnabled ();
    CStreaming::RequestModel (model, flags);
}

/*******************************************************/
void
PoliceHeliRandomizer::RandomizeHelis (bool disableRandomization)
{
    if (mRandomizationEnabled)
        {
            mVCNHeli    = GetRandomHeliID ();
            mPoliceHeli = GetRandomHeliID ();
            UpdatePatches ();
        }

    if (disableRandomization)
        mRandomizationEnabled = false;
}

/*******************************************************/
void
PoliceHeliRandomizer::UpdatePatches ()
{
    injector::WriteMemory<uint16_t> (0x6C79CF, mPoliceHeli);
    injector::WriteMemory<uint16_t> (0x6C658A, mPoliceHeli);
    injector::WriteMemory<uint16_t> (0x40B841, mPoliceHeli);

    injector::WriteMemory<uint16_t> (0x6C7A5F, mVCNHeli);
    injector::WriteMemory<uint16_t> (0x6C6562, mVCNHeli);
    injector::WriteMemory<uint16_t> (0x40B86C, mVCNHeli);

    injector::WriteMemory (0x6C7A96 + 2,
                           &ms_aInfoForModel[mPoliceHeli].m_nLoadState);

    injector::WriteMemory (0x6C7AF0, &ms_aInfoForModel[mVCNHeli].m_nLoadState);
}

/*******************************************************/
void
PoliceHeliRandomizer::DestroyInstance ()
{
    if (PoliceHeliRandomizer::mInstance)
        delete PoliceHeliRandomizer::mInstance;
}

/*******************************************************/
void
PoliceHeliRandomizer::ExceptionHandlerCallback (_EXCEPTION_POINTERS *ep)
{
    auto inst = GetInstance ();
    Logger::GetLogger ()->LogMessage ("Helicopters (VCN, Police): "
                                      + std::to_string (inst->mVCNHeli) + ", "
                                      + std::to_string (inst->mPoliceHeli));
}

/*******************************************************/
PoliceHeliRandomizer *
PoliceHeliRandomizer::GetInstance ()
{
    if (!PoliceHeliRandomizer::mInstance)
        {
            PoliceHeliRandomizer::mInstance = new PoliceHeliRandomizer ();
            atexit (&PoliceHeliRandomizer::DestroyInstance);
        }
    return PoliceHeliRandomizer::mInstance;
}
