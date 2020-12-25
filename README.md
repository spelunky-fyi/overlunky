# Overlunky

An overlay for Spelunky 2 to help you with modding, exploring the depths of the game and practicing with tools like spawning arbitrary items, warping to levels and teleporting made by the cool people from the Spelunky Community Discord.

[![Overlunky](https://img.shields.io/github/v/release/spelunky-fyi/overlunky?label=Overlunky)](https://github.com/spelunky-fyi/overlunky/releases/latest) is compatible with ![Spelunky](https://img.shields.io/badge/Spelunky2-1.20.0j-green)

## Disclaimer
You are strongly discouraged from using any modding tools in your actual online Steam installation as to prevent unlocking achievements, corrupting your savefile and cheating in the leaderboards. You should make a copy of your game somewhere else and install [Mr. Goldbergs Steam Emulator](https://gitlab.com/Mr_Goldberg/goldberg_emulator/-/releases) in the game directory. (TL;DR: Copy the steam_api64.dll from the zip to the offline game directory and create steam_appid.txt with the text `418530` in it.) Also block the modded installation in your firewall. If you break anything using this tool you get to keep both pieces. Do not report modding related bugs to BlitWorks.

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
      + **Enter**: Spawn door. Set the appropriate theme too or you will get weird results.
  - **F3**: Camera controls
      + **Ctrl+Comma/Period**: Change the zoom level
      + Lock horizontal scrolling with the *N*x buttons for *N* wide levels
      + Note: It only sets the *default* zoom level, which isn't used in camp or shops.
  - **F11**: Hide overlay
  - **Ctrl+G**: Toggle peaceful mode
  - **Ctrl+M**: Toggle click events
  - **Ctrl+Numbers**: Set zoom level
  - **RAlt+Arrows**: Teleport to direction
  - Spawn or teleport to mouse cursor (see options)
  - Peaceful mode (*nothing* takes damage from anything)

## Usage 
**[Download the latest release](https://github.com/spelunky-fyi/overlunky/releases/latest)** and extract anywhere. Run the program, leave it running and then start your game, or the other way around! Overlunky doesn't do any permanent changes to your game, it only exists when you run it.

## Troubleshooting
  - If your game crashes when launching Overlunky or it just closes and you don't get the overlay in game:
    + Make sure you are running the latest version of each. We don't support old game versions.
    + Make sure your antivirus is not blocking it.

## Current issues
  - **Your antivirus will probably block this** because of the way it hooks the game, you have to add an exception if you are using AV.
  - If you're playing with keyboard, the keys you type in the overlay still activate in the game too.
  - Mouse clicks are only translated correctly with the default zoom (the one set with F3), e.g. they're not accurate inside shops or when the game otherwise decides to zoom in.
  - Mouse clicks are also translated wrong if playing fullscreen on non 16:9 resolution with black bars.
  - If you teleport while ledge grabbing or touching anything with physics, that thing is coming with you. Including - but not limited to - mounts, push blocks, ladders and colossal space ship heads.
