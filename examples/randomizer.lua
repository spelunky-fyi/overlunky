-- spelunky 2 randomizer
-- this is actually pretty good!
-- endles random level themes until you've killed all the bosses
-- random items in pots
-- random actually useful items in crates
-- random coffins
-- random player stats
-- weird seismic activity

register_option_int("min_levels", "Min normal levels before boss", 3, 1, 100)
register_option_int("max_levels", "Max normal levels before boss", 8, 1, 100)
register_option_int("bosses", "Bosses to defeat to trigger ending", 5, 1, 5)
register_option_int("max_health", "Max starting health", 20, 4, 99)
register_option_int("max_bombs", "Max starting bombs", 20, 4, 99)
register_option_int("max_ropes", "Max starting ropes", 20, 4, 99)

items = {220,221,222,223,224,225,227,228,229,230,231,232,233,234,237,238,239,240,242,243,244,245,246,247,248,249,250,251,252,253,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,283,284,286,287,288,289,290,295,296,297,298,300,301,302,303,304,305,306,307,308,309,310,311,312,317,318,319,320,321,322,323,326,327,328,331,332,333,334,336,337,338,339,340,341,347,348,356,357,358,365,366,371,372,373,374,377,395,396,399,400,401,402,409,416,422,428,429,435,436,439,440,442,444,448,453,456,457,462,469,475,476,477,478,479,480,481,482,490,491,492,493,494,495,496,497,498,499,500,501,506,507,508,509,510,511,512,513,514,515,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,536,557,558,560,563,565,567,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,592,593,596,604,610,630,631,884,885,886,887,888,890}
tools = {374,422,509,510,511,512,513,514,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,534,536,557,558,560,563,565,567,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,585}
theme = {1,2,3,5,6,7,8,9,10,11}
bosses = {THEME.OLMEC, THEME.ABZU, THEME.DUAT, THEME.TIAMAT, THEME.HUNDUN}
bosses_left = {}
world = {1,2,2,3,4,4,5,6,7,7,4,4,4,6,7,7,1}

bosses_left = {}
normal_levels = 0
nextworld = 1
nextlevel = 1
nexttheme = 1
you_win = 0
boss_level = 0

function init_run()
  bosses_left = {table.unpack(bosses)}
  normal_levels = 0
  nexttheme = theme[math.random(#theme)]
  nextlevel = math.random(4)
  if nexttheme == THEME.COSMIC_OCEAN then
    nextlevel = math.random(5,97)
  end
  nextworld = world[nexttheme]
  you_win = 0
  boss_level = 0
  players = get_players()
  for i,player in ipairs(players) do
    player.health = math.random(4, options.max_health)
    player.inventory_ptr.bombs = math.random(4, options.max_bombs)
    player.inventory_ptr.ropes = math.random(4, options.max_ropes)
  end
end
init_run()

function remove_boss(boss)
  for k,v in ipairs(bosses_left) do
    if v == boss then
      table.remove(bosses_left, k)
      message("Boss defeated! Bosses remaining: "..tostring(#bosses_left-(#bosses-options.bosses)))
    end
  end
end

function random_level()
  if math.random(120) == 1 then
    nexttheme = THEME.EGGPLANT_WORLD
    nextlevel = 2
  elseif #bosses_left > #bosses-options.bosses and normal_levels >= options.min_levels and math.random(options.max_levels) < normal_levels then
    boss_level = 1
    normal_levels = 0
    nexttheme = bosses_left[math.random(#bosses_left)]
    nextlevel = 4
    if nexttheme == THEME.OLMEC then
      nextlevel = 1
    end
  elseif #bosses_left <= #bosses-options.bosses and normal_levels >= options.min_levels and math.random(options.max_levels) < normal_levels then
    boss_level = 0
    nexttheme = THEME.COSMIC_OCEAN
    nextlevel = 98
    nextworld = 7
    you_win = 1
    message("Lets get out of here!")
    return
  else
    boss_level = 0
    normal_levels = normal_levels+1
    nexttheme = theme[math.random(#theme)]
    nextlevel = math.random(4)
    if nexttheme == THEME.COSMIC_OCEAN then
      nextlevel = math.random(5, 97)
    end
  end
  nextworld = world[nexttheme]
end

function random_doors()
  if state.level < 98 then
    print("Going to level "..tostring(state.level_count)..": "..tostring(nextworld).."-"..tostring(nextlevel).." theme "..tostring(nexttheme))
    doors = get_entities_by_type(23)
    for i,v in ipairs(doors) do
      print("Setting door "..tostring(v))
      set_door_target(v, nextworld, nextlevel, nexttheme)
      if boss_level == 1 then
        x, y, layer = get_position(v)
        spawn_entity(ENT_TYPE.ITEM_LITWALLTORCH, x-0.7, y+0.5, layer, 0, 0)
        spawn_entity(ENT_TYPE.ITEM_LITWALLTORCH, x+0.7, y+0.5, layer, 0, 0)
      end
    end
  end
end

function on_level()
  message("Level "..tostring(state.level_count+1)..". Bosses remaining: "..(#bosses_left-(#bosses-options.bosses)))
  if state.level_count == 0 then
    init_run()
  end

  -- randomize pots
  pots = get_entities_by_type(ENT_TYPE.ITEM_POT)
  for i,v in ipairs(pots) do
    item = items[math.random(#items)]
    print("Setting pot "..tostring(v).." to "..tostring(item))
    set_contents(v, item)
  end

  -- randomize crates
  crates = get_entities_by_type(ENT_TYPE.ITEM_CRATE)
  for i,v in ipairs(crates) do
    item = tools[math.random(#tools)]
    print("Setting crate "..tostring(v).." to "..tostring(item))
    set_contents(v, item)
  end

  -- randomize coffins
  coffins = get_entities_by_type(ENT_TYPE.ITEM_COFFIN)
  for i,v in ipairs(coffins) do
    item = math.random(194, 216)
    if item == 214 then
      item = 215
    end
    print("Setting coffin "..tostring(v).." to "..tostring(item))
    set_contents(v, item)
  end

  -- check for dead bosses
  if (state.journal_flags & (1 << 15)) > 0 then
    remove_boss(THEME.ABZU)
  end
  if (state.journal_flags & (1 << 16)) > 0 then
    remove_boss(THEME.DUAT)
  end

  -- give turkey
  if state.level_count > 0 then
    players = get_players()
    for i,player in ipairs(players) do
      print("Give turkey")
      x, y, layer = get_position(player.uid)
      spawn_entity(ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY, x, y+1, layer, 0, 0)
    end
  end

  -- randomize next level
  random_level()
  random_doors()
end

function on_frame()
  if you_win == 0 then
    state.world_next = nextworld
    state.level_next = nextlevel
    state.theme_next = nexttheme
  end

  -- check for dead olmec
  if state.world == 3 and state.level == 1 then
    olmecs = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_OLMEC)
    if olmecs[1] then
      flags = get_entity_flags2(olmecs[1])
      if (flags & (1 << 10)) > 0 then -- this olmec is dead?
        remove_boss(THEME.OLMEC)
      end
    end
  end

  -- check for dead tiamat
  if state.world == 6 and state.level == 4 then
    tiamats = get_entities_by_type(ENT_TYPE.MONS_TIAMAT)
    if tiamats[1] then
      flags = get_entity_flags(tiamats[1])
      if (flags & (1 << 28)) > 0 then -- this tiamat is dead?
        remove_boss(THEME.TIAMAT)
      end
    end
  end

end

function on_transition()
  -- check for dead hundun
  if state.world == 7 and state.level == 4 then
    remove_boss(THEME.HUNDUN)
  end
end

function on_death()
  message("I eventually died in level "..tostring(state.level_count+1)..". Bosses remaining: "..(#bosses_left-(#bosses-options.bosses)))
end

setinterval(function()
  rocks = get_entities_by_type(ENT_TYPE.ITEM_ROCK)
  for i,v in ipairs(rocks) do
    print("Throwing rock "..tostring(v))
    entity = get_entity(v)
    entity.velocityx = math.random()*2-1
    entity.velocityy = math.random()*2-1
  end
end, 5000)

message("Spelunky 2 Door+Pot Randomizer WIP initialized!")
