meta.name = "Save/Load"
meta.version = "WIP"
meta.description = "Saves and loads a number."
meta.author = "Malacath"

num_saved = 0

set_callback(function(save_ctx)
    save_ctx:save(tostring(num_saved))
    message("Saved "..tostring(num_saved))
    num_saved = num_saved + 1
end, ON.SAVE)

set_callback(function(load_ctx)
    local load_data = load_ctx:load()
    if load_data ~= "" then
        num_saved = tonumber(load_data)
        message("Loaded "..tostring(num_saved))
    end
end, ON.LOAD)

set_callback(function()
    message("Could've saved but didn't, because I didn't take an arg...")
end, ON.SAVE)
