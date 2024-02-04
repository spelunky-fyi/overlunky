meta.name = 'Barrymod'
meta.version = '1.0'
meta.description = 'Creates checkpoints and restarts the current level on death like nothing happened.'
meta.author = 'Dregu'

register_option_bool('save_early', 'Multiverse of Madness Mode',
    'Rerolls the level generation on death,\nbut keeps quest state and inventory.',
    false)

register_option_callback('buttons', nil, function(ctx)
    if ctx:win_button('Quick Save') then save_state(1) end
    ctx:win_inline()
    if ctx:win_button('Quick Load') then load_state(1) end
end)

set_callback(function()
    if not options.save_early then
        save_state(1)
    end
    for _, p in pairs(players) do
        set_on_player_instagib(p.uid, function(e)
            -- can't load_state directly here, cause we're still in the middle of an update
            restart = true
        end)
    end
end, ON.LEVEL)

set_callback(function()
    if options.save_early then
        -- for whatever prng related reason, loading a save created at this point will reroll the level rng, which is a neat I guess
        save_state(1)
    end
end, ON.PRE_LEVEL_GENERATION)

set_callback(function()
    if restart then
        restart = nil
        -- load the save state we made earlier, after updates to not mess with the running state
        load_state(1)
    end
end, ON.POST_UPDATE)

set_callback(function()
    if state.screen ~= SCREEN.TRANSITION then return end
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
end, ON.POST_LEVEL_GENERATION)
