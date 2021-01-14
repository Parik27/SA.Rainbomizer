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

#include <cstdint>
#include <vector>

// Hooks
// - Select car colour
// - Initialise car colour tables

// Hooked Functions

struct CRGBA;

void __fastcall RandomizeVehicleColour (void *info, void *edx, uint8_t *prim,
                                        uint8_t *secn, uint8_t *tert,
                                        uint8_t *quat, int variation);
int RandomizeColourTables ();

CRGBA GetRainbowColour (int offset = 0);

class ColourRandomizer
{
    static ColourRandomizer *mInstance;

    ColourRandomizer (){};
    static void DestroyInstance ();

public:

    static inline struct Config
    {
        bool RandomizeCarCols;
        bool RandomizeFades;
        bool RandomizeText;

        bool RainbowHueCycle;

        //std::vector<int64_t> Exceptions;
        bool                 CrazyMode;
    } m_Config;

    /// Returns the static instance for CarColRandomizer.
    static ColourRandomizer *GetInstance ();

    void Initialise ();
};
