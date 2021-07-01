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

struct CCarGenerator;

// Hooks
void __fastcall RandomizeFixedSpawn (CCarGenerator *gen);
int __fastcall RandomizeRandomSpawn (void *group, void *edx, char a2, char a3);

class ParkedCarRandomizer
{
    static ParkedCarRandomizer *mInstance;

    ParkedCarRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for ParkedCarRandomizer.
    static ParkedCarRandomizer *GetInstance ();

    static inline struct Config
    {
        bool RandomizeFixedSpawns;
        bool RandomizeRandomSpawns;
        bool UseSameType;
    } m_Config;

    /// Initialises Hooks/etc.
    void Initialise ();
};
