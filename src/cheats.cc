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

#include "cheats.hh"
#include <cstdlib>
#include "logger.hh"
#include "base.hh"
#include "functions.hh"
#include <ctime>

CheatRandomizer *CheatRandomizer::mInstance = nullptr;

/*******************************************************/
void
RandomizeCheats ()
{
    if (CheatRandomizer::GetInstance ()->ShouldActivate ())
        {
            int *cheatTables = (int *) 0x8A5B58;
            int  cheatIndex  = -1;
            do
                {
                    cheatIndex = random (59);
                }
            while (cheatTables[cheatIndex] == 0);

            printf ("%d\n", cheatIndex);
            typedef void func (void);
            func *       f = (func *) cheatTables[cheatIndex];
            f ();
        }
    CPad::DoCheats ();
}

/*******************************************************/
void
CheatRandomizer::Initialise ()
{
    Logger::GetLogger ()->LogMessage ("Intialised CheatRandomizer");
    RegisterHooks ({{HOOK_CALL, 0x53BFB8, (void *) &RandomizeCheats}});
}

/*******************************************************/
void
CheatRandomizer::DestroyInstance ()
{
    if (CheatRandomizer::mInstance)
        delete CheatRandomizer::mInstance;
}

/*******************************************************/
bool
CheatRandomizer::ShouldActivate ()
{
    int currentTime = time (NULL);
    if (mTimer == 0)
        {
            mTimer = currentTime;
            return false;
        }
    if (mTimer + 20 < currentTime)
        {
            mTimer = currentTime;
            return true;
        }

    return false;
}

/*******************************************************/
CheatRandomizer *
CheatRandomizer::GetInstance ()
{
    if (!CheatRandomizer::mInstance)
        {
            CheatRandomizer::mInstance = new CheatRandomizer ();
            atexit (&CheatRandomizer::DestroyInstance);
        }
    return CheatRandomizer::mInstance;
}
