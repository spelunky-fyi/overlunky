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
    state.items.player_select[1].activated = true
    state.items.player_select[1].character = ENT_TYPE.CHAR_ANA_SPELUNKY + savegame.players[1]
    state.items.player_select[1].texture = TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0 + savegame.players[1]
    state.items.player_count = 1
    state.screen_next = 12
    state.world_start = 1
    state.level_start = 1
    state.theme_start = 1
    state.world_next = 1
    state.level_next = 1
    state.theme_next = 1
    state.quest_flags = 1
    state.loading = 1
  end
end

set_callback(function()
  skip()
end, ON.SCREEN)

skip()
