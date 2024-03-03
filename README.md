![Build](https://github.com/Parik27/Rainbomizer/workflows/Build/badge.svg)
[![Discord](https://img.shields.io/discord/681996979974570066.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/BYVBQw7)
# Rainbomizer 🌈

<img src="https://i.ibb.co/Qc44GgF/Spinning.gif">

Rainbomizer is a randomizer for Grand Theft Auto: San Andreas.

## Features 🌈

- **Colour Randomizer**: Randomizes the colours of various in-game elements, including cars, the HUD, and fades. Includes 128 colours hue cycle.
- **(NEW) Timecycle Randomizer**: Randomizes the appearance and colours of the game world, such as the sky, water, lighting, and other time-based elements. Also includes weather randomization. *Use with caution if you are sensitive to bright lights and colours.*
- **Traffic Randomizer**: Randomizes cars that spawn in traffic including law enforcement vehicles.
- **Script Vehicle Randomizer**: Randomizes the vehicles you are given in missions. The randomizer makes sure that you get vehicles that are usable for the mission.
- **Parked Car Randomizer**: Randomizes cars which are found parked around the map including boats and airplanes at airports, with entirely random spawns.
- **Mission Randomizer**: Randomizes which mission is started by each mission marker. When the randomized mission is complete, the game progresses as if you completed the original.
- **Weapon Randomizer**: Randomizes the weapons that are wielded by the player and other NPCs.
- **Pickup Randomizer**: Randomizes all pickups to other random weapons and pickups, including spawns created on starting a new game, mission pickups, and drops from dead peds.
- **(NEW) Player Randomizer**: Randomizes the player's apperance every fade between other NPC models in the game and/or CJ wearing random clothing options.
- **(NEW) Ped Randomizer**: Randomizes the appearance of every single ped, including those on the street, cops, gang members, and mission characters.
- **Cutscene Randomizer**: Randomizes the models used in motion-captured cutscenes, as well as the location in which they take place.
- **Sound Randomizer**: Randomizes dialogues spoken by characters in missions. *(Requires original AudioEvents.txt)* This can also randomize the generic voice lines spoken by peds on the street and various other sound effects.
- **(NEW) Language Randomizer**: Randomize the language of in-game text between other game languages.
- **License Plate Randomizer**: Randomizes the license plates of vehicles to random words from the game script.
- **(NEW) Riot Randomizer**: Activates a small chance for the game's riot mode to randomly activate upon entering new zones, as well as an additional feature that randomizes the timing and order of Traffic Lights.
- **Police Helicopter Randomizer**: Randomizes the helicopters that the police spawn in with working spotlight and gun.
- **Blip Randomizer**: Randomizes every radar blip on the map with a different icon.
- **Wanted Level Randomizer**: Randomizes how you get wanted levels during and outside of missions.
- **DYOM Randomizer**: Adds an option to download and play a random mission from the DYOM website.
- **Cheat Randomizer**: Randomizes which cheat activates when you enter a cheat (disabled by default).

#### Experimental Randomizers (expect crashes / softlocks)

- **Handling Randomizer**: Shuffles the handlings of vehicles.
- **Particle Randomizer**: Randomizes all of the game's particle effects.
- **Weapon Stats Randomizer**: Randomizes properties of all weapons, including their damage, fire rate, and clip size.
- **Object Randomizer**: Randomizes objects spawned by mission scripts into other objects.
- **Respawn Point Randomizer**:  Randomizes where you respawn after getting busted or wasted.
- **Animation Randomizer**:  Randomizes every animation in the game into a different animation.

## Supported Versions 🌈

* GTA San Andreas - v1.00

Other versions of the game *will not* work and will most likely cause a crash.

Check this link if you wish to downgrade a later version to a compatible version - https://dl1.rockstarnexus.com/gta/sa/GTASA_12-9-14_Steam_to_V1_DG.exe

## Installation 🌈

[Tutorial by SpeedyFolf](https://www.youtube.com/watch?v=HRz_zTuctKc)

Installation requires an ASI Loader to be installed.

[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) by [ThirteenAG](https://github.com/ThirteenAG) is a popular option since it supports CrashDumps, which will be necessary if you want to report a crash.

If you already have mods like CLEO, you probably already have an ASI Loader installed. 

1. To install the Ultimate ASI Loader, simply extract the Ultimate ASI Loader's dinput8.dll file to the game's root directory (Folder where the game is installed).
2. To install Rainbomizer, download the release archive from [Releases](https://github.com/Parik27/Rainbomizer/releases) and extract the archive to the root directory.

## Configuration 🌈

If you wish to change any of the mod's functionalities, for example to disable a specific randomizer, a configuration file (`config.toml`) has been provided with the mod. The config file is located in the `rainbomizer` folder in the game's root directory.

The default configuration file is in the main repository, `config.toml`, and is automatically created by the mod if it doesn't exist in the rainbomizer folder.

The configuration file is in the TOML format. It is similar in structure to INI's, but with a defined and better structure. It can be opened and edited with a basic text editor such as Notepad++.

To configure a specific randomizer, search for its name in the section listing all of the Randomizers at the top of the file. You can set the "Enabled" key to 'true' or 'false' to enable or disable it respectively.

Many randomizers allow you to further customise how each individual randomizer works via further sub-options as you proceed through the file. These options can allow you to enable or disable certain things about each randomizer, increase or decrease odds of specific things happening, or force something specific to always happen, such as a certain mission or vehicle spawn.

Please note that randomizers listed under the `Experimental` section are disabled by default as they are not thoroughly tested and are known to cause crashes and / or softlocks. Enable such options at your own discretion.

More information about how to configure each specific randomizer option is provided in the default config file.

## Mod Support 🌈

This mod features full compatibility with all of the effects in the [GTA Trilogy Chaos Mod](https://github.com/gta-chaos-mod/Trilogy-ASI-Script) by [Lordmau5](https://github.com/Lordmau5) for those who wish to use the two mods together. As they are often combined, we made an effort to make sure it was supported by the Rainbomizer.

The mod also supports the [Design Your Own Mission mod](https://www.gtagarage.com/mods/show.php?id=5038) as shown by the DYOM Randomizer - all randomizers are compatible except for Language Randomizer which is recommended to be disabled.

Please note that compatibility with other mods not mentioned here is not guaranteed, and we recommend removing them if you are having frequent crashes or other issues, or else disabling randomizers which you think may not be functional with them. However, minor mods such as those that modify a vehicle's appearance are likely to work.

## Credits 🌈

#### Lead Developers

- [Parik](https://github.com/Parik27) - Creation and implementation of the initial ASI.
- [123robot](https://www.twitch.tv/123robot) - Development of several new features and improvements for v3.0, as well as previous assistance with creating external files, testing, and other support.
- [GTA Madman](https://github.com/GTAMadman) - Assistance with the creation of Ped and Player Randomizers, as well as contributing other fixes, feedback, and extensive testing.

#### Original Concept

- [NABN00B](https://www.twitch.tv/nabn00b) - Gave the original idea for a car randomizer for San Andreas.
- [MrMateczko](https://www.twitch.tv/mrmateczko_) - Special mention for sharing a modded cargrp file that led to the original idea.
- [Veigar](https://gtaforums.com/profile/685882-veigar) - Idea for the Parked Car/Texture Randomizer and support during early development.

#### Additional Contributors

- [iguana](https://www.twitch.tv/iguana_) - Assisted with looking through code and helped inspire the mission randomizer.
- [SpeedyFolf](https://www.twitch.tv/speedyfolf) - Creating the tutorial video, beta testing, and feedback during development.
- [SRewo](https://github.com/SRewo) - Creating the Weapon Stats Randomizer.

#### Beta Testers

- [Fryterp23](https://www.twitch.tv/fryterp23)
- [Hugo_One](https://www.twitch.tv/hugo_one)
- [Lordmau5](https://www.twitch.tv/lordmau5)
- [Riekelt](https://www.twitch.tv/riekelt)
- [Waris](https://www.twitch.tv/wariscoach)

## Used Libraries 🌈

- [injector](https://github.com/thelink2012/injector) (thelink2012) - zlib License
- [cpptoml](https://github.com/skystrife/cpptoml) - (skystrife) - MIT License

## Contribution 🌈

You can contribute to the project by reporting any crashes/issues you encounter in the [Issues](https://github.com/Parik27/Rainbomizer/issues) section of the repository.

Pull requests are welcome, but are required to follow the current style used in the project.

You can also keep up with or contribute to development as it happens by [joining the official Rainbomizer Discord!](https://discord.gg/BYVBQw7) You can give #suggestions, ask for #support, or view current development in #sa.
