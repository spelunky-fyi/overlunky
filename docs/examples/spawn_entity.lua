-- spawn megajelly on top of player using absolute coordinates on level start
set_callback(function()
    local x, y, layer = get_position(players[1].uid)
    spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
end, ON.LEVEL)

-- spawn clover next to player using player-relative coordinates
set_callback(function()
    spawn(ENT_TYPE.ITEM_PICKUP_CLOVER, 1, 0, LAYER.PLAYER1, 0, 0)
end, ON.LEVEL)
