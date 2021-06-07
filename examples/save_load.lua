meta.name = "Save/Load"
meta.version = "WIP"
meta.description = "Saves and loads a number."
meta.author = "Malacath"

local num_saved = 0
local num_levels = 0

set_callback(function()
    message("Num Levels "..tostring(num_levels))
    num_levels = num_levels + 1
end, ON.LEVEL)

set_callback(function(save_ctx)
    local save_data = {
        num_saved = num_saved,
        num_levels = num_levels,
    }
    local save_data_str = json.encode(save_data)
    save_ctx:save(save_data_str)
    message("Saved "..inspect(save_data))
    num_saved = num_saved + 1
end, ON.SAVE)

set_callback(function(load_ctx)
    local load_data_str = load_ctx:load()
    if load_data_str ~= "" then
        local load_data_str = json.decode(load_data_str)
        num_saved = load_data_str.num_saved
        num_levels = load_data_str.num_levels
        message("Loaded "..inspect(load_data_str))
    end
end, ON.LOAD)

set_callback(function()
    message("Could've saved but didn't, because I didn't take an arg...")
end, ON.SAVE)
