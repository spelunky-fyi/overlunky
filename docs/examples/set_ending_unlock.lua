-- change character unlocked by endings to pilot
set_ending_unlock(ENT_TYPE.CHAR_PILOT)

-- change texture of the actual savior in endings to pilot
set_callback(function()
    set_post_entity_spawn(function(ent)
        if state.screen == SCREEN.WIN then
            ent:set_texture(TEXTURE.DATA_TEXTURES_CHAR_PINK_0)
        end
        clear_callback()
    end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.PLAYER)
end, ON.WIN)
