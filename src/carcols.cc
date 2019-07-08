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
#include "carcols.hh"
#include "base.hh"
#include "functions.hh"
#include <cmath>
#include "logger.hh"

CarColRandomizer *CarColRandomizer::mInstance = nullptr;

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
CarColRandomizer::DestroyInstance ()
{
    if (CarColRandomizer::mInstance)
        delete CarColRandomizer::mInstance;
}

/*******************************************************/
CarColRandomizer *
CarColRandomizer::GetInstance ()
{
    if (!CarColRandomizer::mInstance)
        {
            CarColRandomizer::mInstance = new CarColRandomizer ();
            atexit (&CarColRandomizer::DestroyInstance);
        }
    return CarColRandomizer::mInstance;
}

/*******************************************************/
void
CarColRandomizer::Initialise ()
{
    RegisterHooks ({{HOOK_CALL, 0x5B8F17, (void *) &RandomizeColourTables},
                    {HOOK_JUMP, 0x4C8500, (void *) &RandomizeVehicleColour}});

    Logger::GetLogger ()->LogMessage ("Initialised CarColRandomizer");
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

    // Skip first 2 colours (white, black)
    for (int i = 2; i < 128; i++)
        {
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
