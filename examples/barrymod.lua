meta.name = 'Barrymod'
meta.version = '1.0'
meta.description = 'Restarts the current level on death or manually like nothing happened. '
meta.author = 'Dregu'

register_option_button('load', 'Quickload', function()
    load_state(1)
end)

register_option_button('save', 'Quicksave', function()
    save_state(1)
end)

set_callback(function()
    save_state(1)
    for _, p in pairs(players) do
        set_on_player_instagib(p.uid, function(e) restart = true end)
    end
end, ON.LEVEL)

set_callback(function()
    if restart then
        restart = nil
        load_state(1)
    end
end, ON.POST_UPDATE)

set_callback(function()
    local tile = get_grid_entity_at(6, 121, LAYER.FRONT)
    if tile then
        get_entity(tile):remove()
    end

    tile = get_grid_entity_at(6, 120, LAYER.FRONT)
    if tile then
        get_entity(tile):decorate_internal()
    end
end, ON.TRANSITION)
