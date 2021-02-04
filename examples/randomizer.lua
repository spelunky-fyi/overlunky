-- spelunky 2 randomizer
-- this is actually pretty good!
-- random level themes with a boss rush at the end
-- random items in pots
-- random coffins

-- pro tip: just change this to make it shorter
max = 100

items = {220,221,222,223,224,225,227,228,229,230,231,232,233,234,237,238,239,240,242,243,244,245,246,247,248,249,250,251,252,253,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,283,284,286,287,288,289,290,295,296,297,298,300,301,302,303,304,305,306,307,308,309,310,311,312,317,318,319,320,321,322,323,326,327,328,331,332,333,334,336,337,338,339,340,341,347,348,356,357,358,365,366,371,372,373,374,377,395,396,399,400,401,402,409,416,422,428,429,435,436,439,440,442,444,448,453,456,457,462,469,475,476,477,478,479,480,481,482,490,491,492,493,494,495,496,497,498,499,500,501,506,507,508,509,510,511,512,513,514,515,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,536,557,558,560,563,565,567,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,592,593,596,604,610,630,631,884,885,886,887,888,890}
theme = {1,2,3,5,6,7,8,9,10,11}
bosses = {THEME.OLMEC, THEME.ABZU, THEME.DUAT, THEME.TIAMAT, THEME.HUNDUN}
world = {1,2,2,3,4,4,5,6,7,8,4,4,4,6,7,7,1}
nexttheme = theme[math.random(#theme)]
nextlevel = math.random(4)
if nexttheme == THEME.COSMIC_OCEAN then
  nextlevel = math.random(5,97)
end
nextworld = world[nexttheme]
function on_level()
  message("Started level "..tostring(state.level_count+1).."/"..tostring(max)..".")
  pots = get_entities_by_type(ENT_TYPE.ITEM_POT);
  for i,v in ipairs(pots) do
    item = items[math.random(#items)]
    print("Setting pot "..tostring(v).." to "..tostring(item))
    set_contents(v, item)
  end
  coffins = get_entities_by_type(ENT_TYPE.ITEM_COFFIN);
  for i,v in ipairs(coffins) do
    item = math.random(194, 216)
    if item == 214 then
      item = 215
    end
    print("Setting coffin "..tostring(v).." to "..tostring(item))
    set_contents(v, item)
  end
  if math.random(100) == 1 and state.level_count < max-5 then
    nexttheme = THEME.EGGPLANT_WORLD
    nextlevel = 2
  elseif math.random(100) < max/20 and state.level_count > max/3 and state.level_count < max-5 then
    nexttheme = bosses[math.random(#bosses)]
    nextlevel = 4
    if nexttheme == THEME.OLMEC then
      nextlevel = 1
    end
  elseif state.level_count < max-5 then
    nexttheme = theme[math.random(#theme)]
    nextlevel = math.random(4)
    if nexttheme == THEME.COSMIC_OCEAN then
      nextlevel = math.random(5, 97)
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
function on_frame()
  state.world_next = nextworld;
  state.level_next = nextlevel;
  state.theme_next = nexttheme;
end
function on_transition()
  message("Level "..tostring(state.level_count).."/"..tostring(max).." completed!")
end
function on_death()
  message("Died in level "..tostring(state.level_count+1).."/"..tostring(max)..".")
end
message("Spelunky 2 Door+Pot Randomizer initialized!")
