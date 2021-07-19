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