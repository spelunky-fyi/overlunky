-- door randomizer
-- plays random level themes and ends with a boss rush
-- this one changes the actual target of exit doors
max = 20
theme = {1,2,3,5,6,7,8,9,10,11}
world = {1,2,2,3,4,4,5,6,7,8,4,4,4,6,7,7,1}
nextworld = 1
nextlevel = 1
nexttheme = 1
function on_level()
  if math.random(100) == 1 and state.level_count < max-5 then
    nexttheme = THEME.EGGPLANT_WORLD
    nextlevel = 2
  elseif state.level_count < max-5 then
    nexttheme = theme[math.random(#theme)]
    nextlevel = math.random(4)
    if nexttheme == THEME.COSMIC_OCEAN then
      nextlevel = nextlevel+50
    end
  elseif state.level_count < max-4 then
    nexttheme = THEME.OLMEC
    nextlevel = 1
  elseif state.level_count < max-3 then
    nexttheme = THEME.ABZU
    nextlevel = 4
  elseif state.level_count < max-2 then
    nexttheme = THEME.DUAT
    nextlevel = 4
  elseif state.level_count < max-1 then
    nexttheme = THEME.TIAMAT
    nextlevel = 4
  elseif state.level_count < max then
    nexttheme = THEME.HUNDUN
    nextlevel = 4
  else
    nexttheme = THEME.COSMIC_OCEAN
    nextlevel = 98
  end
  nextworld = world[nexttheme]
  if state.level < 98 then
    print("Going to level "..tostring(state.level_count).."/"..tostring(max)..": "..tostring(nextworld).."-"..tostring(nextlevel).." theme "..tostring(nexttheme))
    doors = get_entities_by_type(23);
    for i,v in ipairs(doors) do
      print("Setting door "..tostring(v))
      set_target(v, nextworld, nextlevel, nexttheme)
    end
  end
end
function on_transition()
  state.world_next = nextworld;
  state.level_next = nextlevel;
  state.theme_next = nexttheme;
end
