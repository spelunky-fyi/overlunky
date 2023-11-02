meta = {
    name = "Supplemental TAS controls",
    description = [[Examples how to override and recreate core Overlunky features.

This script works in both Overlunky and Playlunky. When Overlunky is loaded, it will disable all matching OL keybinds to make sure they don't interfere with the substitutes implemented in the mod.

- (Ctrl+Space) toggle FADE+PRE_UPDATE pause
- (Space) frame advance
- (PgUp/PgDn/Home/End) engine frametime control
- (Ctrl+PgUp/PgDn/Home/End) zoom control
- (Ins) toggle level start auto pause
- (Del) toggle skip fades

]],
    author = "Dregu",
    version = "1.1"
}

control = {
    type = PAUSE.FADE,
    paused = false,
    skip = false,
    level = false,
    fade = false,
    frametime = 1/60,
    zoom = 13.5,
}

override_keys = {
    KEY.INSERT,
    KEY.DELETE,
    KEY.HOME,
    KEY.END,
    KEY.PGUP,
    KEY.PGDN,
    KEY.SPACE,
}

set_global_interval(function()
    if KEY and get_bucket and get_bucket().overlunky then
        local ol = get_bucket().overlunky
        for _, v in pairs(override_keys) do
            ol.ignore_keycodes:insert(nil, v)
            ol.ignore_keycodes:insert(nil, KEY.OL_MOD_CTRL | v)
        end
        return false
    end
end, 60)


set_callback(function()
    if state.loading > 0 and control.fade then
        set_frametime(0)
    elseif state.loading > 0 then
        set_frametime(1/60)
    else
        set_frametime(control.frametime)
    end
    set_frametime_unfocused(0)
    if state.logic.ouroboros and get_zoom_level() > 13.5 then
        zoom(13.5)
    elseif not state.logic.ouroboros then
        zoom(control.zoom)
    end
    if control.fade then
        state.fadevalue = 0
        state.fadeout = 0
        state.fadein = 0
        state.loading_black_screen_timer = 0
    end
    if control.level and state.loading == 3 and ((not control.fade and state.fadeout == 1) or control.fade) then
        control.paused = true
        control.skip = false
        state.fadevalue = 0
        state.loading = 0
    end
    if control.paused and control.skip then
        control.skip = false
        state.pause = clr_mask(state.pause, control.type)
        return false
    elseif control.paused then
        state.pause = set_mask(state.pause, control.type)
    end
    return control.paused and not test_flag(state.pause, PAUSE.MENU)
end, ON.PRE_UPDATE)

set_callback(function(ctx)
    local iio = get_io()
    if iio.keyctrl then
        if iio.keypressed(0x20, false) then --SPACE
            control.paused = not control.paused
            if control.paused then
                state.pause = set_mask(state.pause, control.type)
            else
                state.pause = clr_mask(state.pause, control.type)
            end
        elseif iio.keypressed(0x21, true) then --PGUP
            control.zoom = control.zoom - 1
            if control.zoom < 1 then control.zoom = 1 end
            zoom(control.zoom)
        elseif iio.keypressed(0x22, true) then --PGDN
            control.zoom = control.zoom + 1
            zoom(control.zoom)
        elseif iio.keypressed(0x24, false) then --HOME
            control.zoom = 13.5
            zoom(control.zoom)
        elseif iio.keypressed(0x23, false) then --END
            control.zoom = 0
            zoom(control.zoom)
        end
    elseif iio.keypressed(0x20, true) and control.paused then --SPACE
        control.skip = true
    elseif iio.keypressed(0x21, true) then --PGUP
        control.frametime = control.frametime / 1.2
        set_frametime(control.frametime)
    elseif iio.keypressed(0x22, true) then --PGDN
        control.frametime = control.frametime * 1.2
        if control.frametime > 1 then control.frametime = 1 end
        set_frametime(control.frametime)
    elseif iio.keypressed(0x24, false) then --HOME
        control.frametime = 1/60
        set_frametime()
    elseif iio.keypressed(0x23, false) then --END
        control.frametime = 1/12
        set_frametime(control.frametime)
    elseif iio.keypressed(0x2D, false) then --INS
        control.level = not control.level
    elseif iio.keypressed(0x2E, false) then --DEL
        control.fade = not control.fade
    end
end, ON.GUIFRAME)

register_option_callback("tas_controls", nil, function(ctx)
    local old_paused = control.paused
    control.paused = ctx:win_check("Paused", control.paused)
    if control.paused ~= old_paused then
        if control.paused then
            state.pause = set_mask(state.pause, control.type)
        else
            state.pause = clr_mask(state.pause, control.type)
        end
    end
    control.skip = ctx:win_check("Frame advance", control.skip)
    control.level = ctx:win_check("Pause on level start", control.level)
    control.fade = ctx:win_check("Skip fades", control.fade)
    control.type = ctx:win_input_int("Pause type", control.type)
    local fps = ctx:win_drag_float("FPS", control.frametime > 0 and 1 / control.frametime or 0, 0, 240)
    ctx:win_inline() if ctx:win_button("Reset##ResetFPS") then fps = 60 end
    control.frametime = fps > 0 and 1 / fps or 0
    control.zoom = ctx:win_drag_float("Zoom", control.zoom, 1, 100)
    ctx:win_inline() if ctx:win_button("Reset##ResetZoom") then control.zoom = 13.5 end
end)
