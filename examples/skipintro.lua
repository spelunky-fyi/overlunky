meta.name = 'Skip intro'
meta.description = 'Jump straight to 1-1 from intro or title screen.'
meta.author = 'Dregu'
meta.version = '1.2'

--[[ Overlunky Pro Tip: Put
autorun_scripts = ["skipintro.lua"]
in your overlunky.ini for this to make any sense...

Quick start with "Q" is baked into Overlunky now, but maybe you can use this with Playlunky. ]]

local function skip()
  -- Jump to 1-1 from any screen before menu
  if state.screen >= SCREEN.LOGO and state.screen < SCREEN.MENU then

    -- init single player:
    state.items.player_select[1].activated = true
    state.items.player_count = 1
    -- get character from last played game:
    state.items.player_select[1].character = ENT_TYPE.CHAR_ANA_SPELUNKY + savegame.players[1]
    state.items.player_select[1].texture = TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0 + savegame.players[1]

    -- init state:
    state.screen_next = SCREEN.LEVEL
    state.world_start = 1
    state.level_start = 1
    state.theme_start = THEME.DWELLING
    state.world_next = 1
    state.level_next = 1
    state.theme_next = THEME.DWELLING
    state.quest_flags = QUEST_FLAG.RESET
    state.loading = 1

    -- fix for character select screen (when going directly from death screen):
    state.screen_character_select.available_mine_entrances = 4;
    -- can also be set to SCREEN.LEVEL
    -- then going to character select (from death screen) and choosing the character will bring you back to the game directly
    state.screen_character_select.next_screen_to_load = SCREEN.CAMP;

    -- disable the [title screen -> menu] animation:
    game_manager.screen_menu.loaded_once = true

    -- set controller to first input device for player one and menu
    -- id 0 usually is keyboard, without this, you will need to press jump button first to register controller
    if game_manager.game_props.input_index[1] == -1 then
        game_manager.game_props.input_index[1] = 0
    end
    if game_manager.game_props.input_index[5] == -1 then
        game_manager.game_props.input_index[5] = 0
    end
  end
end

set_callback(skip, ON.SCREEN)

-- only for overlunky loading the script from the UI
skip()
