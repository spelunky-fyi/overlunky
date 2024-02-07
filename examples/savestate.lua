meta.name = "SaveState example"
meta.author = "Dregu"
meta.version = "1.0"

states = {}
do_load = nil
do_save = nil

function clear_states()
    for _, v in pairs(states) do
        v:clear()
    end
    states = {}
end

set_callback(function(ctx)
    ctx:window("Advanced SaveStates", 0, 0, 0, 0, true, function(ctx)
        if ctx:win_button("New save") then
            do_save = true
        end
        ctx:win_inline()
        if ctx:win_button("Clear saves") then
            clear_states()
        end
        ctx:win_separator()
        for i, v in pairs(states) do
            if ctx:win_button(F "Load #{i}: frame {v:get().time_level}") then
                do_load = v
            end
        end
    end)
end, ON.GUIFRAME)

set_callback(function()
    if #states == 0 then
        states[1] = SaveState:new()
    end
end, ON.LEVEL)

set_callback(function()
    if do_load then
        do_load:load()
        do_load = nil
    elseif do_save then
        states[#states + 1] = SaveState:new()
        do_save = nil
    end
end, ON.POST_UPDATE)

set_callback(clear_states, ON.PRE_LEVEL_DESTRUCTION)
