meta.name = "Jelly mode"
meta.version = "1.0"
meta.description = "Spawns a megajellyfish on every entrance, and a clover for good luck! You can hide in the backlayer, but it will be waiting!"
meta.author = "Dregu"

current_layer = 0

-- spawn megajelly using absolute coordinates
set_callback(function()
  x, y, layer = get_position(players[1].uid)
  spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
  -- jellies go dumb when you go to the backlayer, so we'll just spawn a new one when you come back
  set_interval(function()
    if #players < 1 then return end
    x, y, layer = get_position(players[1].uid)
    if layer ~= current_layer then
      if layer == 0 then
        spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
      else
        jellys = get_entities_by_type(ENT_TYPE.MONS_MEGAJELLYFISH)
        for i,v in ipairs(jellys) do
          move_entity(v, 0, 0, 0, -1) -- apparently you can't kill these
        end
      end
      current_layer = layer
    end
  end, 10)
end, ON.LEVEL)

-- spawn clover using player-relative coordinates
set_callback(function()
  spawn(ENT_TYPE.ITEM_PICKUP_CLOVER, 0, 1, LAYER.PLAYER1, 0, 0)
end, ON.LEVEL)
