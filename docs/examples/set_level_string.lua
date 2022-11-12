-- set the level string shown in hud, journal and game over
-- also change the one used in transitions for consistency
set_callback(function()
    if state.screen_next == SCREEN.LEVEL then
        local level_str = "test" .. tostring(state.level_count)
        set_level_string(level_str)
        change_string(hash_to_stringid(0xda7c0c5b), F"{level_str} COMPLETED!")
    end
end, ON.PRE_LOAD_SCREEN)
