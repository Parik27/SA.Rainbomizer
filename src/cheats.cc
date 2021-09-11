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
#include "config.hh"

CheatRandomizer *CheatRandomizer::mInstance = nullptr;

std::string chits[] = {"Chit Activated", "Cabbage", "Giant Pizza Activated",
                       "Beep Boop Boop Beep"};

/*******************************************************/
const char *__fastcall RandomizeHashesAfterCheatActivated (CText *text,
                                                           void *edx, char *key)
{
    CheatRandomizer::GetInstance ()->RandomizeCheatHashes ();

    if (CheatRandomizer::m_Config.EasterEgg)
        {
            int         chits_len = sizeof (chits) / sizeof (chits[0]);
            const char *chit      = chits[random (chits_len - 1)].c_str ();

            return chit;
        }

    return text->Get (key);
}

/*******************************************************/
void
CheatRandomizer::Initialise ()
{
    if (!ConfigManager::ReadConfig ("CheatRandomizer",
                                    std::pair ("EnableEasterEgg",
                                               &m_Config.EasterEgg)))
        return;

    Logger::GetLogger ()->LogMessage ("Intialised CheatRandomizer");
    RegisterHooks (
        {{HOOK_CALL, 0x43854D, (void *) &RandomizeHashesAfterCheatActivated}});

    RandomizeCheatHashes ();
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
void
CheatRandomizer::RandomizeCheatHashes ()
{
    const int CHEAT_HASH_COUNT = 92;

    unsigned int *aCheatHashKeys = (unsigned int *) 0x8A5CC8;
    for (int i = 0; i < CHEAT_HASH_COUNT; i++)
        {
            int temp = aCheatHashKeys[i];
            int swap = random (CHEAT_HASH_COUNT - 1);

            aCheatHashKeys[i]    = aCheatHashKeys[swap];
            aCheatHashKeys[swap] = temp;
        }
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
