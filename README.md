# Overlunky
![Continuous Integration](https://github.com/spelunky-fyi/overlunky/workflows/Continuous%20Integration/badge.svg)

An overlay for Spelunky 2 to help you with modding, exploring the depths of the game and practicing with tools like spawning arbitrary items, warping to levels and teleporting made by the cool people from the [Spelunky Community Discord](https://discord.gg/spelunky-community).

**Please read the [![open issues](https://img.shields.io/github/issues-raw/spelunky-fyi/overlunky)](https://github.com/spelunky-fyi/overlunky/issues) before complaining about them in the discord.**

[![Overlunky](https://img.shields.io/github/v/release/spelunky-fyi/overlunky?include_prereleases&label=Overlunky)](https://github.com/spelunky-fyi/overlunky/releases/tag/whip) (latest auto build) is compatible with ![Spelunky](https://img.shields.io/badge/Spelunky2-1.28-green) (Steam version)

**Overlunky (or any other modding tool for that matter) does not support the GamePass/Xbox version**, but it might be considered in the future.

## Disclaimer
You are strongly discouraged from using any modding tools in your actual online Steam installation as to prevent unlocking achievements, ruining or corrupting your savefile and cheating while using online features. You should make a copy of your game somewhere else and install [Mr. Goldbergs Steam Emulator](https://mr_goldberg.gitlab.io/goldberg_emulator/) in the game directory. (Just replace steam_api64.dll with the one in the zip and that copy of the game can't talk to Steam no more!) If you break anything using this tool you get to keep both pieces. Do not report modding related bugs to BlitWorks.

**Steam achievements and game saves are disabled by default while running Overlunky though.**

## Installation and usage
  - ### Just use [Modlunky2](https://github.com/spelunky-fyi/modlunky2/releases/latest/) Overlunky tab to download and update OL, but please read the rest of the documentation too!
  - **[Download the latest WHIP build](https://github.com/spelunky-fyi/overlunky/releases/tag/whip)** and extract to your game folder, keeping the folder structure.
  - Run the program, leave it running and then start your game, or the other way around!
    + It won't automatically start your game, so don't hold your breath waiting for it
    + You can press ENTER in the launcher to try launch the game from the parent folder
  - Check the [keyboard shortcuts](#features) and [troubleshooting](#troubleshooting) before asking dumb questions
  - It doesn't do any permanent changes to your game, it only exists when you run it

**Overlunky does NOT work online!** I thought the disclaimer was clear on this... Now I'm not going to stop you from trying, but you'll just find yourself in a world of desync and crashes.

## WHIP build and automatic updates

[WHIP builds](https://github.com/spelunky-fyi/overlunky/releases/tag/whip) are whipped together automatically from the latest changes and may not be very well tested or documented. This is usually the release to use with the latest version of the game. Numbered releases are made for legacy purposes when a new, incompatible game version comes out.

**WHIP also has an auto-update system to check for a new version on every launch. This won't update the bundled scripts though, so you might want to do a manual update from time to time with the download button in Modlunky2.**

## Features

Current features and their *default* keyboard shortcuts.

  - There's a LOT of useful keys that are not listed here because this list is getting pretty long, check settings or `overlunky.ini` for cool beans.
  - The default keys are also changed from time to time to make room for better features, so check the key config for your current keys, or delete the ini (section) to revert to default keys.
  - **F1**: Search and spawn entities where you're standing
      + **Ctrl+Enter**: Spawn entity
      + **Tab**: Add selected item id to list when making a kit
      + **Mouse left**: Spawn entity at mouse cursor (drag to set velocity)
      + **Ctrl+Mouse left**: Spawn overlay on hovered entity (drag to offset position) / Spawn or throw single liquid blob
      + **Alt+Mouse left**: Hold to draw floors or other entities on grid
      + **Mouse right**: Teleport to mouse cursor (drag to set velocity) / Remove entity from hotbar
      + **Mouse middle**: Select or drag safe entities around (doubleclick to clear selection)
      + **Alt+Mouse middle**: Select multiple entities in the finder
      + **Shift+Mouse middle**: Select or drag all entities around (even walls and background)
      + **Ctrl+Mouse middle**: Launch dragged entity with velocity
      + **Mouse 5**: Destroy safe entities
      + **Alt+Mouse 5**: Erase multiple entities safely
      + **Shift+Mouse 5**: Destroy any entity (really unsafe :)
      + **Shift+123...**: Spawn saved kit number
      + **Shift+Tab**: Change P1 layer
      + **Delete**: Delete selected entity safely
      + **Numbers**: Select item in hotbar
      + **Numbers+Mouse middle**: Add selected entity type to hotbar
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
      + **Ctrl+1**: Reset zoom level to default for all areas
      + **Ctrl+2345**: Zoom to X level width
      + **Shift+IJKL**: Move camera in desired direction
      + **Shift+U**: Reset camera focus and bounds
      + **Mouse 4**: Drag camera around or focus on an entity. Hold to reset.
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
  - **F7**: Entity finder
      + **Alt+Delete**: Delete all selected entities
  - **F8**: Scripts
      + You can load, edit and set options for Lua scripts here
      + **Ctrl+F5**: Reload enabled scripts
  - **F9**: Options
      + Lots of options. **Check the key config for hotkeys for these and other stuff!**
  - **F11**: Hide overlay
  - **F12**: Switch UI style (default menu bar / floating dockable windows)
  - **\~ (or your native Console Key)**: Toggle developer console
    - The console is its own script environment, where you can run some quick oneliners or paste snippets in from the API doc inline examples.
    - **Enter**: Execute
    - **Ctrl+Enter**: New line
    - **"clear/clr/cls"**: Clear history
    - **"reset/reload"**: Clear all callbacks made in the console
    - **"enable/disable"**: Enable/Disable all callbacks made in the console temporarily
    - **"quit"**: Ragequit
    - **"P"**: Short for get_player(1)
    - **"E"**: Short for the selected Entity
    - **"U"**: Short for the selected uid
  - **Ctrl+Shift+K**: Edit keybindings
  - **Ctrl+Shift+S**: Save options, hotkeys and UI colors
  - **Ctrl+Shift+L**: Load options, hotkeys and UI colors
  - **Ctrl+Shift+D**: Detach active tab in tabbed interface
  - **Mouse right**: Detach hovered tab in menu UI
  - **Enter**: Run active tool
  - **Backspace**: (Hold) Peek to the other layer
  - **Escape**: Return focus to game from an input
  - **Q**: Quick start from the intro or menu
  - **Ctrl+Q**: Quick restart from anywhere
  - **Ctrl+C**: Quick return to camp from anywhere

### Scripts
Lua scripting is still buggy and unfinished and the **API might change**, although we will try to deprecate old stuff without breaking anything. There's an [API documentation](https://spelunky-fyi.github.io/overlunky/), [internal details](https://github.com/spelunky-fyi/overlunky/tree/main/docs) and [examples](https://github.com/spelunky-fyi/overlunky/tree/main/examples) to get you started. The examples should be considered [Unlicensed](https://unlicense.org/), so please DO NOT credit the author when copying them. Scripts are loaded from `Spelunky 2/Overlunky/Scripts` by default, but you can change this in the ini. Optionally you can also load *main.lua* files from `Mods/Packs`. Check the [Playlunky documentation](https://github.com/spelunky-fyi/Playlunky/wiki#script-mods) on how to run your scripts as PL mods. Set the [command line switch](#command-line-switches) `--console` to see script messages in the console.

**Scripts don't work online either!** While technically they can work, and there's some work being done on this, yours truly has little interest in making any online features.

## Troubleshooting
  - If you're running **Linux**
    + Check the [command line switches](#command-line-switches) to mitigate issues that come with injecting.
    + The game and OL should run fine with Wine 7 if you install DXVK (and the Steam emulator?). Without DXVK the game colors are probably messed up, and Overlunky might mess them up even more. These are probably Wine bugs and not Overlunky bugs.
    + Also works with **Proton Experimental** or maybe Proton 7 using the [command line switch](#command-line-switches) `--launch_game` for a non-Steam game shortcut. Just make sure the game and the OL shortcut are set to use the same Proton version under compatibility.
    + Modlunky2 should work just fine under Wine and Proton too, if you add it as a non-Steam game.
    + You're still mostly on your own here, this is a Windows 10 program for a Windows 10 game.
  - If your game **crashes** when launching Overlunky or it just closes and you **don't see the overlay** in game:
    + Try all the different launching methods from [command line ](#command-line-switches) or in Modlunky2.
    + Make sure you are running the latest version of each. We don't support old game versions and sometimes not the too recent ones either. The latest somewhat tested version can be found at the top if this document.
    + Make sure your antivirus is not blocking it. **This is very likely if you're using one.** Overlunky is very hacky and acts like malware. It isn't, but you can audit the source code and build it yourself if you want.
    + Try deleting `Spelunky 2/overlunky.ini` or `Spelunky 2/imgui.ini`.
    + Try the WHIP build, it might have already fixed the problem.
  - If your game **crashes** when actually trying to use the tools:
    + See previous section.
    + Some entities just crash the game, try to avoid them next time.
    + Some script mods just crash the game too. Complain to the author of the mod.
  - If you have problems with **Modlunky2 or Playlunky**:
    + While we try to keep all the modding tools compatible, some things may break sometimes.
    + Because PL and OL are using the same scripting API, one might inadvertently undo the change you did in the other. Don't expect perfection when using both.
  - If you're **missing** some overlay **windows** or tabs:
    + Hit F12 to change the window mode.
    + Delete `Spelunky 2/imgui.ini` to reset tool window positions that might be outside the screen region for whatever reason.
    + The tools can be moved outside the game window. Maybe they're on a different monitor for some reason?
    + Maybe the overlay is just really tiny or collapsed? You can resize it. Just try your magic with the mouse.
  - If you mess up your **keyboard shortcuts** or UI:
    + You can reset keyboard shortcuts in the options.
    + Delete `Spelunky 2/overlunky.ini` and `Spelunky 2/imgui.ini` to reset everything.
  - If stuff just don't work and you're running **Windows 7** or a machine not designed to play games
    + You're on your own here. We don't test on or make this for ancient software/hardware.
  - If you have problems with **OBS**
    + Use *Game Capture* and disable *Allow Transparency* if Overlunky looks weird/black.
    + Try toggling the *Capture third-party overlays* option in *Game Capture*. It's possible to hide OL if running the game through Steam.
    + If you have lag or crashes, check previous question.
  - If you **don't have scripts** in the Scripts tab
    + You didn't put them in the right place. [Follow](#installation-and-usage) the [instructions](#scripts), use Modlunky2 or change the script directory in the options.
  - If some of the keyboard shortcuts don't work
    + Make sure your keyboard is not [ghosting](https://www.microsoft.com/applied-sciences/projects/anti-ghosting-demo) the key combo.
    + Make sure Windows is not using the keys for [something stupid](https://superuser.com/questions/1367843/how-to-disable-ctrlshift-keyboard-layout-switch-for-the-same-input-language-i).
  - If it still doesn't work
    + Stop by #s2-modding-help on the [community Discord](https://discord.gg/spelunky-community) or [submit an issue](https://github.com/spelunky-fyi/overlunky/issues)
    + We want to know your OS version, are you using the game on Steam, what you have tried, what tools you are using, how you are using them and all the versions. These can be found in the terminal output or in-game overlays. ("Latest" or WHIP is not a version, but 7f0bc94 or 0.6.1 is.)


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
The binaries will be in `build/bin/Release/`. You can also try the scripts in `.vscode` with VSCode.

## Command line switches

```
--launch_game [path]    launch ../Spel2.exe, path/Spel2.exe, or a specific exe, and load OL with Detours
--oldflip               launch the game with -oldflip, may improve performance with external windows
--console               keep console open to debug scripts etc
--inject                use the old injection method instead of Detours with --launch_game
--info_dump             output a bunch of game data to 'Spelunky 2/game_data'
--update                reset AutoUpdate setting and update launcher and DLL to the latest WHIP build
--update_launcher       update launcher to the latest WHIP build
--help                  show this helpful help
--version               show version information
```

Without arguments the launcher will try to find a running instance of Spel2.exe and inject to it. `--launch_game` can be used with Steam, also on Linux, by adding `Overlunky.exe` as a non-Steam game and adding it to **launch options under properties**. If Overlunky is installed in the default location under `Spelunky 2/Overlunky`, a simple `--launch_game` should work. If you want to use **Overlunky with Playlunky** on Steam, you can use launch `playlunky_launcher.exe` with the `--overlunky` launch option.
