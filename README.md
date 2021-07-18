![Build](https://github.com/Parik27/Rainbomizer/workflows/Build/badge.svg)
[![Discord](https://img.shields.io/discord/681996979974570066.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/BYVBQw7)
# Rainbomizer 🌈

<img src="https://media.discordapp.net/attachments/419957309192536066/602097860292509718/spinning.gif">

Rainbomizer is a randomizer for Grand Theft Auto: San Andreas.

## Features 🌈

- **Colour Randomizer**: Randomizes the colours of various in-game elements, including cars, the HUD, and fades. Includes 128 colours hue cycle. The intensity of this randomizer can be modified extensively in the configuration file.
- **(NEW) Timecycle Randomizer**: Randomizes the appearance and colours of the game world, such as the sky, water, lighting, and other time-based elements. Also includes weather randomization which can be configured separately. *Use with caution if you are sensitive to bright lights and colours.*
- **Traffic Randomizer**: Randomizes cars that spawn in traffic including law enforcement vehicles.
- **Script Vehicle Randomizer**: Randomizes the vehicles you are given in missions. The randomizer makes sure that you get vehicles that are usable for the mission. For unrecognised scripts not in the default game, the randomizer will use a set of generic patterns to try and ensure compatibility.
- **Parked Car Randomizer**: Randomizes cars which are found parked around the map including boats and airplanes at airports, with entirely random spawns. This can be configured to only randomize between the same type of parked vehicle.
- **Mission Randomizer**: Randomizes which mission is started by each mission marker. When the randomized mission is complete, the game progresses as if you completed the original. Additional features include missions that take place in locked-out areas unlocking during the mission, the ability for the same mission marker to always give the same randomized mission, and the ability for the player to be teleported back to the original mission's end point upon mission completion.
- **Weapons Randomizer**: Randomizes the weapons that are wielded by the player and other NPCs.
- **Pickup Randomizer**: Randomizes all pickups to other random weapons and pickups, including spawns created on starting a new game, mission pickups, and drops from dead peds.
- **(NEW) Player Randomizer**: Randomizes the player's apperance every fade between other NPC models in the game and/or CJ wearing random clothing options.
- **(NEW) Ped Randomizer**: Randomizes the appearance of every single ped, including those on the street, cops, gang members, and mission characters. This only affects their appearance, and so their behaviour will remain the same (e.g. GSF members randomized to look like someone else can still be recruited).
- **Cutscene Randomizer**: Randomizes the models used in motion-captured cutscenes, as well as the location in which they take place.
- **Voice Line Randomizer**: Randomizes dialogues spoken by characters in missions. *(Requires original AudioEvents.txt)* This can also randomize the generic voice lines spoken by peds on the street and various other sound effects.
- **(NEW) Language Randomizer**: Randomize the language of in-game text between other game languages. Some types of text will change at regular intervals defined within the config file.
- **License Plate Randomizer**: Randomizes the license plates of vehicles to random words from the game script.
- **(NEW) Riot Randomizer**: Activates a small chance for the game's riot mode to randomly activate upon entering new zones, as well as an additional feature that randomizes the timing and order of Traffic Lights.
- **Police Helicopters Randomizer**: Randomizes the helicopters that the police spawn in with working spotlight and gun.
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
- [123robot](https://www.twitch.tv/123robot) - Development of several new features and improvements for V3.0, as well as previous assistance with external files, data gathering, testing and other support.
- [GTA Madman](https://github.com/GTAMadman) - Assistance with the creation of Ped and Player Randomizers, as well as contributing other fixes, feedback, and extensive testing.

#### Original Concept

- [NABN00B](https://www.twitch.tv/nabn00b) - Gave the original idea for a car randomizer for San Andreas.
- [MrMateczko](https://www.twitch.tv/mrmateczko_) - Special mention for sharing a modded cargrp file that led to the original idea.
- [Veigar](https://gtaforums.com/profile/685882-veigar) - Idea for the Parked Car/Texture Randomizer and support during early development.

#### Additional Contributors

- [iguana](https://www.twitch.tv/iguana_) - Assisted with looking through code and helped inspire the mission randomizer.
- [opain-replika](https://github.com/opain-replika) - Identified issue with Kickstart vehicles.
- [sisco0](https://github.com/sisco0) - Implemented the config option for random car colours.
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

## Frequently Asked Questions (FAQ) 🌈

If you have questions about the mod or other issues installing the mod or getting it to work, check if your query is responded to here. Otherwise, you can ask for help in the [Rainbomizer Discord server](https://discord.gg/BYVBQw7).

#### When I try to start the game, I get a warning message about an ASI error and/or the file gets deleted and the game plays without any changes.
This means you have an anti-virus that is blocking the mod from functioning. Either add an exception for the SA.Rainbomizer.ASI or disable your anti-virus when playing.

#### The Mission Randomizer is enabled but missions aren't random or they don't pass properly upon completion.
This most likely means that you do not have the correct main.scm file. [Click here to download a main.scm supported by the mod](https://cdn.discordapp.com/attachments/681997113751502870/695376713651322980/main.scm). Make a backup of your existing file and place the new one in the data/scripts folder of your game installation.

#### The game is crashing after I activated Experimental Randomizers (such as Particles, Weapon Stats, or Objects).
As noted above, the experimental randomizers are not fully functional and are known to crash or otherwise softlock the game. If you want the smoothest experience, these should all be disabled, and are just there for you to have fun experimenting with if you wish to, but are not suited to a full playthrough.

#### I'm having other trouble installing the mod.
First you should re-check that you've followed all your steps. Is your game downgraded to V1? If our provided downgrader doesn't work for you, you may wish to find an alternative. Did you download the Ultimate ASI Loader? If so, make sure you did not install the x64 version. If you already have a dinput8.dll file in your SA directory such as for a mouse fix, you can rename the ASI Loader to an alternative such as vorbisFile.dll or d3d8.dll. Again, make sure your anti-viruses and/or Windows Defender are not blocking the mod. If none of this works for you, [join our Discord](https://discord.gg/BYVBQw7) and ask for help in the #support channel.

#### How does the Mission Randomizer work exactly?
All of the missions are started at the usual markers that'd start at, such as first marker being the one at Grove Street for Big Smoke. However, the mission the marker starts will be randomized to a different mission that you must complete. This mission will always be the same if `RandomizeOnce` is enabled and will change each time you start it if not. Each marker has a different random mission linked to it which is determined by a seed that the mod randomly generates or one that you set yourself in the config file. During the course of the random mission, the cities unlocked will correspond to those that are normally unlocked during that mission, and will revert back if you fail or pass. Upon completing the mission, you will be teleported back to where you would have ended the mission that normally started at the marker you originally entered, and the game will progress as if you completed the original mission, meaning completing a randomized mission in place of Big Smoke still unlocks The Johnson House and the next mission (Ryder).

#### Can I make it so that the models in cutscenes don't have those broken jaws?
Yes, scroll down to the section for CutsceneRandomizer and set `UseOnlyNormalCutsceneModels` to true. Every ped model is included by default for fun and variety, but only certain models designed for cutscenes can use proper facial animations which is why this occurs.

#### I don't like [feature] and want to disable it.
There is a high chance that anything you want to disable or change can be using the config file. See the Configuration section above for more details. If there is something you want to change and you don't see it there, you can give us feedback about it on the [Rainbomizer Discord](https://discord.gg/BYVBQw7).

#### Can I use this mod with the Chaos Mod?
Yes, they are fully compatible together and every effect works with every stable randomizer. See the Mod Support section above for more details.

#### Can I use this mod with other types of mods?
We cannot guarantee compatibility with any other mods, but most that are not Total Conversions should be stable, however if you have frequent crashes or other issues, it is most likely another mod you are combining with the Rainbomizer making it unstable. Total Conversion mods most likely will not function very well as the mod is primarily made for the normal game, however certain randomizers may still work. See the Mod Support section above for more details.
