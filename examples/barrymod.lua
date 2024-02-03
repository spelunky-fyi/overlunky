meta.name = 'Barrymod'
meta.version = '1.0'
meta.description = 'Restarts the current level on death or manually like nothing happened. '
meta.author = 'Dregu'

register_option_bool('alt', 'Multiverse of Madness Mode',
    'Rerolls the level generation on death,\nbut keeps quest state and inventory.',
    false)

register_option_callback('buttons', nil, function(ctx)
    if ctx:win_button('Quick Save') then save_state(1) end
    ctx:win_inline()
    if ctx:win_button('Quick Load') then load_state(1) end
end)

function save_early()
    return options.alt and state.theme ~= THEME.OLMEC -- typical olmec crashes with this?
end

set_callback(function()
    if not save_early() then
        save_state(1)
    end
    for _, p in pairs(players) do
        set_on_player_instagib(p.uid, function(e) restart = true end)
    end
end, ON.LEVEL)

set_callback(function()
    if save_early() then
        save_state(1)
    end
end, ON.PRE_LEVEL_GENERATION)

set_callback(function()
    if restart then
        restart = nil
        load_state(1)
    end
end, ON.POST_UPDATE)

set_callback(function()
    local tile = get_entity(get_grid_entity_at(6, 121, LAYER.FRONT))
    if tile then
        tile:remove()
        tile = get_entity(get_grid_entity_at(6, 120, LAYER.FRONT))
        if tile then
            tile:decorate_internal()
        end
    else
        tile = get_entity(spawn_grid_entity(ENT_TYPE.FLOOR_GENERIC, 6, 121, LAYER.FRONT))
        tile:decorate_internal()
    end
end, ON.TRANSITION)
