-- set some visual settings needed by your mod
-- doing this here will reapply these after visiting the options, which would reset them to real values

set_callback(function()
    if state.screen_next == SCREEN.LEVEL then
        -- use the secret tiny hud size
        set_setting(GAME_SETTING.HUD_SIZE, 3)
        -- force opaque textboxes
        set_setting(GAME_SETTING.TEXTBOX_OPACITY, 0)
    end
end, ON.PRE_LOAD_SCREEN)
