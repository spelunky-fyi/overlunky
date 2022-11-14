register_option_bool("bomb_bag", "BombBag", "Spawn bomb bag at the start of every level", false)

set_callback(function()
    if options.bomb_bag then
        -- Spawn the bomb bag at player location thanks to the LAYER.PLAYER1
        spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_PICKUP_BOMBBAG, 0, 0, LAYER.PLAYER1)
    end
end, ON.LEVEL)
