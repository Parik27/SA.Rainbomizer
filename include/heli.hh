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

struct CHeli;
struct _EXCEPTION_POINTERS;

int  GetRandomHeliID ();
void RandomizeHelisOnUnload ();
void TurnOnRandomization (int model, int flags);

class PoliceHeliRandomizer
{
    static PoliceHeliRandomizer *mInstance;

    int  mPoliceHeli;
    int  mVCNHeli;
    bool mRandomizationEnabled = true;

    PoliceHeliRandomizer (){};
    static void DestroyInstance ();

public:
    /// Returns the static instance for PoliceHeliRandomizer.
    static PoliceHeliRandomizer *GetInstance ();

    /// Initialises Hooks/etc.
    void Initialise ();

    int
    GetPoliceHeli ()
    {
        return mPoliceHeli;
    }
    int
    GetVCNHeli ()
    {
        return mVCNHeli;
    }

    static void ExceptionHandlerCallback (_EXCEPTION_POINTERS *ep);

    void UnloadHelis ();
    void RandomizeHelis (bool disableRandomization = true);
    void UpdatePatches ();

    void
    SetRandomizationEnabled (bool enabled = true)
    {
        mRandomizationEnabled = enabled;
    }
};
