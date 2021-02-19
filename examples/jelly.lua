meta.name = "Jelly mode"
meta.version = "1.0"
meta.description = "Spawns a megajellyfish on every entrance, and a clover for good luck!"
meta.author = "Dregu"

-- spawn megajelly using absolute coordinates
set_callback(function()
  x, y, layer = get_position(players[1].uid)
  spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
end, ON.LEVEL)
-- spawn clover using player-relative coordinates
set_callback(function()
  spawn(ENT_TYPE.ITEM_PICKUP_CLOVER, 0, 1, LAYER.PLAYER1, 0, 0)
end, ON.LEVEL)
