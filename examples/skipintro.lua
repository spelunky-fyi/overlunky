meta.name = 'Skip intro'
meta.description = 'Jump straight to 1-1 from intro or title screen.'
meta.author = 'Dregu'
meta.version = '1.1'

--[[ Overlunky Pro Tip: Put
autorun_scripts = ["skipintro.lua"]
in your overlunky.ini for this to make any sense...

Quick start with Q is baked into Overlunky now, but maybe you can use this with PL. ]]

local function skip()
  -- Jump to 1-1 from any screen before menu
  if state.screen >= 0 and state.screen < 4 then

    -- init single player:
    state.items.player_select[1].activated = true
    -- get character from last played game
    state.items.player_select[1].character = ENT_TYPE.CHAR_ANA_SPELUNKY + savegame.players[1]
    state.items.player_select[1].texture = TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0 + savegame.players[1]
    state.items.player_count = 1

    -- init state:
    state.screen_next = 12
    state.world_start = 1
    state.level_start = 1
    state.theme_start = 1
    state.world_next = 1
    state.level_next = 1
    state.theme_next = 1
    state.quest_flags = 1
    state.loading = 1

    -- fix for character select screen:
    state.screen_character_select.available_mine_entrances = 4;
    -- you can also set it to SCREEN.LEVEL to skip the camp
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

set_callback(function()
  skip()
  -- run only once, otherwise we can't go back to main menu
  clear_callback()
end, ON.SCREEN)

skip()
