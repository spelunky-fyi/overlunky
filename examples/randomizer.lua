meta.name = "Randomizer"
meta.version = "0.1"
meta.description = "Random exit doors, pot/crate/coffin contents, player stats, boss order and other cool stuff!"
meta.author = "Dregu"

register_option_int("min_levels", "Min normal levels before boss", 3, 1, 100)
register_option_int("max_levels", "Max normal levels before boss", 8, 1, 100)
register_option_int("bosses", "Bosses to defeat to trigger ending", 5, 1, 5)
register_option_int("max_health", "Max starting health", 20, 4, 99)
register_option_int("max_bombs", "Max starting bombs", 20, 4, 99)
register_option_int("max_ropes", "Max starting ropes", 20, 4, 99)
register_option_bool("lock_exit", "Lock exit in Dwelling", true)
--register_option_bool("seismic", "Weird seismic activity", true) -- this crashy, api bad

items = {220,221,222,223,224,225,227,228,229,230,231,232,233,234,237,238,239,240,242,243,244,245,246,247,248,249,250,251,252,253,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,283,284,286,287,288,289,290,295,296,297,298,300,301,302,303,304,305,306,307,308,309,310,311,312,317,318,319,320,321,322,323,331,332,333,334,336,337,338,339,340,341,347,348,356,357,358,365,366,371,372,373,374,377,395,396,399,400,401,402,409,416,422,428,429,435,436,439,440,442,444,448,453,456,457,462,469,475,476,477,478,479,480,481,482,490,491,492,493,494,495,496,497,498,499,500,501,506,507,508,509,510,511,512,513,514,515,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,536,557,558,560,563,565,567,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,592,593,596,604,610,630,631,884,885,886,887,888,890}
tools = {374,422,509,510,511,512,513,514,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,534,536,557,558,560,563,565,567,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,585}
theme = {1,2,3,5,6,7,8,9,10,11}
bosses = {THEME.OLMEC, THEME.ABZU, THEME.DUAT, THEME.TIAMAT, THEME.HUNDUN}
bosses_left = {}
world = {1,2,2,3,4,4,5,6,7,8,4,4,4,6,7,7,1}
critters = {}
critters[THEME.DWELLING] = ENT_TYPE.MONS_CRITTERDUNGBEETLE
critters[THEME.JUNGLE] = ENT_TYPE.MONS_CRITTERBUTTERFLY
critters[THEME.VOLCANA] = ENT_TYPE.MONS_CRITTERSNAIL
critters[THEME.TIDE_POOL] = ENT_TYPE.MONS_CRITTERCRAB
critters[THEME.TEMPLE] = ENT_TYPE.MONS_CRITTERLOCUST
critters[THEME.ICE_CAVES] = ENT_TYPE.MONS_CRITTERPENGUIN
critters[THEME.NEO_BABYLON] = ENT_TYPE.MONS_CRITTERDRONE
critters[THEME.SUNKEN_CITY] = ENT_TYPE.MONS_CRITTERSLIME
critters[THEME.CITY_OF_GOLD] = ENT_TYPE.MONS_CRITTERLOCUST

bosses_left = {}
normal_levels = 0
nextworld = 1
nextlevel = 1
nexttheme = 1
you_win = false
boss_level = false
exit_locked = false
hidden_block = 0
hidden_x = 0
hidden_y = 0

function setflag(v, b) return v | (1 << b-1) end
function clrflag(v, b) return v & ~(1 << b-1) end
function testflag(v, b) return (v & (1 << b-1)) > 0 end

function init_run()
  bosses_left = {table.unpack(bosses)}
  normal_levels = 0
  nexttheme = theme[math.random(#theme)]
  nextlevel = math.random(4)
  if nexttheme == THEME.COSMIC_OCEAN then
    nextlevel = math.random(5,97)
  elseif nexttheme == THEME.ICE_CAVES then
    nextlevel = 1
  end
  nextworld = world[nexttheme]
  you_win = false
  boss_level = false
  hidden_block = 0
  exit_locked = false
  for i,player in ipairs(players) do
    player.health = math.random(4, options.max_health)
    player.inventory.bombs = math.random(4, options.max_bombs)
    player.inventory.ropes = math.random(4, options.max_ropes)
  end
  --[[ debug
  nextworld = 1
  nextlevel = math.random(4)
  nexttheme = 1]]--
end
init_run()

function remove_boss(boss)
  for k,v in ipairs(bosses_left) do
    if v == boss then
      table.remove(bosses_left, k)
      toast("Boss defeated!\nBosses remaining: "..tostring(#bosses_left-(#bosses-options.bosses)))
    end
  end
end

function random_level()
  -- rare eggplant world
  if math.random(120) == 1 then
    nexttheme = THEME.EGGPLANT_WORLD
    nextlevel = 2

  -- pick a boss level from the remaining
  elseif #bosses_left > #bosses-options.bosses and normal_levels >= options.min_levels and math.random(options.max_levels) < normal_levels then
    boss_level = true
    normal_levels = 0
    nexttheme = bosses_left[math.random(#bosses_left)]
    nextlevel = 4
    if nexttheme == THEME.OLMEC then
      nextlevel = 1
    end

  -- all bosses killed, go to 7-98
  elseif #bosses_left <= #bosses-options.bosses and normal_levels >= options.min_levels and math.random(options.max_levels) < normal_levels and (state.theme == THEME.DWELLING or state.theme == THEME.VOLCANA or state.theme == THEME.TEMPLE or state.theme == THEME.SUNKEN_CITY or state.theme == THEME.CITY_OF_GOLD) then
    boss_level = false
    nexttheme = THEME.COSMIC_OCEAN
    nextlevel = 98
    nextworld = 42
    you_win = true
    toast("Lets get out of here!")
    return

  -- pick a regular level
  else
    boss_level = false
    normal_levels = normal_levels+1
    nexttheme = theme[math.random(#theme)]
    nextlevel = math.random(4)
    if nexttheme == THEME.COSMIC_OCEAN then
      nextlevel = math.random(5, 97)
    elseif nexttheme == THEME.ICE_CAVES then
      nextlevel = 1
    end
  end
  nextworld = world[nexttheme]
  if nexttheme == THEME.COSMIC_OCEAN then
    nextworld = 42
  end
end

function lock_exit()
  if not options.lock_exit then return end
  if state.level_count == 0 then return end
  if state.theme ~= THEME.DWELLING then return end

  for i,player in ipairs(players) do
    skeletonkey = entity_has_item_type(player.uid, ENT_TYPE.ITEM_POWERUP_SKELETON_KEY)
    if skeletonkey then return end
  end

  -- pick a random ground block
  floors = get_entities_by(ENT_TYPE.FLOOR_GENERIC, 0, 0) -- type, mask, layer
  if #floors == 0 then return end
  hidden_block = floors[math.random(math.ceil(#floors/4), #floors)]
  x, y, l = get_position(hidden_block)
  hidden_x = x
  hidden_y = y
  exit_locked = true

  toast("Oh no the exit is blocked!\nWe need a skeleton key!")

  -- put fake blackmarket door there to find with udjat
  spawn_entity(ENT_TYPE.LOGICAL_BLACKMARKET_DOOR, x, y, 0, 0, 0)

  -- give some tools to help find it
  x, y, layer = get_position(players[1].uid)
  goldkeys = get_entities_by_type(ENT_TYPE.ITEM_LOCKEDCHEST_KEY)
  if #goldkeys == 0 then
    spawn_entity(ENT_TYPE.ITEM_PICKUP_UDJATEYE, x+1, y, layer, 0, 0)
  end
  spawn_entity(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, x-1, y, layer, 0, 0)
  spawn_entity(ENT_TYPE.ITEM_MATTOCK, x, y, layer, 0, 0)
  doors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
  for i,v in ipairs(doors) do
    x, y, l = get_position(v)
    spawn_entity(ENT_TYPE.ITEM_PICKUP_CLOVER, x+1, y, layer, 0, 0)
  end

end

function random_doors()
  if state.level < 98 then
    print("Going to level "..tostring(state.level_count)..": "..tostring(nextworld).."-"..tostring(nextlevel).." theme "..tostring(nexttheme))
    message("DEBUG: Going to "..tostring(nextworld).."-"..tostring(nextlevel).." theme "..tostring(nexttheme))
    doors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
    for i,v in ipairs(doors) do
      print("Setting door "..tostring(v))
      set_door_target(v, nextworld, nextlevel, nexttheme)
      x, y, layer = get_position(v)
      if boss_level then
        spawn_entity(ENT_TYPE.ITEM_LITWALLTORCH, x-0.7, y+0.5, layer, 0, 0)
        spawn_entity(ENT_TYPE.ITEM_LITWALLTORCH, x+0.7, y+0.5, layer, 0, 0)
      elseif state.theme ~= THEME.COSMIC_OCEAN and nexttheme == THEME.COSMIC_OCEAN then
        spawn_entity(ENT_TYPE.ITEM_FLOATING_ORB, x, y+0.3, layer, 0, 0)
      elseif critters[nexttheme] ~= nil then
        spawn_entity(critters[nexttheme], x-1, y, layer, 0, 0)
        spawn_entity(critters[nexttheme], x+1, y, layer, 0, 0)
      end
    end
  end
end

function throw_rock(id)
  if options.seismic then
    entity = get_entity(id)
    if entity == nil then return end
    if entity.type.id ~= ENT_TYPE.ITEM_ROCK then return end
    entity.velocityx = math.random()-0.5
    entity.velocityy = math.random()-0.5
  end
  set_timeout(function() throw_rock(id) end, math.random(5*60, 10*60))
end

function on_level()
  state.world = world[state.theme]
  if state.level_count == 0 then
    init_run()
  end

  exit_locked = false

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

  -- invisible traps?!
  traps = get_entities_by_type(ENT_TYPE.FLOOR_SPRING_TRAP, ENT_TYPE.ITEM_LANDMINE)
  for i,v in ipairs(traps) do
    spring = get_entity(v)
    spring.color.a = math.random()*0.5
  end
  if #traps > 0 then
    toast("Oh no the traps are invisible!?")
  end

  -- check for dead bosses
  if (state.journal_flags & (1 << 15)) > 0 then
    remove_boss(THEME.ABZU)
  end
  if (state.journal_flags & (1 << 16)) > 0 then
    remove_boss(THEME.DUAT)
  end

  -- give turkey
  x, y, layer = get_position(players[1].uid)
  if state.level_count > 0 then
    spawn_entity(ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY, x, y+1, layer, 0, 0)
  end

  -- randomize next level
  random_level()

  -- lock exit and hide key somewhere
  lock_exit()

  -- spawn duat skip door
  if state.theme == THEME.DUAT then
    spawn_door(17, 106, 0, nextworld, nextlevel, nexttheme)
    spawn_entity(ENT_TYPE.BG_DOOR_BACK_LAYER, 17, 106, 0, 0, 0)
  end

  -- kill all the pets, sorry
  if exit_locked then
    pets = get_entities_by_type(ENT_TYPE.MONS_PET_DOG, ENT_TYPE.MONS_PET_CAT, ENT_TYPE.MONS_PET_HAMSTER)
    for i,pet in ipairs(pets) do
      move_entity(pet, 0, 0, 0, 0)
      print("Killed pet, sorry!")
    end
  end

  -- throw some rocks
  --[[rocks = get_entities_by_type(ENT_TYPE.ITEM_ROCK)
  for i,v in ipairs(rocks) do
    set_timeout(throw_rock(v), math.random(5*60, 10*60))
  end]]--

  -- set door targets
  random_doors()

end

function on_guiframe()
  -- force level even when engine isn't running
  if not you_win then
    state.world_next = nextworld
    state.level_next = nextlevel
    state.theme_next = nexttheme
  end
end

function on_frame()
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
      if testflag(flags, 29) then -- this tiamat is dead
        remove_boss(THEME.TIAMAT)
      end
    end
  end

  -- check for dead hundun
  if state.world == 7 and state.level == 4 then
    hunduns = get_entities_by_type(ENT_TYPE.MONS_HUNDUN)
    if hunduns[1] then
      ai = get_entity_ai_state(hunduns[1])
      if ai == 4 then -- this hundun is just chillin on the floor
        remove_boss(THEME.HUNDUN)
      end
    end
  end

  -- unlock exit if hidden block found
  if exit_locked then
    for i,player in ipairs(players) do
      skeletonkey = entity_has_item_type(player.uid, ENT_TYPE.ITEM_POWERUP_SKELETON_KEY) -- not PICKUP!
      if skeletonkey then
        exit_locked = false
      end
    end
    block = get_entity(hidden_block)
    if block == nil then
      exit_locked = false
      spawn_entity(ENT_TYPE.ITEM_PICKUP_SKELETON_KEY, hidden_x, hidden_y, 0, 0, 0)
      hidden_block = 0
      hidden_x = 0
      hidden_y = 0
    else
      doors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
      for i,v in ipairs(doors) do
        x, y, l = get_position(v)
        lock_door_at(x, y)
      end
    end
  else
    doors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
    for i,v in ipairs(doors) do
      x, y, l = get_position(v)
      unlock_door_at(x, y)
    end
  end
end

function on_transition()
  exit_locked = false
  toast("Level "..tostring(state.level_count).." completed!")

end

function on_death()
  message("I eventually died in level "..tostring(state.level_count+1)..". Bosses remaining: "..(#bosses_left-(#bosses-options.bosses)))
  exit_locked = false
end

message("Spelunky 2 Door+Pot Randomizer WIP initialized!")
