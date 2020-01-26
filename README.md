# Rainbomizer 🌈

<img src="https://media.discordapp.net/attachments/419957309192536066/602097860292509718/spinning.gif">

Rainbomizer is a randomizer for Grand Theft Auto: San Andreas. 

## Features 🌈

- **Traffic Randomizer**: Randomizes cars that spawn on traffic including law enforcement vehicles.
- **Parked Car Randomizer**: Randomizes cars which are found parked around the map including airplanes at airports with entirely random spawns.
- **Script Vehicle Randomizer**: Randomizes the vehicles you are given in missions. The randomizer makes sure that you get vehicles that are usable for the mission.
- **(NEW) Colour Randomizer**: Randomizes the colours of various in-game elements, including cars, the HUD, and fades. Includes 128 colours hue cycle. The intensity of this randomizer can be modified extensively in the configuration file.
- **(NEW) Mission Randomizer**: Randomizes which mission is started by each mission marker. When the randomized mission is complete, the game progresses as if you completed the original. Additional features include missions that take place in locked-out areas unlocking during the mission, the ability for the same mission marker to always give the same randomized mission, and the ability for the player to be teleported back to the original mission's end point upon mission completion.
- **Weapons Randomizer**: Randomizes the weapons that are dropped/wielded by the player/enemies.
- **Pickup Randomizer**: Randomizes weapon pickups scattered around the map, as well as the appearance of certain other types of pickups.
- **Voice Line Randomizer**: Randomizes dialogues spoken by characters in missions. *(Requires original AudioEvents.txt)*
- **(NEW) Cutscene Randomizer**: Randomizes the models used in motion-captured cutscenes, as well as the location in which they take place.
- **(NEW) Blip Randomizer**: Randomizes every radar blip on the map with a different icon.
- **(NEW) Particle Randomizer**: Randomizes all of the game's particle effects.
- **Police Helicopters Randomizer**: Randomizes the helicopters that the police spawn in with working spotlight and gun.
- **Cheat Randomizer**: Randomizes which cheat activates when you enter a cheat.
- **License Plate Randomizer**: Randomizes the license plates of vehicles to a random word.
- *(Experimental)* **Handling Randomizer**: Shuffles the handlings of vehicles.
- *(Experimental)* **(NEW) Object Randomizer**: Randomizes objects spawned by mission scripts into other objects.

## Supported Versions 🌈

* GTA San Andreas - v1.0

Other versions of the game *will not* work and will most likely cause a crash.

Check this link if you wish to downgrade a later version to a compatible version - https://dl1.rockstarnexus.com/gta/sa/GTASA_12-9-14_Steam_to_V1_DG.exe

## Installation 🌈

Installation requires an ASI Loader to be installed. 

[Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) by [ThirteenAG](https://github.com/ThirteenAG) is a popular option since it supports CrashDumps, which will be necessary if you want to report a crash.

If you already have mods like CLEO, you probably already have an ASI Loader installed. 

1. To install the Ultimate ASI Loader, simply extract the Ultimate ASI Loader's dinput8.dll file to the game's root directory (with gta_sa.exe), and rename it to `vorbisFile.dll` if necessary (For Example, while using dinput8.dll for mouse fix already)
2. To install Rainbomizer, download the release archive from [Releases](https://github.com/Zarig/Rainbomizer/releases) and extract the asi and the toml file to the `scripts` folder or the root directory.

## Configuration 🌈

If you wish to change any of the mod's functionalities, for example to disable a specific randomizer, a configuration file (`rainbomizer.toml`) has been provided with the mod.

The default configuration file is in the main repository, `config.toml`, but it is automatically created by the mod if it doesn't exist in the same folder as the asi file.

The configuration file is in the TOML format. It is similar in structure to ini's but with a defined and better structure. 

To configure a specific randomizer, search for its name in the configuration file. You can set the "Enabled" key to 'true' or 'false' to enable or disable it respectively.

More information about how to configure specific randomizers is provided in the default config file.

## Credits 🌈

- [NABN00B](https://www.twitch.tv/nabn00b) - For the original idea for a car randomizer for San Andreas.
- [GTA Madman](https://twitch.tv/gta_madman) - For contributing fixes and extensive testing
- [123robot](https://www.twitch.tv/123robot) - For support through the development and streaming Rainbomizer playthroughs
- [Veigar](https://gtaforums.com/profile/685882-veigar/) - Idea for the Parked Car/Texture Randomizer and support throughout the development phase
- [MrMateczko](https://www.twitch.tv/mrmateczko_) - Special mention for sharing a modded cargrp file that led to the original idea.
- [Waris](https://www.twitch.tv/wariscoach) - Streaming a playthrough of the Rainbomizer.
- [Lordmau5](https://www.twitch.tv/lordmau5/) - Special mention for supporting and testing.

## Used Libraries 🌈

- [injector](https://github.com/thelink2012/injector) (thelink2012) - zlib License
- [cpptoml](https://github.com/skystrife/cpptoml) - (skystrife) - MIT License

## Contribution 🌈

You can contribute to the project by reporting any crashes/issues you encounter in the [Issues](https://github.com/Zarig/Rainbomizer/issues) section of the repository

Pull requests are welcome but are required to follow the current style used in the project.

You can also keep up with or contribute to development as it happens by [joining the linked Discord](http://discord.gg/3dRXnjC) and following the #rainbomizer-and-coding channel.
