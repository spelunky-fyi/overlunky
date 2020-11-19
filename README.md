# Overlunky

An overlay for Spelunky 2 to help you with modding, exploring the depths of the game and practicing with tools like spawning arbitrary items, warping to levels and teleporting made by the cool people from the Spelunky Community Discord.

## Features
Current features and their keyboard shortcuts:
  - **F-keys**: Collapse/Show the window
  - **F1**: Search and spawn entities where you're standing
      + **Enter**: Spawn entity
      + **Ctrl+Arrows**: Move spawn coordinates around you
      + **Ctrl+Enter**: Teleport to coordinates
      + Enter multiple numeric IDs like `526 560 570` to spawn them all at once. Useful for making a kit you can just paste in.
      + Note: Item numbers can change between versions, so use the search to find the latest ones or check `Spelunky 2/entities.txt`
  - **F2**: Spawn a warp door to any world, level and theme
      + **Enter**: Spawn door. Set the appropriate theme theme too or you will get weird results.
  - **F11**: Hide overlay completely for screenshots and faking being a god gamer
  - Spawn or teleport to mouse cursor (see options)

## Usage 
**[Download the latest release](https://github.com/spelunky-fyi/overlunky/releases/latest)** and extract anywhere. Run the program, leave it running and then start your game, or the other way around! If you don't get the screen overlay and are using an antivirus there's a high chance this program will get blocked because of the way it works. You need to whitelist it to use it.

## Current issues
  - If you're playing with keyboard, the keys you type in the overlay still activate in the game too.
  - If you have problems with the entity list not populating, try starting the injector after reaching the camp. (Restart your game first!)
  - Mouse click coordinates are only translated correctly with the default zoom, e.g. they're not accurate inside shops.

## Disclaimer
You are strongly discouraged from using any modding tools in your actual online Steam installation as to prevent unlocking achievements, corrupting your savefile and cheating in the leaderboards. You should make a copy of your game somewhere else and install [Mr. Goldbergs Steam Emulator](https://gitlab.com/Mr_Goldberg/goldberg_emulator/-/releases) in the game directory. (TL;DR: Copy the steam_api64.dll from the zip to the offline game directory and create steam_appid.txt with the text `418530` in it.) Also block the modded installation in your firewall. If you break anything using this tool you get to keep both pieces. Do not report modding related bugs to Blitworks.
