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
EnableRainbomizer = true		# Set to false to disable all Rainbomizer features regardless of other settings

ColourRandomizer = true			# Randomizes the colours of various in-game elements, including cars, the HUD, and fades. 
TrafficRandomizer = true		# Randomizes cars that spawn in traffic including law enforcement vehicles.
ScriptVehicleRandomizer = true	# Randomizes the vehicles you are given in missions.
ParkedCarRandomizer = true		# Randomizes cars which are found parked around the map including airplanes at airports with entirely random spawns.
MissionRandomizer = true		# Randomizes which mission is started by each mission marker.
WeaponRandomizer = true			# Randomizes the weapons that are wielded by the player and other NPCs.
PickupsRandomizer = true		# Randomizes pickups around the map upon starting a new game, as well as pickups during missions.
PlayerRandomizer = true			# Randomizes the player's appearance every fade between other NPC models and random clothing options for CJ.
PedRandomizer = true			# Randomizes the appearance of peds on the street, cops, and mission peds.
CutsceneRandomizer = true		# Randomizes the models used in motion-captured cutscenes, as well as the location in which they take place.
VoiceLineRandomizer = true		# Randomizes dialogue spoken by characters in missions. (Requires original AudioEvents.txt)
LanguageRandomizer = true		# Randomizes the language of text except for subtitles. Some types of text will change language at a regular interval.
LicensePlateRandomizer = true	# Randomizes the license plates of vehicles to random words from the game script.
PoliceHeliRandomizer = true		# Randomizes the helicopters that the police spawn in with working spotlight and gun.
BlipRandomizer = true			# Randomizes every radar blip on the map with a different icon. WARNING: Blips are permanently saved when you save the game.
WantedLevelRandomizer = true	# Randomizes how you get wanted levels during and outside of missions.
DYOMRandomizer = true			# When using Design Your Own Mission, adds an option to the menu to download and play a random mission from the DYOM website.
CheatRandomizer = false			# Randomizes which cheat activates when you enter a cheat code.
HandlingRandomizer = false		# (EXPERIMENTAL) Shuffles the handlings of vehicles.
WeaponStatsRandomizer = false	# (EXPERIMENTAL) Randomizes properties of all weapons, including their damage, fire rate, and clip size. Thanks to SRewo for creating this randomizer.
ParticleRandomizer = false		# (EXPERIMENTAL) Randomizes all of the game's particle effects.
ObjectRandomizer = false		# (EXPERIMENTAL) Randomizes objects spawned by mission scripts into other objects.
RespawnPointRandomizer = false	# (EXPERIMENTAL) Randomizes where you respawn after death or arrest.
AnimationRandomizer = false		# (EXPERIMENTAL) Randomizes every animation in the game.

# General Settings
[EnableRainbomizer]

Seed	  = -1 # -1 for random

Unprotect = true
AutosaveSlot = 8 # Set it to a non-existent slot (eg. -1 or 9) to disable
ModifyCredits = true

# Additional settings for individual randomizers follow.

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
[ColourRandomizer]

RandomizeCarCols = true # (Car Colours)
# TODO: RandomizeMarkers = true # (Mission Markers and Arrows)
RandomizeFades = false # (In-game fade-in/outs. WARNING: Hard on eyes)
RainbowText = true # (HUD elements and menu text)

RainbowHueCycle = false # (All randomized elements will have transition between rainbow colours)

# CrazyMode = false # Changes images and menu background (WARNING: VERY hard on eyes)
Exceptions = [0, 1] # These are exceptions to car colours. (Default: Black and white car colours)

#######################################################
[CheatRandomizer]

EnableEasterEgg = true

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
[ParkedCarRandomizer]

RandomizeFixedSpawns = true # (Fixed spawns like Sweet's car)
RandomizeRandomSpawns = true # (For example: Car Parks)

#######################################################
[MissionRandomizer]

# Forces every mission to start a specific mission using its respective ID from the main.scm file. Set to -1 to not force a mission and randomize normally.
ForcedMissionID = -1

# RandomizeOnce - means the starting a specific mission will give the same mission for every new game/load game
RandomizeOnce = true
RandomizeOnceSeed = "" # https://en.wikipedia.org/wiki/Random_seed - useful for races
ForcedRandomizeOnceSeed = false # Force the seed on existing save files

DisableMainScmCheck = false # Allow custom main.scm's to run with mission randomizer.
		      	    # Note: Missions might not progress properly with a custom
			    #       main.scm file.

#######################################################
[CutsceneRandomizer]

RandomizeModels = true
RandomizeLocations = true

#######################################################
[PlayerRandomizer]

#######################################################
[WantedLevelRandomizer]

# Randomizes number of wanted stars you get in missions like 2 stars in Drive-by
RandomizeMissionWantedLevels = true

# Randomizes how many "chaos points" you get for each crime.
# You could, for example, get 3 stars for stealing a cop bike, but lose 1 star for killing
# a cop.
RandomizeChaosPoints = false

#######################################################
[VoiceLineRandomizer]

MatchSubtitles = true
ForcedAudioLine = -1 # Force a single voice line. Set to -1 for normal randomization.

#######################################################
[LanguageRandomizer]

MinTimeBeforeTextChange = 1 # seconds
MaxTimeBeforeTextChange = 5 # seconds

#######################################################
[ScriptVehicleRandomizer]

# Gives you any vehicle for every mission (You might not be able to proceed with missions with this enabled)
SkipChecks = false
)";
