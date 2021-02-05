-- jelly mode
-- spawns a jellyfish above every entrance
function on_level()
  print("Entered level "..tostring(state.world).."-"..tostring(state.level))
  players = get_players()
  x, y, layer = get_position(players[1].uid)
  spawn_entity(ENT_TYPE.MONS_MEGAJELLYFISH, x, y+3, layer, 0, 0)
end
