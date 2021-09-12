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

#include "base.hh"
#include "gxt.hh"
#include "logger.hh"
#include "traffic.hh"
#include "colours.hh"
#include "heli.hh"
#include "cgen.hh"
#include <ctime>
#include "scm.hh"
#include "sounds.hh"
#include "cheats.hh"
#include "plates.hh"
#include "handling.hh"
#include "weapons.hh"
#include "config.hh"
#include "autosave.hh"
#include "pickups.hh"
#include "missions.hh"
#include "objects.hh"
#include "cutscenes.hh"
#include "particles.hh"
#include "blips.hh"
#include "credits.hh"
#include "weaponstats.hh"
#include "clothes.hh"
#include "wanted.hh"
#include "respawns.hh"
#include "dyom.hh"
#include "animations.hh"
#include "ped.hh"
#include "timecycle.hh"
#include "riot.hh"
#include "map.hh"
#include "generalsettings.hh"

///////////////////////////////////////////////
//  _ ____  _____           _           _    //
// / |___ \|___ / _ __ ___ | |__   ___ | |_  //
// | | __) | |_ \| '__/ _ \| '_ \ / _ \| __| //
// | |/ __/ ___) | | | (_) | |_) | (_) | |_  //
// |_|_____|____/|_|  \___/|_.__/ \___/ \__| //
///////////////////////////////////////////////
// Visit his Twitch Channel - https://www.twitch.tv/123robot

class Rainbomizer
{
public:
    /*******************************************************/
    Rainbomizer ()
    {
        if (!ConfigManager::ReadConfig ("EnableRainbomizer"))
            return;

        GeneralSettings::GetInstance ()->Initialise ();

        // Unprotect if required
        if (GeneralSettings::m_Config.Unprotect)
            UnProtectInstance ();

        auto logger = Logger::GetLogger ();

        ExceptionManager::GetExceptionManager ()->RegisterExceptionManager ();
        logger->LogMessage ("Registered Exception Manager");

        if (!VerifyGameVersion ())
            return;

        HookManager::GetInstance ()->Initialise ();

        TrafficRandomizer::GetInstance ()->Initialise ();
        ColourRandomizer::GetInstance ()->Initialise ();
        TimeCycleRandomizer::GetInstance ()->Initialise ();
        ParkedCarRandomizer::GetInstance ()->Initialise ();
        PoliceHeliRandomizer::GetInstance ()->Initialise ();
        ScriptVehicleRandomizer::GetInstance ()->Initialise ();
        LicensePlateRandomizer::GetInstance ()->Initialise ();
        SoundRandomizer::GetInstance ()->Initialise ();
        HandlingRandomizer::GetInstance ()->Initialise ();
        CheatRandomizer::GetInstance ()->Initialise ();
        WeaponRandomizer::GetInstance ()->Initialise ();
        PickupsRandomizer::GetInstance ()->Initialise ();
        MissionRandomizer::GetInstance ()->Initialise ();
        ObjectsRandomizer::GetInstance ()->Initialise ();
        CutsceneRandomizer::GetInstance ()->Initialise ();
        ParticleRandomizer::GetInstance ()->Initialise ();
        BlipRandomizer::GetInstance ()->Initialise ();
        WeaponStatsRandomizer::GetInstance ()->Initialise ();
        ClothesRandomizer::GetInstance ()->Initialise ();
        WantedLevelRandomizer::GetInstance ()->Initialise ();
        RespawnPointRandomizer::GetInstance ()->Initialise ();
        DyomRandomizer::GetInstance ()->Initialise ();
        RiotRandomizer::GetInstance ()->Initialise ();
        AnimationRandomizer::GetInstance ()->Initialise ();
        GxtRandomizer::GetInstance ()->Initialise ();
        PedRandomizer::GetInstance ()->Initialise ();
        MapRandomizer::GetInstance ()->Initialise ();

        CreditsExtender::Initialise ();

        AutoSave::GetInstance ()->Initialise ();
    }

} rainbow;
