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

#include <cstdlib>
#include "colours.hh"
#include "base.hh"
#include "functions.hh"
#include <cmath>
#include "logger.hh"
#include "config.hh"
#include <algorithm>
#include "injector/calling.hpp"
#include <chrono>
#include <string>

ColourRandomizer *ColourRandomizer::mInstance = nullptr;

/*******************************************************/
void
HSVtoRGB (int H, double S, double V, int output[3])
{
    double C = S * V;
    double X = C * (1 - std::abs (fmod (H / 60.0, 2) - 1));
    double m = V - C;
    double Rs, Gs, Bs;

    if (H >= 0 && H < 60)
        {
            Rs = C;
            Gs = X;
            Bs = 0;
        }
    else if (H >= 60 && H < 120)
        {
            Rs = X;
            Gs = C;
            Bs = 0;
        }
    else if (H >= 120 && H < 180)
        {
            Rs = 0;
            Gs = C;
            Bs = X;
        }
    else if (H >= 180 && H < 240)
        {
            Rs = 0;
            Gs = X;
            Bs = C;
        }
    else if (H >= 240 && H < 300)
        {
            Rs = X;
            Gs = 0;
            Bs = C;
        }
    else
        {
            Rs = C;
            Gs = 0;
            Bs = X;
        }

    output[0] = (Rs + m) * 255;
    output[1] = (Gs + m) * 255;
    output[2] = (Bs + m) * 255;
}

/*******************************************************/
void
ColourRandomizer::DestroyInstance ()
{
    if (ColourRandomizer::mInstance)
        delete ColourRandomizer::mInstance;
}

/*******************************************************/
ColourRandomizer *
ColourRandomizer::GetInstance ()
{
    if (!ColourRandomizer::mInstance)
        {
            ColourRandomizer::mInstance = new ColourRandomizer ();
            atexit (&ColourRandomizer::DestroyInstance);
        }
    return ColourRandomizer::mInstance;
}

/*******************************************************/
void
RandomizeFadeColour ()
{
    injector::WriteMemory<CRGBA> (0xC3EFA8, GetRainbowColour ());
}

/*******************************************************/
CRGBA
GetRainbowColour (int offset)
{
    int    colour[3];
    double time = 1000.0 * clock () / CLOCKS_PER_SEC;

    HSVtoRGB (int (time / 10 + offset) % 360, 0.7, 0.7, colour);

    return {colour[0], colour[1], colour[2]};
}

constexpr int
HashColour (const CRGBA &colour)
{
    return (colour.r * 255 + colour.g) * 255 + colour.b;
}

/*******************************************************/
CRGBA *__fastcall RandomizeColours (CRGBA *thisCol, void *edx, uint8_t r,
                                    uint8_t g, uint8_t b, uint8_t a)
{
    static auto config   = ConfigManager::GetInstance ()->GetConfigs ().colours;
    static bool hueCycle = config.hueCycle;
    static bool crazyMode = config.crazyMode;

    static int offset = random (0, 10000);
    float      time = (!hueCycle) ? offset : 1000.0 * clock () / CLOCKS_PER_SEC;

    int colour[3];
    int hash = HashColour ({r, g, b, a});
    HSVtoRGB ((int) (time / 10 + hash) % 360, 0.7, 0.7, colour);

    if ((r != g && g != b) || crazyMode)
        {
            thisCol->r = colour[0];
            thisCol->g = colour[1];
            thisCol->b = colour[2];
            thisCol->a = a;
        }
    else
        {
            thisCol->r = r;
            thisCol->g = g;
            thisCol->b = b;
            thisCol->a = a;
        }
    return thisCol;
}

/*******************************************************/
CRGBA *__fastcall SkipRandomizeColours (CRGBA *thisCol, void *edx, uint8_t r,
                                        uint8_t g, uint8_t b, uint8_t a)
{
    thisCol->r = r;
    thisCol->g = g;
    thisCol->b = b;
    thisCol->a = a;

    return thisCol;
}

/*******************************************************/
void __fastcall RandomizeMarkers (C3dMarker *marker)
{
    auto colour      = marker->colour;
    marker->colour   = GetRainbowColour (HashColour (colour));
    marker->colour.a = colour.a;

    HookManager::CallOriginal<injector::thiscall<void (C3dMarker *)>,
                              0x7250B1> (marker);

    marker->colour = colour;
}

/*******************************************************/
void
ColourRandomizer::Initialise ()
{
    auto config = ConfigManager::GetInstance ()->GetConfigs ().colours;

    if (!config.enabled)
        return;

    if (config.cars)
        RegisterHooks (
            {{HOOK_CALL, 0x5B8F17, (void *) &RandomizeColourTables},
             {HOOK_JUMP, 0x4C8500, (void *) &RandomizeVehicleColour}});
    if (config.texts)
        RegisterHooks ({{HOOK_JUMP, 0x7170C0, (void *) &RandomizeColours},
                        {HOOK_CALL, 0x728788, (void *) &SkipRandomizeColours},
                        {HOOK_CALL, 0x7250B1, (void *) &RandomizeMarkers}});

    if (config.fades)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x50BF22, (void *) &RandomizeFadeColour}});
            injector::MakeRangedNOP (0x50BF22 + 5, 0x50BF33);
        }

    Logger::GetLogger ()->LogMessage ("Initialised ColourRandomizzer");
}

/*******************************************************/
void __fastcall RandomizeVehicleColour (void *info, void *edx, uint8_t *prim,
                                        uint8_t *secn, uint8_t *tert,
                                        uint8_t *quat, int variation)
{
    *prim = random (127);
    *secn = random (127);
    *tert = random (127);
    *quat = random (127);
}

/*******************************************************/
int
RandomizeColourTables ()
{
    int ret = CModelInfo::LoadVehicleColours ();

    auto config = ConfigManager::GetInstance ()->GetConfigs ().colours;

    for (int i = 0; i < 128; i++)
        {
            // Check for exceptions
            if (std::find (std::begin (config.exceptions),
                           std::end (config.exceptions), i)
                != std::end (config.exceptions))
                continue;

            int colour[] = {0, 0, 0};
            HSVtoRGB ((int) (i * 2.8125), random (50, 100) / 100.0,
                      random (50, 100) / 100.0, colour);

            ms_vehicleColourTable[i].r = (uint8_t) colour[0];
            ms_vehicleColourTable[i].g = (uint8_t) colour[1];
            ms_vehicleColourTable[i].b = (uint8_t) colour[2];
        }

    Logger::GetLogger ()->LogMessage ("Randomized Colour Tables");
    return ret;
}
