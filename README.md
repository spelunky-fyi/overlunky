# Overlunky
![Continuous Integration](https://github.com/spelunky-fyi/overlunky/workflows/Continuous%20Integration/badge.svg)

An overlay for Spelunky 2 to help you with modding, exploring the depths of the game and practicing with tools like spawning arbitrary items, warping to levels and teleporting made by the cool people from the Spelunky Community Discord.

**Please read the [![open issues](https://img.shields.io/github/issues-raw/spelunky-fyi/overlunky)](https://github.com/spelunky-fyi/overlunky/issues) before complaining about them in the discord.**

[![Overlunky](https://img.shields.io/github/v/release/spelunky-fyi/overlunky?label=Overlunky)](https://github.com/spelunky-fyi/overlunky/releases/latest) is compatible with ![Spelunky](https://img.shields.io/badge/Spelunky2-1.25.2-green) (Steam version)

[![Overlunky](https://img.shields.io/github/v/release/spelunky-fyi/overlunky?include_prereleases&label=Overlunky)](https://github.com/spelunky-fyi/overlunky/releases/tag/whip) (latest prerelease build) is compatible with ![Spelunky](https://img.shields.io/badge/Spelunky2-1.25.2-green) (Steam version)

**Overlunky (or any other modding tool for that matter) does not support the GamePass/Xbox version**, but it might be considered in the future.

[WHIP builds](https://github.com/spelunky-fyi/overlunky/releases/tag/whip) are whipped together automatically from the latest changes and are not tested or documented at all. They are mainly to keep up with the ever changing scripting api and usually contain bugfixes for that. Use at your own discretion, but if you want the latest scripts, get this. ![WHIP commits](https://img.shields.io/github/commits-since/spelunky-fyi/overlunky/latest)

## Disclaimer
You are strongly discouraged from using any modding tools in your actual online Steam installation as to prevent unlocking achievements, ruining or corrupting your savefile and cheating while using online features. You should make a copy of your game somewhere else and install [Mr. Goldbergs Steam Emulator](https://mr_goldberg.gitlab.io/goldberg_emulator/) in the game directory. (Just replace steam_api64.dll with the one in the zip and that copy of the game can't talk to Steam no more!) If you break anything using this tool you get to keep both pieces. Do not report modding related bugs to BlitWorks.

**Steam achievements and game saves are disabled by default while running Overlunky though.**

## Installation and usage
**[YouTube tutorial](https://youtu.be/Zzba4cV9f2c) for kids who can't read good and who wanna learn to do other stuff good too.**

**[Download the latest release](https://github.com/spelunky-fyi/overlunky/releases/latest)** and extract to your game folder. Run the program, leave it running and then start your game, or the other way around! Check the [keyboard shortcuts](#features) and [troubleshooting](#troubleshooting) before asking dumb questions. Overlunky doesn't do any permanent changes to your game, it only exists when you run it.

**Overlunky does NOT work online!** I thought the disclaimer was clear on this... Now I'm not going to stop you from trying, but you'll just find yourself in a world of desync and crashes.

Check the generated `Spelunky 2/overlunky.ini` after running to change shortcut keys and check [entities.txt](https://github.com/spelunky-fyi/overlunky/blob/main/docs/game_data/entities.txt) for a list of numerical entity IDs.

## Features
Current features and their *default* keyboard shortcuts. Note: There's a LOT of useful keys that are not listed here because this list is getting pretty long, check your `overlunky.ini` for cool beans. The default keys are also changed from time to time to make room for better features, so check the key config for your current keys, or delete the ini (section) to revert to default keys.
  - **F1**: Search and spawn entities where you're standing
      + **Ctrl+Enter**: Spawn entity
      + **Tab**: Add selected item id to list when making a kit
      + **Mouse left**: Spawn entity at mouse cursor (drag to set velocity)
      + **Ctrl+Mouse left**: Spawn overlay on hovered entity (drag to offset position) / Spawn or throw single liquid blob
      + **Mouse right**: Teleport to mouse cursor (drag to set velocity)
      + **Mouse middle**: Select or drag safe entities around
      + **Shift+Mouse middle**: Select or drag all entities around (even walls and background)
      + **Ctrl+Mouse middle**: Launch dragged entity with velocity
      + **(Ctrl/Shift+)Mouse 4**: Boom / Big Boom / Nuke!
      + **Mouse 5**: Destroy safe entities
      + **Shift+Mouse 5**: Destroy any entity (really unsafe :)
      + **Ctrl+Mouse 5**: Clone entity
      + **Shift+123...**: Spawn saved kit number
      + **Shift+Tab**: Change P1 layer
      + You can also draw a velocity vector for spawn/teleport by holding the mouse button
      + Dragged entities have noclip on, so you can drag yourself through walls etc
      + Enter multiple numeric IDs like `526 560 570` to spawn them all at once. Useful for making a kit you can `Save` for later use with a single click.
  - **F2**: Warp and make doors to many places
      + **Shift+Enter**: Spawn back layer door
      + **Ctrl+Shift+Enter**: Spawn warp door
      + **Ctrl+W**: Warp to set level
      + **Ctrl+A**: Warp to next level (first/normal exit)
      + **Ctrl+B**: Warp to next level (second/special exit)
  - **F3**: Camera controls
      + **Ctrl+,**: Zoom in
      + **Ctrl+.**: Zoom out
      + **Ctrl+0**: Auto fit level width to screen
      + **Ctrl+2345**: Zoom to X level width
      + **Shift+ArrowKey**: Move camera in desired direction
      + **Mouse 4**: Drag camera around or focus on an entity
      + **Mouse 4 longpress**: Reset camera focus to player
      + **Mouse 4+Wheel**: Zoom
      + **Ctrl+Wheel**: Zoom
  - **F4**: Entity properties
      + See and change a lot of variables of the selected entity
      + **Mouse middle**: Select or drag safe entities around
      + **Shift+Mouse middle**: Select or drag all entities around (even walls and background)
      + **Ctrl+Mouse middle**: Launch dragged entity with velocity
  - **F5**: Game state
      + See and change a lot of variables from the game state
      + **Ctrl+Space**: Toggle game engine pause
      + **(Shift+)Space**: Advance frame when engine is paused
  - **F6**: Save editor
      + Edit your save data.
      + Note: Doesn't automatically save to file, you need to let the game trigger a save for that.
  - **F8**: Scripts
      + You can load, edit and set options for Lua scripts here
      + **Ctrl+F5**: Reload enabled scripts
  - **F9**: Options
      + Lots of options. **Check the key config for hotkeys for these and other stuff!**
  - **F11**: Hide overlay
  - **\~ (or your native Console Key)**: Toggle developer console
    - The console is its own script environment, where you can run some quick oneliners or paste snippets in.
    - **Enter**: Execute
    - **Ctrl+Enter**: New line
    - **"clear/clr/cls"**: Clear history
    - **"reset/reload"**: Clear all callbacks made in the console
    - **"quit"**: Ragequit
  - **Ctrl+Shift+K**: Edit keybindings
  - **Ctrl+Shift+U**: Change UI color. You can also change the font in the ini.
  - **Ctrl+Shift+I**: ImGui internal metrics and settings
  - **Ctrl+Shift+S**: Save options, hotkeys and UI colors
  - **Ctrl+Shift+L**: Load options, hotkeys and UI colors
  - **Ctrl+Shift+D**: Detach active tab in tabbed interface
  - **Enter**: Run active tool
  - **Escape**: Return focus to game from an input
  - **Q**: Quick start from the intro or menu
  - **Ctrl+Q**: Quick restart from anywhere
  - **Ctrl+C**: Quick return to camp from anywhere

### Scripts
Lua scripting is still buggy and unfinished and the **API might change**, although we will try to deprecate old stuff without breaking anything. There's some [documentation](https://github.com/spelunky-fyi/overlunky/blob/main/docs/script-api.md), [internal details](https://github.com/spelunky-fyi/overlunky/tree/main/docs) and [examples](https://github.com/spelunky-fyi/overlunky/tree/main/examples) to get you started. The examples should be considered [Unlicensed](https://unlicense.org/), so please DO NOT credit the author when copying them. Scripts are loaded from `Spelunky 2/Overlunky/Scripts` by default, but you can change this in the ini. Optionally you can also load *main.lua* files from `Mods/Packs`. Check the [Playlunky documentation](https://github.com/spelunky-fyi/Playlunky/wiki#script-mods) on how to run your scripts as PL mods. Set `OL_DEBUG=1` in the *game environment* to enable logging script messages to console.

**Scripts don't work online either!** While technically they can work, and there's some work being done on this, yours truly has little interest in making any online features.

## Troubleshooting
  - If your game **crashes** when launching Overlunky or it just closes and you **don't see the overlay** in game:
    + Make sure you are running the latest version of each. We don't support old game versions and sometimes not the too recent ones either. The latest somewhat tested version can be found at the top if this document.
    + Make sure your antivirus is not blocking it. **This is very likely if you're using one.** Overlunky is very hacky and acts like malware. It isn't, but you can audit the source code and build it yourself if you want.
    + Try deleting `Spelunky 2/overlunky.ini` or `Spelunky 2/imgui.ini`.
    + Try the WHIP build, it might have already fixed the problem.
  - If your game **crashes** when actually trying to use the tools:
    + See previous section.
    + Some entities just crash the game, try to avoid them next time.
    + Some scripts just crash the game too, the api is not very safe.
  - If your game **crashes** and you're using **Modlunky2 or Playlunky**:
    + While we try to keep all the modding tools compatible, some things may break sometimes.
    + Check the [open issues](https://github.com/spelunky-fyi/overlunky/issues) for known compatibility problems with other tools.
  - If you're **missing** some overlay **windows** or tabs:
    + Try resetting window positions. (See hotkeys above.)
    + Delete `Spelunky 2/imgui.ini` to reset tool window positions that might be outside the screen region for whatever reason.
    + Maybe the overlay is just really tiny or collapsed? You can resize it. Just try your magic with the mouse.
  - If you mess up your **keyboard shortcuts** or UI:
    + Delete `Spelunky 2/overlunky.ini`.
  - If stuff just don't work and you're running **Windows 7** or a machine not designed to play games
    + You're on your own here. We don't test on or make this for ancient software/hardware.
  - If you have problems with **OBS**
    + Use *Game Capture* and disable *Allow Transparency* if Overlunky looks weird/black.
    + If you have lag or crashes, check previous question.
  - If you're running **Linux**
    + The game and OL should run fine with the latest Wine if you install DXVK.
    + Also works with [Proton-GE](https://github.com/GloriousEggroll/proton-ge-custom) using the [command line switch](#command-line-switches) `--launch_playlunky ..` for a non-Steam game shortcut. Just make sure the game and the OL shortcut are set to use Proton-GE.
    + Put the Segoe UI ttf files to your wineprefixes Windows/Fonts to show the ui correctly.
    + You're still mostly on your own here, this is a Windows 10 program for a Windows 10 game.
  - If you **don't have scripts** in the Scripts tab
    + You didn't put them in the right place. [Follow](#installation-and-usage) the [instructions](#scripts).

## Development
If you'd rather build it yourself or help with development, clone the repository, get some C++ build tools and run this to get started:
```bash
cd overlunky
git submodule update --init --recursive
mkdir build
cd build
cmake ..
cmake --build . --config Release --target ALL_BUILD
```
The binaries will be in `build/bin/Release/`. You can also try the scripts in `.vscode` with VSCode. Set `OL_DEBUG=1` in the *game environment* to enable logging to console.

## Command line switches
  - `--launch_game <path_to_game_directory>` will launch Spel2.exe from that directory and hook to the game process, rather than trying to find it.
  - `--launch_playlunky <path_to_game_directory>` will launch playlunky_launcher.exe from that directory and hook to the game process.
These can be used with Steam, also on Linux, by adding Overlunky.exe as a non-Steam game and adding to launch options.
