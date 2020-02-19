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

#include "handling.hh"
#include <cstdlib>
#include "logger.hh"
#include "functions.hh"
#include "base.hh"
#include "config.hh"

HandlingRandomizer *HandlingRandomizer::mInstance = nullptr;

/*******************************************************/
void __fastcall RandomizeHandling (CVehicle *vehicle, void *edx, CPed *ped) {}

/*******************************************************/
void
HandlingRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().handling;
    if (!config.enabled)
        return;

    RegisterHooks ({{HOOK_CALL, 0x64BB57, (void *) &RandomizeHandling}});
    Logger::GetLogger ()->LogMessage ("Intialised HandlingRandomizer");
}

/*******************************************************/
void
HandlingRandomizer::DestroyInstance ()
{
    if (HandlingRandomizer::mInstance)
        delete HandlingRandomizer::mInstance;
}

/*******************************************************/
HandlingRandomizer *
HandlingRandomizer::GetInstance ()
{
    if (!HandlingRandomizer::mInstance)
        {
            HandlingRandomizer::mInstance = new HandlingRandomizer ();
            atexit (&HandlingRandomizer::DestroyInstance);
        }
    return HandlingRandomizer::mInstance;
}
