meta.name = "Teleport predictor and SaveStates"
meta.author = "Dregu"
meta.version = "1.0"

states = {}

function clear_states()
    for _, v in pairs(states) do
        v:clear()
    end
    states = {}
    if tp then
        tp:clear()
        tp = nil
    end
end

set_callback(function(ctx)
    ctx:window("Advanced SaveStates", 0, 0, 0, 0, true, function(ctx)
        if ctx:win_button("New save") then
            states[#states + 1] = SaveState:new()
        end
        ctx:win_inline()
        if ctx:win_button("Clear saves") then
            clear_states()
        end
        ctx:win_separator()
        for i, v in pairs(states) do
            if ctx:win_button(F "Load #{i}: frame {v:get_state().time_level}") then
                v:load()
            end
        end
    end)

    -- teleport predictor/simulator using save states
    -- simulates accurate teleport destination when pressing Z or X while paused
    if not players[1] then return end
    if (get_io().keypressed(KEY.Z) or get_io().keypressed(KEY.X)) and pause:paused() then
        if tp then
            tp:save()
        else
            tp = SaveState:new()
        end
        box_frame = state.time_level

        players[1]:topmost_mount():set_post_update_state_machine(function(e)
            clear_callback()
            box = get_hitbox(e.uid)
            box_color = 0xcc33ff33
        end)
        set_callback(function()
            clear_callback()
            if not players[1] then box_color = 0xcc3333ff end
            if tp then tp:load() end
        end, ON.POST_UPDATE)
        pause:frame_advance()
    end
    if box and box_frame == state.time_level then
        ctx:draw_rect_filled(screen_aabb(box), 0, box_color)
    end
end, ON.GUIFRAME)

set_callback(function()
    if #states == 0 then
        states[1] = SaveState:new()
    end
    box = nil
    box_frame = nil
end, ON.LEVEL)

set_callback(clear_states, ON.PRE_LEVEL_DESTRUCTION)

set_callback(function(slot, loading)
    if slot > 0 then
        print(F "Loading save slot {slot}...")
    else
        print("Loading custom save slot...")
    end
    rewind = state.time_level - loading.time_level
    if rewind < 0 then
        print(F "Not forwarding {-rewind} frames, that would break spacetime!")
        return true
    else
        print(F "Rewinding {rewind} frames...")
    end
end, ON.PRE_LOAD_STATE)

set_callback(function(slot, loaded)
    print(F "Rewinded {rewind} frames!")
    rewind = nil
end, ON.POST_LOAD_STATE)
