meta.name = "Jelly mode"
meta.version = "1.0"
meta.description = "Spawns a megajellyfish on every entrance, have fun!"
meta.author = "Dregu"

set_callback(function()
  message("Entered level "..tostring(state.world).."-"..tostring(state.level))
  x, y, layer = get_position(players[1].uid)
  spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
end, ON.LEVEL)
