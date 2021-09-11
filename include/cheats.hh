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

struct CText;
const char *__fastcall RandomizeHashesAfterCheatActivated (CText *text,
                                                           void * edx,
                                                           char * key);

class CheatRandomizer
{
    static CheatRandomizer *mInstance;

    int mTimer = 0;

    CheatRandomizer (){};
    static void DestroyInstance ();

public:
    static inline struct Config
    {
        bool EasterEgg;
    } m_Config;
    /// Returns the static instance for CheatRandomizer.
    static CheatRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    /// Randomizes the Cheat Hash table
    void RandomizeCheatHashes ();

    /// Should activate a cheat
    bool ShouldActivate ();
};
