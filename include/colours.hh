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
struct CRunningScript;

void __fastcall RandomizeVehicleColour (void *info, void *edx, uint8_t *prim,
                                        uint8_t *secn, uint8_t *tert,
                                        uint8_t *quat, int variation);
void __fastcall RandomizeScriptVehicleColours (CRunningScript *scr, void *edx,
                                               short count);
template <bool callOriginal> int RandomizeColourTables ();

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
        bool ChangeCarColsFade;
        bool RandomizeMarkers;
        bool RandomizeText;
        bool RandomizeWeaponSprites;
        bool OldColourRandomization;

        bool RandomizeLights;
        bool ConsistentLights;
        bool RandomizeClouds;
        bool RandomizeStars;
        bool RandomizeRainbows;
        bool RandomizeFireLight;
        bool ChangeOnFade;

        bool RainbowHueCycle;

        bool RandomizeFades;
        bool CrazyMode;
    } m_Config;

    inline static std::uint32_t HudRandomizerSeed = 0;

    /// Returns the static instance for CarColRandomizer.
    static ColourRandomizer *GetInstance ();

    void Initialise ();

    struct Pattern
    {
        int ID;
        int colours[3];
    };
};

CRGBA *__fastcall RandomizeColours (CRGBA *thisCol, void *edx, uint8_t r,
                                    uint8_t g, uint8_t b, uint8_t a);
