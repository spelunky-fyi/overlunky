-- Sets the right layer when using the vanilla tile code if waddler is still happy,
-- otherwise spawns the floor to the left of this tile.
-- Manually spawning FLOOR_STORAGE pre-tilecode doesn't seem to work as expected,
-- so we destroy it post-tilecode.
set_post_tile_code_callback(function(x, y, layer)
    if not test_flag(state.quest_flags, 10) then
        -- Just set the layer and let the vanilla tilecode handle the floor
        set_storage_layer(layer)
    else
        local floor = get_entity(get_grid_entity_at(x, y, layer))
        if floor then
            floor:destroy()
        end
        if get_grid_entity_at(x - 1, y, layer) ~= -1 then
            local left = get_entity(get_grid_entity_at(x - 1, y, layer))
            spawn_grid_entity(left.type.id, x, y, layer)
        end
    end
end, "storage_floor")

-- This fixes a bug in the game that breaks storage on transition.
-- The old storage_uid is not cleared after every level for some reason.
set_callback(function()
    state.storage_uid = -1
end, ON.TRANSITION)

-- Having a waddler is completely optional for storage,
-- but this makes a nice waddler room if he still likes you.
define_tile_code("waddler")
set_pre_tile_code_callback(function(x, y, layer)
    if not test_flag(state.quest_flags, 10) then
        local uid = spawn_roomowner(ENT_TYPE.MONS_STORAGEGUY, x + 0.5, y, layer, ROOM_TEMPLATE.WADDLER)
        set_on_kill(uid, function()
            -- Disable current level storage if you kill waddler
            state.storage_uid = -1
        end)
    end
    return true
end, "waddler")
