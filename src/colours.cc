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
std::vector<ColourRandomizer::Pattern> colourPatterns;
CRGBA              cloudColourDistant = {-1, -1, -1, -1};
CRGBA              cloudColourHigh = {-1, -1, -1, -1};

ColourRandomizer::Pattern GetColour (ColourRandomizer::Pattern pattern)
{
    for (int i = 0; i < colourPatterns.size (); i++)
    {
        if (colourPatterns[i].ID == pattern.ID)
        {
            return colourPatterns[i];
        }
    }
    return {-1, { -1, -1, -1 }};
}

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
    static bool hueCycle = ColourRandomizer::m_Config.RainbowHueCycle;

    int    colour[3];
    static int offsetForStatic = random (0, 10000);
    double     time
        = (!hueCycle) ? offsetForStatic : 1000.0 * clock () / CLOCKS_PER_SEC;

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
    static bool hueCycle  = ColourRandomizer::m_Config.RainbowHueCycle;
    static bool crazyMode = ColourRandomizer::m_Config.CrazyMode;

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
RandomizeCoronaColours (int ID, CEntity *attachTo, char red, char green,
                        char blue, char alpha, CVector const &posn,
                        float radius, float farClip, int coronaType,
                        int flaretype, bool enableReflection,
                        bool checkObstacles, int _param_not_used, float angle,
                        bool longDistance, float nearClip, char fadeState,
                        float fadeSpeed, bool onlyFromBelow,
                        bool reflectionDelay)
{
    CRGBA colour = {red, green, blue, alpha};
    ColourRandomizer::Pattern currentID = {ID, {colour.r, colour.g, colour.b}};
    if (GetColour (currentID).ID == -1)
    {
        currentID.colours[0] = random (255);
        currentID.colours[1] = random (255);
        currentID.colours[2] = random (255);
        colourPatterns.push_back (currentID);
    }
    else
    {
        colour.r = GetColour (currentID).colours[0];
        colour.g = GetColour (currentID).colours[1];
        colour.b = GetColour (currentID).colours[2];
    }
    colour               = GetRainbowColour (HashColour (colour));
    CCoronas::RegisterCorona (ID, attachTo, colour.r, colour.g, colour.b, alpha, posn, radius,
                    farClip, coronaType, flaretype, enableReflection,
                    checkObstacles, _param_not_used, angle, longDistance,
                    nearClip, fadeState, fadeSpeed, onlyFromBelow,
                    reflectionDelay);
}

/*******************************************************/
void
RandomizeLightColours (int ID, CEntity *attachTo, char red, char green,
                        char blue, char alpha, CVector const &posn,
                        float radius, float farClip, RwTexture* texture,
                        int flaretype, bool enableReflection,
                        bool checkObstacles, int _param_not_used, float angle,
                        bool longDistance, float nearClip, char fadeState,
                        float fadeSpeed, bool onlyFromBelow,
                        bool reflectionDelay)
{
    CRGBA colour = {red, green, blue, alpha};
    ColourRandomizer::Pattern currentID = {ID, {colour.r, colour.g, colour.b}};
    if (GetColour (currentID).ID == -1)
    {
        currentID.colours[0] = random (255);
        currentID.colours[1] = random (255);
        currentID.colours[2] = random (255);
        colourPatterns.push_back (currentID);
    }
    else
        {
            colour.r = GetColour (currentID).colours[0];
            colour.g = GetColour (currentID).colours[1];
            colour.b = GetColour (currentID).colours[2];
        }
    colour       = GetRainbowColour (HashColour (colour));
    CCoronas::RegisterCorona (ID, attachTo, colour.r, colour.g, colour.b, alpha,
                              posn, radius, farClip, texture, flaretype,
                              enableReflection, checkObstacles, _param_not_used,
                              angle, longDistance, nearClip, fadeState,
                              fadeSpeed, onlyFromBelow, reflectionDelay);
}

/*******************************************************/
void RandomizeWeaponSpriteColours (float x, float y, float z, float halfw,
                             float halfh, char r, char g, char b, char a,
                             float rhw, char intensity, char udir, char vdir)
{
    CRGBA colour = {r, g, b, a};
    colour       = GetRainbowColour (HashColour (colour));
    CSprite::RenderOneXLUSprite (x, y, z, halfw, halfh, colour.r, colour.g,
                                 colour.b, a, rhw, intensity, udir, vdir);
}

/*******************************************************/
void
RandomizeWeaponEffectColours (float arg1, float arg2, int arg3, int arg4,
                              int arg5, int arg6, float arg7, char r, char g,
                              char b, char a, float arg12, char arg13)
{
    CRGBA colour = {r, g, b, a};
    colour       = GetRainbowColour (HashColour (colour));
    CSprite::RenderOneXLUSprite_Triangle (arg1, arg2, arg3, arg4, arg5, arg6, arg7, colour.r, colour.g,
                                 colour.b, a, arg12, arg13);
}

/*******************************************************/
void
RandomizeMoreWeaponEffectColours (float x, float y, float z, float halfw,
                                 float halfh, char r, char g, char b, char a,
                                 float rhw, float arg11, char intensity)
{
    CRGBA colour = {r, g, b, a};
    colour       = GetRainbowColour (HashColour (colour));
    CSprite::RenderOneXLUSprite_Rotate_Aspect (x, y, z, halfw, halfh, colour.r, colour.g,
                                 colour.b, a, rhw, arg11, intensity);
}

/*******************************************************/
template <int address>
void
RandomizeWeatherEffectColours (float x, float y, float z, float w, float h,
                               char r, char g, char b, char a, float recipNearZ,
                               char arg11)
{
    CRGBA colour = {r, g, b, a};
    if (address == 0x715EDF)
    {
        if (cloudColourHigh.a == -1)
            cloudColourHigh = {colour.r, colour.g, colour.b, colour.a};
        colour = cloudColourHigh;
    }
    colour       = GetRainbowColour (HashColour (colour));
    CSprite::RenderBufferedOneXLUSprite (x, y, z, w, h, colour.r, colour.g,
                                 colour.b, a, recipNearZ, arg11);
}

/*******************************************************/
void
RandomizeFogColours (float x, float y, float z, float w, float h, char r,
                     char g, char b, char a, float recipNearZ, float angle,
                     char arg12)
{
    CRGBA colour = {r, g, b, a};
    colour       = GetRainbowColour (HashColour (colour));
    CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect (x, y, z, w, h, colour.r, colour.g,
                                 colour.b, a, recipNearZ, angle, arg12);
}

/*******************************************************/
void
RandomizeCloudColours (float x, float y, float z, float w, float h, char r,
                       char g, char b, char a, float recipNearZ, float angle,
                       char arg12)
{
    CRGBA colour = {r, g, b, a};
    if (cloudColourDistant.a == -1)
        cloudColourDistant = {colour.r, colour.g, colour.b, colour.a};
    colour = cloudColourDistant;
    colour       = GetRainbowColour (HashColour (colour));
    CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension (x, y, z, w, h, colour.r, colour.g,
                                                          colour.b, a, recipNearZ, angle,
                                                          arg12);
}

/*******************************************************/
void
RandomizeRpLights (RpLight *light, RwRGBAReal *colour)
{
    CRGBA colourNew = {colour->r, colour->g, colour->b, colour->a};
    colourNew       = GetRainbowColour (HashColour (colourNew));
    colour->r       = colourNew.r;
    colour->g       = colourNew.g;
    colour->b       = colourNew.b;
    RpLightSetColor (light, colour);
}

/*******************************************************/
void
ColourRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("ColourRandomizer", 
            std::pair ("RandomizeCarCols", &m_Config.RandomizeCarCols),
            std::pair ("RandomizeMarkers", &m_Config.RandomizeMarkers),
            std::pair ("RandomizeText", &m_Config.RandomizeText),
            std::pair ("RandomizeLights", &m_Config.RandomizeLights),
            std::pair ("RandomizeClouds", &m_Config.RandomizeClouds),
            std::pair ("RandomizeOtherSkyElements", &m_Config.RandomizeOtherSkyElements),
            std::pair ("RainbowHueCycle", &m_Config.RainbowHueCycle),
            std::pair ("RandomizeFades", &m_Config.RandomizeFades),
            std::pair ("CrazyMode", &m_Config.CrazyMode)))
        return;

    if (m_Config.RandomizeCarCols)
        RegisterHooks (
            {{HOOK_CALL, 0x5B8F17, (void *) &RandomizeColourTables},
             {HOOK_JUMP, 0x4C8500, (void *) &RandomizeVehicleColour}, 
             {HOOK_CALL, 0x47EA9B, (void *) &RandomizeScriptVehicleColours},
             {HOOK_CALL, 0x47B8FE, (void *) &RandomizeScriptVehicleColours}});
    if (m_Config.RandomizeText)
    {
        RegisterHooks ({{HOOK_JUMP, 0x7170C0, (void *) &RandomizeColours},
                        {HOOK_CALL, 0x728788, (void *) &SkipRandomizeColours}});

        for (int address : {0x58D8FD, 0x58E95B, 0x58E9CA, 0x58EA39, 0x58EAA8,
                            0x714011, 0x725D70})
            {
                injector::MakeCALL (address, (void *) &RandomizeWeaponSpriteColours);
            }
    }

    if (m_Config.RandomizeMarkers)
        RegisterHooks ({{HOOK_CALL, 0x7250B1, (void *) &RandomizeMarkers}});

    if (m_Config.RandomizeLights)
    {
            for (int address :
                 {0x45584E, 0x455F59, 0x456219, 0x47F18E, 0x48BFB4, 0x49DD4C,
                  0x53B7E0, 0x53B893, 0x53B99C, 0x53BAA5, 0x6ABA60, 0x6ABB35,
                  0x6ABC69, 0x6BD4DD, 0x6BD531, 0x6C5B1F, 0x6DDF28, 0x6E0D01,
                  0x6E0DF7, 0x6E1A2D, 0x6E219F, 0x6E2297, 0x6E253E, 0x6E2633,
                  0x7175C3, 0x7176BA, 0x7177B1, 0x73AAA2})
                {
                    injector::MakeCALL (address,
                                        (void *) &RandomizeCoronaColours);
                }

            for (int address :
                 {0x5363A8, 0x536511, 0x6FC6B1, 0x6FC73D, 0x6FCA80, 0x6FD02E})
                {
                    injector::MakeCALL (address,
                                        (void *) &RandomizeLightColours);
                }

            for (int address : {0x7433A3, 0x7434F2, 0x74378E, 0x7438AB})
                {
                    injector::MakeCALL (address,
                                        (void *) &RandomizeWeaponEffectColours);
                }

            for (int address : {0x56F18B, 0x742EAF, 0x742F45, 0x743073,
                                0x74311D, 0x743A0A, 0x743BD4})
                {
                    injector::MakeCALL (
                        address, (void *) &RandomizeMoreWeaponEffectColours);
                }
    }

    if (m_Config.RandomizeOtherSkyElements)
    {
        for (int address : {0x6FB9A6, 0x713F0F, 0x714371})
        {
                injector::MakeCALL (0x6FB9A6, (void *) &RandomizeWeatherEffectColours<0x6FB9A6>); // Wet Roads
                injector::MakeCALL (0x713F0F, (void *) &RandomizeWeatherEffectColours<0x713F0F>); // Stars
                injector::MakeCALL (0x714371, (void *) &RandomizeWeatherEffectColours<0x714371>); // Rainbows
        }
    }

    if (m_Config.RandomizeClouds)
    {
        injector::MakeCALL (0x715EDF, (void *) &RandomizeWeatherEffectColours<0x715EDF>); // High Clouds
        injector::MakeCALL (0x700817, (void *) &RandomizeFogColours);
        injector::MakeCALL (0x700B6B, (void *) &RandomizeFogColours);
        injector::MakeCALL (0x714216, (void *) &RandomizeCloudColours); // Distant Clouds
    }

    //for (int address : {0x735F9F, 0x735FAF})
    //{
    //    injector::MakeCALL (0x751A90, (void *) &RandomizeRpLights);
    //}

    if (m_Config.RandomizeFades)
        {
            RegisterHooks (
                {{HOOK_CALL, 0x50BF22, (void *) &RandomizeFadeColour}});
            injector::MakeRangedNOP (0x50BF22 + 5, 0x50BF33);
        }

    Logger::GetLogger ()->LogMessage ("Initialised ColourRandomizer");
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
void __fastcall RandomizeScriptVehicleColours (CRunningScript *scr, void *edx,
                                               short count)
{
    scr->CollectParameters (count);
    ScriptParams[1] = random (127);
    ScriptParams[2] = random (127);
}

/*******************************************************/
int
RandomizeColourTables ()
{
    int ret = CModelInfo::LoadVehicleColours ();

    for (int i = 0; i < 128; i++)
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
