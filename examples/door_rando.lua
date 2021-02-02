-- door randomizer
-- plays random level themes and ends with a boss rush
max = 100
theme = {1,2,3,5,6,7,8,9,10,11}
function on_transition()
  print("Transition to "..tostring(state.world_next).."-"..tostring(state.level_next))
  state.world_next = max
  state.level_next = (state.level_count-1)%4+1
  if math.random(100) == 1 and state.level_count < max-5 then
    state.theme_next = THEME.EGGPLANT_WORLD
  elseif state.level_count < max-5 then
    state.theme_next = theme[math.random(#theme)]
  elseif state.level_count < max-4 then
    state.theme_next = THEME.OLMEC
  elseif state.level_count < max-3 then
    state.theme_next = THEME.ABZU
  elseif state.level_count < max-2 then
    state.theme_next = THEME.DUAT
  elseif state.level_count < max-1 then
    state.theme_next = THEME.TIAMAT
  elseif state.level_count < max then
    state.theme_next = THEME.HUNDUN
  else
    state.level_next = 98
    state.theme_next = THEME.COSMIC_OCEAN
  end
  print("Going to level "..tostring(state.level_count).."/"..tostring(max)..": "..tostring(state.world_next).."-"..tostring(state.level_next).." theme "..tostring(state.theme_next))
end
function on_level()
  state.world = max
  state.level = state.level_count+1
end
