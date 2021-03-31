const char config_toml[] = R"(
########################################################

# Rainbomizer - A (probably fun) Grand Theft Auto San Andreas Mod that
#				randomizes stuff
# Copyright (C) 2019 - Parik

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.	If not, see <https://www.gnu.org/licenses/>.
#
#######################################################
# General Configuration

[Randomizers]
# Set to false to disable all Rainbomizer features regardless of other settings
EnableRainbomizer = true

# Set to true or false to enable / disable specific randomizers. 
# Brief explanations of each can be found at the end of this file.

ColourRandomizer = true
TimeCycleRandomizer = true
TrafficRandomizer = true
ScriptVehicleRandomizer = true
ParkedCarRandomizer = true
MissionRandomizer = true
WeaponRandomizer = true
PickupsRandomizer = true
PlayerRandomizer = true
PedRandomizer = true
CutsceneRandomizer = true
VoiceLineRandomizer = true
LanguageRandomizer = true
LicensePlateRandomizer = true
RiotRandomizer = true
PoliceHeliRandomizer = true
BlipRandomizer = true
WantedLevelRandomizer = true
DYOMRandomizer = true
CheatRandomizer = false

# EXPERIMENTAL RANDOMIZERS (high risk of crashes / softlocks)

HandlingRandomizer = false
WeaponStatsRandomizer = false
ParticleRandomizer = false
ObjectRandomizer = false
RespawnPointRandomizer = false
AnimationRandomizer = false

# General Settings
[EnableRainbomizer]

Seed	  = -1 # -1 for random

Unprotect = true
AutosaveSlot = 8 # Set it to a non-existent slot (eg. -1 or 9) to disable
ModifyCredits = true

# Additional settings for individual randomizers follow.

#######################################################
[ColourRandomizer]

RandomizeCarCols = true # (Car Colours)
RandomizeMarkers = true # (Mission Markers and Arrows)
RandomizeText = true # (HUD elements and menu text)
RandomizeLights = true # (Light emitters e.g. light posts, headlights)
RandomizeClouds = true # (Clouds in the sky)
RandomizeOtherSkyElements = true # (Stars, rainbows, sun, etc)

RainbowHueCycle = false # (All randomized elements will have transition between rainbow colours)

RandomizeFades = false # (In-game fade-in/outs. WARNING: Hard on eyes)
CrazyMode = false # Changes images and menu background (WARNING: VERY hard on eyes)

#######################################################
[TimeCycleRandomizer]

# Randomizes timecyc.dat values, which will change appearance and colours of sky and objects.
# Re-randomizes every fade.
RandomizeTimeCycle = true

# Randomizes weather cycle at regular intervals.
RandomizeWeather = true

#######################################################
[TrafficRandomizer]

# Forces one specific vehicle to spawn in traffic. Set to -1 to not force a vehicle.
ForcedVehicleID = -1

# Change these options to disable certain types of vehicles from spawning in traffic
EnableTrains = true
EnableBoats = true
EnableAircrafts = true
EnableCars = true
EnableBikes = true
EnableTrailers = true

# Ped to spawn in odd Ambulances and Fire Truck
DefaultModel = 0

#######################################################
[ScriptVehicleRandomizer]

EnableExtraTimeForSchools = true

# Allow certain vehicle checks for sub-missions or markers to be skipped so they can be activated with any vehicle

LowriderMissions = true # (Lowrider Minigame, High Stakes Low-Rider)
WuZiMu = true # (Wu Zi Mu start mission marker)
SweetsGirl = true # (Picking up Sweet in Sweet's Girl)
CourierMissions = true # (Courier Missions in LS / SF / LV + BMX Challenge)
NRG500Challenge = true # (NRG-500 Challenge in SF)
ChiliadChallenge = true # (All 3 Chiliad Challenge routes)

# Gives you any vehicle for every mission (You might not be able to proceed with missions with this enabled)
SkipChecks = false

#######################################################
[ParkedCarRandomizer]

RandomizeFixedSpawns = true # (Fixed spawns like Sweet's car)
RandomizeRandomSpawns = true # (For example: Car Parks)

#######################################################
[MissionRandomizer]

# Forces every mission to start a specific mission using its respective ID from the main.scm file. 
# Set to -1 to not force a mission and randomize normally.
ForcedMissionID = -1

# RandomizeOnce - means that each mission marker is always linked to the same random mission
RandomizeOnce = true
RandomizeOnceSeed = "" # https://en.wikipedia.org/wiki/Random_seed - useful for races
ForcedRandomizeOnceSeed = false # Force the seed on existing save files

DisableMainScmCheck = false # Allow custom main.scm's to run with mission randomizer.
		      	    # Note: Missions might not progress properly with a custom
			    #       main.scm file.

#######################################################
[WeaponRandomizer]

RandomizePlayerWeapons = true
SkipChecks = false # Checks related to weapons required for certain missions
		   # You might not be able to complete some missions
		   # without this enabled.

#######################################################
[PickupsRandomizer]

ReplaceWithWeaponsOnly = true # Does nothing yet

#######################################################
[PlayerRandomizer]

# If both activated, you will have a chance to randomize between both other ped models and 
# CJ wearing random clothing at any point.
RandomizePlayerModel = true
RandomizePlayerClothing = true

# Set to true to allow models featuring nudity
IncludeNSFWModels = false

#######################################################
[PedRandomizer]

RandomizeGenericModels = true # (Peds on the street or in interiors, non-named peds in missions)
RandomizeCops = true # (Cops can look like any ped)
RandomizeSpecialModels = true # (Mission characters e.g. Sweet, Big Smoke, Cesar)

# Set to true to allow models featuring nudity
IncludeNSFWModels = false

#######################################################
[CutsceneRandomizer]

RandomizeModels = true
RandomizeLocations = true

#######################################################
[VoiceLineRandomizer]

MatchSubtitles = true

# Force a single voice line. Set to -1 for normal randomization.
ForcedAudioLine = -1

#######################################################
[LanguageRandomizer]

MinTimeBeforeTextChange = 1 # seconds
MaxTimeBeforeTextChange = 5 # seconds

#######################################################
[RiotRandomizer]

# Adds a small chance of riot mode briefly activating as you enter new areas.
RandomizeRiots = true

# Randomizes which light is activated next (green, yellow, red) as well as
# the time between light changes.
RandomizeTrafficLights = true

#######################################################
[WantedLevelRandomizer]

# Randomizes number of wanted stars you get in missions like 2 stars in Drive-by
RandomizeMissionWantedLevels = true

# Randomizes how many "chaos points" you get for each crime.
# You could, for example, get 3 stars for stealing a cop bike, but lose 1 star for killing
# a cop.
RandomizeChaosPoints = false

#######################################################
[CheatRandomizer]

EnableEasterEgg = true

#######################################################
# EXPLANATIONS OF RANDOMIZERS

# Colour Randomizer
# Randomizes the colours of various in-game elements, including cars, the HUD, and fades. 

# Traffic Randomizer
# Randomizes cars that spawn in traffic including law enforcement vehicles.

# Script Vehicle Randomizer 
# Randomizes the vehicles you are given in missions.

# Parked Car Randomizer 
# Randomizes cars which are found parked around the map including airplanes at airports with entirely random spawns.

# Mission Randomizer
# Randomizes which mission is started by each mission marker.

# Weapon Randomizer
# Randomizes the weapons that are wielded by the player and other NPCs.

# Pickups Randomizer
# Randomizes pickups around the map upon starting a new game, as well as pickups during missions.

# Player Randomizer
# Randomizes the player's appearance every fade between other NPC models and random clothing options for CJ.

# Ped Randomizer
# Randomizes the appearance of peds on the street, cops, and mission peds.

# Cutscene Randomizer
# Randomizes the models used in motion-captured cutscenes, as well as the location in which they take place.

# Voice Line Randomizer	
# Randomizes dialogue spoken by characters in missions. (Requires original AudioEvents.txt)

# Language Randomizer
# Randomizes the language of text (except for subtitles when using Voice Line Randomizer). 
# Some types of text will change language at a regular interval which can be adjusted above.

# License Plate Randomizer
# Randomizes the license plates of vehicles to random words from the game script.

# Police Heli Randomizer
# Randomizes the helicopters that the police spawn in with working spotlight and gun.

# Blip Randomizer
# Randomizes every radar blip on the map with a different icon. WARNING: Blips are permanently saved when you save the game.

# Wanted Level Randomizer
# Randomizes how you get wanted levels during and outside of missions.

# DYOM Randomizer
# When using the Design Your Own Mission (DYOM) mod, an option is added to the menu to download and play a 
# random mission from the DYOM website.

# Cheat Randomizer
# Randomizes which cheat activates when you enter a cheat code.

# EXPERIMENTAL RANDOMIZERS (high risk of crashes / softlocks)

# Handling Randomizer
# Shuffles the handlings of vehicles.

# Weapon Stats Randomizer
# Randomizes properties of all weapons, including their damage, fire rate, and clip size. 
# Thanks to SRewo for creating this randomizer.

# Particle Randomizer
# Randomizes all of the game's particle effects.

# Object Randomizer
# Randomizes objects spawned by mission scripts into other objects.

# Respawn Point Randomizer
# Randomizes where you respawn after death or arrest.

# Animation Randomizer
# Randomizes every animation in the game.
)";
