-- jelly mode
-- spawns a jellyfish above every entrance
function on_level()
  print("Entered level "..tostring(state.world).."-"..tostring(state.level))
  spawn_entity(312, 0, 3, false, 0, 0, false)
end
