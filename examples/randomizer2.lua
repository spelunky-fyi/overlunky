meta.name = "Randomizer Two"
meta.description = [[Fair, balanced, beginner friendly... These are not words I would use to describe The Randomizer. Fun though? Abso-hecking-lutely.
    
Second incarnation of The Randomizer with new API shenannigans. Most familiar things from 1.2 are still there, but better! Progression is changed though, shops are random, level gen is crazy, chain item stuff, multiple endings, secrets... I can't possibly test all of this so fingers crossed it doesn't crash a lot.]]
meta.version = "2.0d"
meta.author = "Dregu"

local function get_chance(min, max)
    if max == 0 then return 0 end
    if min == 0 then min = 0.001 end
    if min > max then min, max = max, min end
    min = math.floor(1/(min/100))
    max = math.floor(1/(max/100))
    return prng:random(max, min)
    --return prng:random_int(min, max, PRNG_CLASS.LEVEL_GEN)
end

local function pick(from, ignore)
    local item = -1
    for i=1,10 do
        ----math.randomseed(read_prng()[8]+i)
        item = from[prng:random(#from)]
        --item = from[prng:random_index(#from, PRNG_CLASS.LEVEL_GEN)]
        if item ~= ignore then
            return item
        end
    end
    return item
end

local function get_money()
    local money = 0
    for i,p in ipairs(players) do
        money = money + p.inventory.money + p.inventory.collected_money_total
    end
    money = money + state.money_shop_total
    return money
end

local function join(a, b)
    local result = {table.unpack(a)}
    table.move(b, 1, #b, #result + 1, result)
    return result
end

local function has(arr, item)
    for i, v in pairs(arr) do
        if v == item then
            return true
        end
    end
    return false
end

local function shuffle(tbl)
    for i = #tbl, 2, -1 do
        local j = prng:random(i)
        tbl[i], tbl[j] = tbl[j], tbl[i]
    end
end

local function get_ushabti_frame()
    local x, y
    if prng:random() > options.ushabti_chance/100 or state.correct_ushabti == nil then
        x = prng:random(0,9)
        y = prng:random(0,9)
    else
        x = state.correct_ushabti % 10
        y = math.floor(state.correct_ushabti / 10)
    end
    return x + y * 12
end

local theme_name = {}
for i,v in pairs(THEME) do
    theme_name[v] = i
end

local level_map = {}

local function map_level()
    local xmin, ymin, xmax, ymax = get_bounds()
    xmin = math.ceil(xmin)
    xmax = math.ceil(xmax)
    ymin = math.ceil(ymin)
    ymax = math.ceil(ymax)
    for x=xmin,xmax,1 do
        level_map[x] = {}
        for y=ymin,ymax,-1 do
            local mask = 0
            local ents = get_entities_at(0, MASK.PLAYER | MASK.MOUNT | MASK.MONSTER | MASK.ITEM | MASK.ACTIVEFLOOR | MASK.FLOOR | MASK.WATER | MASK.LAVA, x, y, LAYER.FRONT, 0.5)
            for i,v in ipairs(ents) do
                local ent = get_entity(v)
                if ent and (mask & ent.type.search_flags) == 0 then
                    mask = mask + ent.type.search_flags
                end
            end
            level_map[x][y] = mask
        end
    end
end

local function map(x, y)
    if #level_map == 0 then
        map_level()
    end
    if level_map[x] and level_map[x][y] then
        return level_map[x][y]
    end
    return 0
end

--[[TILECODES]]
register_option_bool("tilecode", "Random floor styles", true)
local floor_types = {ENT_TYPE.FLOOR_GENERIC, ENT_TYPE.FLOOR_JUNGLE, ENT_TYPE.FLOORSTYLED_MINEWOOD, ENT_TYPE.FLOORSTYLED_STONE, ENT_TYPE.FLOORSTYLED_TEMPLE, ENT_TYPE.FLOORSTYLED_PAGODA, ENT_TYPE.FLOORSTYLED_BABYLON, ENT_TYPE.FLOORSTYLED_SUNKEN, ENT_TYPE.FLOORSTYLED_BEEHIVE, ENT_TYPE.FLOORSTYLED_VLAD, ENT_TYPE.FLOORSTYLED_MOTHERSHIP, ENT_TYPE.FLOORSTYLED_DUAT, ENT_TYPE.FLOORSTYLED_PALACE, ENT_TYPE.FLOORSTYLED_GUTS, ENT_TYPE.FLOOR_SURFACE}
local floor_tilecodes = {floor=-1, minewood_floor=-1, stone_floor=-1, pagoda_floor=-1, babylon_floor=-1, beehive_floor=-1, cog_floor=-1, duat_floor=-1, sunken_floor=-1, icefloor=-1, palace_floor=-1, temple_floor=-1, vlad_floor=-1, shop_wall=-1, pen_floor=-1, shop_sign=-1}

--[[TRAPS]]
register_option_float("trap_max", "Max trap chance", 4, 0, 100)
register_option_float("trap_min", "Min trap chance", 2, 0, 100)

local traps_ceiling = {ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_FACTORY_GENERATOR, ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_FACTORY_GENERATOR, ENT_TYPE.FLOOR_SHOPKEEPER_GENERATOR}
local traps_floor = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_TIMED_FORCEFIELD, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP, ENT_TYPE.ACTIVEFLOOR_ELEVATOR}
local traps_wall = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_LASER_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP}
local traps_flip = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_LASER_TRAP}
local traps_generic = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
local traps_item = {ENT_TYPE.FLOOR_SPRING_TRAP, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SNAP_TRAP, ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
local traps_totem = {ENT_TYPE.FLOOR_TOTEM_TRAP, ENT_TYPE.FLOOR_LION_TRAP}
local valid_floors = join(floor_types, {ENT_TYPE.FLOOR_ICE})

local function trap_ceiling_spawn(x, y, l)
    local item = pick(traps_ceiling)
    if #get_entities_by_type(ENT_TYPE.FLOOR_SHOPKEEPER_GENERATOR) > 0 then
        item = pick(traps_ceiling, ENT_TYPE.FLOOR_SHOPKEEPER_GENERATOR)
    end
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kill_entity(floor)
    end
    spawn_grid_entity(item, x, y, l)
end
local function trap_ceiling_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    if has({THEME.CITY_OF_GOLD, THEME.ICE_CAVES, THEME.TIAMAT, THEME.OLMEC}, state.theme) then
        return false
    end
    if (map(x, y+1) & MASK.LAVA) > 0 then return false end
    local rx, ry = get_room_index(x, y)
    if y == state.level_gen.spawn_y and (ry >= state.level_gen.spawn_room_y and ry <= state.level_gen.spawn_room_y-1) then return false end
    local floor = get_grid_entity_at(x, y, l)
    local below = get_grid_entity_at(x, y-1, l)
    local below2 = get_grid_entity_at(x, y-2, l)
    if floor ~= -1 and below == -1 and below2 == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local trap_ceiling_chance = define_procedural_spawn("trap_ceiling", trap_ceiling_spawn, trap_ceiling_valid)

local function trap_floor_spawn(x, y, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kill_entity(floor)
    end
    spawn_grid_entity(pick(traps_floor), x, y, l)
end
local function trap_floor_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    if (map(x, y+1) & MASK.LAVA) > 0 then return false end
    local floor = get_grid_entity_at(x, y, l)
    local above = get_grid_entity_at(x, y+1, l)
    if floor ~= -1 and above == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local trap_floor_chance = define_procedural_spawn("trap_floor", trap_floor_spawn, trap_floor_valid)

local function trap_wall_spawn(x, y, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kill_entity(floor)
    end
    local id = pick(traps_wall)
    local ent = spawn_grid_entity(id, x, y, l)
    local left = get_grid_entity_at(x-1, y, l)
    local right = get_grid_entity_at(x+1, y, l)
    if has(traps_flip, id) then
        if left == -1 and right == -1 then
            --math.randomseed(read_prng()[5])
            if prng:random() < 0.5 then
                flip_entity(ent)
            end
        elseif left == -1 then
            flip_entity(ent)
        end
    end
end
local function trap_wall_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    local rx, ry = get_room_index(x, y)
    if y == state.level_gen.spawn_y and (rx >= state.level_gen.spawn_room_x-1 and rx <= state.level_gen.spawn_room_x+1) then return false end
    local floor = get_grid_entity_at(x, y, l)
    local left = get_grid_entity_at(x-1, y, l)
    local right = get_grid_entity_at(x+1, y, l)
    if floor ~= -1 and (left == -1 or right == -1) then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local trap_wall_chance = define_procedural_spawn("trap_wall", trap_wall_spawn, trap_wall_valid)

local function trap_generic_spawn(x, y, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kill_entity(floor)
    end
    spawn_grid_entity(pick(traps_generic), x, y, l)
end
local function trap_generic_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    if (map(x, y+1) & MASK.LAVA) > 0 then return false end
    local rx, ry = get_room_index(x, y)
    if x == state.level_gen.spawn_x and (ry >= state.level_gen.spawn_room_y and ry <= state.level_gen.spawn_room_y-1) then return false end
    local floor = get_grid_entity_at(x, y, l)
    local above = get_grid_entity_at(x, y+1, l)
    if floor ~= -1 then
        if above ~= -1 then
            above = get_entity(above)
            if above.type.id == ENT_TYPE.FLOOR_ALTAR then
                return false
            end
        end
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local trap_generic_chance = define_procedural_spawn("trap_generic", trap_generic_spawn, trap_generic_valid)

local function trap_item_spawn(x, y, l)
    local id = pick(traps_item)
    if id == ENT_TYPE.ITEM_SNAP_TRAP and state.theme == THEME.DUAT then return end
    spawn_entity_snapped_to_floor(id, x, y, l)
end
local function trap_item_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    if (map(x, y) & MASK.LAVA) > 0 then return false end
    local floor = get_grid_entity_at(x, y-1, l)
    local air = get_grid_entity_at(x, y, l)
    if floor ~= -1 and air == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local trap_item_chance = define_procedural_spawn("trap_item", trap_item_spawn, trap_item_valid)

local function trap_totem_spawn(x, y, l)
    local id = pick(traps_totem)
    local uid = get_grid_entity_at(x, y, l)
    if uid ~= -1 then
        local floor = get_entity(uid)
        if has(valid_floors, floor.type.id) then
            spawn_entity_over(id, spawn_entity_over(id, uid, 0, 1), 0, 1)
        end
    end
end
local function trap_totem_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    local floor = get_grid_entity_at(x, y, l)
    local box = AABB:new()
    box.left = x-1
    box.right = x+1
    box.top = y+3
    box.bottom = y+1
    local air = get_entities_overlapping_hitbox(0, MASK.FLOOR | MASK.ACTIVEFLOOR, box, l)
    local left = get_grid_entity_at(x-1, y, l)
    local right = get_grid_entity_at(x+1, y, l)
    if floor ~= -1 and #air == 0 and left ~= -1 and right ~= -1 then
        floor = get_entity(floor)
        left = get_entity(left)
        right = get_entity(right)
        return has(valid_floors, floor.type.id) and has(valid_floors, left.type.id) and has(valid_floors, right.type.id)
    end
    return false
end
local trap_totem_chance = define_procedural_spawn("trap_totem", trap_totem_spawn, trap_totem_valid)

local function trap_frog_spawn(x, y, l)
    local id = ENT_TYPE.FLOOR_BIGSPEAR_TRAP
    local uid = get_grid_entity_at(x, y, l)
    if uid ~= -1 then
        kill_entity(uid)
    end
    uid = get_grid_entity_at(x+1, y, l)
    if uid ~= -1 then
        kill_entity(uid)
    end
    spawn_grid_entity(id, x, y, l)
    spawn_grid_entity(id, x+1, y, l)
end
local function trap_frog_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    local floor = get_grid_entity_at(x, y, l)
    local box = AABB:new()
    box.left = x-1
    box.right = x+2
    box.top = y+1
    box.bottom = y
    local air = get_entities_overlapping_hitbox(0, MASK.FLOOR | MASK.ACTIVEFLOOR, box, l)
    local left = get_grid_entity_at(x-1, y-1, l)
    local right = get_grid_entity_at(x+2, y-1, l)
    if floor ~= -1 and #air == 0 and left ~= -1 and right ~= -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end

    floor = get_grid_entity_at(x, y, l)
    local floor2 = get_grid_entity_at(x+1, y, l)
    left = get_grid_entity_at(x-1, y, l)
    right = get_grid_entity_at(x+2, y, l)
    if floor ~= -1 and floor2 ~= -1 and (left == -1 or right == -1) then
        floor = get_entity(floor)
        floor2 = get_entity(floor2)
        return has(valid_floors, floor.type.id) and has(valid_floors, floor2.type.id)
    end
    return false
end
local trap_frog_chance = define_procedural_spawn("trap_frog", trap_frog_spawn, trap_frog_valid)

set_callback(function(ctx)
    --math.randomseed(read_prng()[1])
    ctx:set_procedural_spawn_chance(trap_ceiling_chance, math.floor(get_chance(options.trap_min, options.trap_max)*1.5))
    ctx:set_procedural_spawn_chance(trap_floor_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_wall_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_generic_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_item_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_totem_chance, get_chance(options.trap_min, options.trap_max)*2)
    ctx:set_procedural_spawn_chance(trap_frog_chance, get_chance(options.trap_min, options.trap_max))

    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.ARROWTRAP_CHANCE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CRUSHER_TRAP_CHANCE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SPARKTRAP_CHANCE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SPIKE_BALL_CHANCE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.BIGSPEARTRAP_CHANCE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.JUNGLE_SPEAR_TRAP_CHANCE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.EGGSAC_CHANCE, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.STICKYTRAP_CHANCE, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SKULLDROP_CHANCE, get_chance(options.trap_min, options.trap_max))
end, ON.POST_ROOM_GENERATION)

set_callback(function()
    level_map = {}
end, ON.PRE_LEVEL_GENERATION)

set_post_entity_spawn(function(ent)
    if state.theme == THEME.DUAT then
        kill_entity(ent.uid)
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_SNAP_TRAP)

set_callback(function()
    if state.theme ~= THEME.DUAT then return end
    local box = AABB:new()
    box.top = state.level_gen.spawn_y - 2
    box.bottom = state.level_gen.spawn_y - 3
    box.left = state.level_gen.spawn_x - 5
    box.right = state.level_gen.spawn_x + 5
    local floor = get_entities_overlapping_hitbox(0, MASK.FLOOR | MASK.ACTIVEFLOOR, box, LAYER.FRONT)
    for i,v in ipairs(floor) do
        kill_entity(v)
    end
    set_interval(function()
        if #get_entities_by_mask(MASK.LAVA) <= 1180 then
            spawn_liquid(ENT_TYPE.LIQUID_LAVA, state.level_gen.spawn_x, state.level_gen.spawn_y - 2)
        elseif #get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR) == 0 then
            spawn(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR, 17.5, 36, LAYER.FRONT, 0, 0)
            spawn(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR, 2.5, 36, LAYER.FRONT, 0, 0)
            spawn(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR, 32.5, 36, LAYER.FRONT, 0, 0)
        end
    end, 10)
end, ON.LEVEL)

set_callback(function()
    if state.theme ~= THEME.DUAT then return end
    local bones = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_BONEBLOCK)
    local elevator = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR)
    if #elevator > 0 then
        local ex, ey, el = get_position(elevator[1])
        for i,v in ipairs(bones) do
            local x, y, l = get_position(v)
            if y < ey+5 then
                kill_entity(v)
            end
        end
    end
end, ON.FRAME)

--[[ENEMIES]]
register_option_float("enemy_max", "Max enemy chance", 12, 0, 100)
register_option_float("enemy_min", "Min enemy chance", 5, 0, 100)
register_option_float("enemy_curse_chance", "Enemy handicap chance", 5, 0, 100)

local enemies_small = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER,
    ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
    ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_MANTRAP, ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_WITCHDOCTOR,
    ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.MONS_ROBOT,
    ENT_TYPE.MONS_FIREBUG_UNCHAINED,
    ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS,
    ENT_TYPE.MONS_CATMUMMY, ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI,
    ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_ALIEN,
    ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER, ENT_TYPE.MONS_SHOPKEEPERCLONE,
    ENT_TYPE.MONS_OLMITE_HELMET, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED,
    ENT_TYPE.MONS_AMMIT, ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG,
    ENT_TYPE.MONS_JUMPDOG, ENT_TYPE.MONS_LEPRECHAUN, ENT_TYPE.MOUNT_TURKEY,
    ENT_TYPE.MOUNT_ROCKDOG, ENT_TYPE.MOUNT_AXOLOTL}
local enemies_big = {ENT_TYPE.MONS_CAVEMAN_BOSS, ENT_TYPE.MONS_LAVAMANDER, ENT_TYPE.MONS_MUMMY, ENT_TYPE.MONS_ANUBIS,
    ENT_TYPE.MONS_GIANTFISH, ENT_TYPE.MONS_YETIKING, ENT_TYPE.MONS_YETIQUEEN, ENT_TYPE.MONS_ALIENQUEEN,
    ENT_TYPE.MONS_LAMASSU, ENT_TYPE.MONS_QUEENBEE, ENT_TYPE.MONS_GIANTFLY, ENT_TYPE.MONS_CRABMAN,
    ENT_TYPE.MOUNT_MECH}
local enemies_climb = {ENT_TYPE.MONS_FIREBUG, ENT_TYPE.MONS_MONKEY}
local enemies_ceiling = {ENT_TYPE.MONS_BAT, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_BAT, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_BAT, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_VAMPIRE, ENT_TYPE.MONS_VAMPIRE, ENT_TYPE.MONS_VLAD, ENT_TYPE.MONS_HANGSPIDER, ENT_TYPE.MONS_HANGSPIDER}
local enemies_air = {ENT_TYPE.MONS_MOSQUITO, ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_GRUB, ENT_TYPE.MONS_IMP, ENT_TYPE.MONS_UFO, ENT_TYPE.MONS_SCARAB}
local enemies_kingu = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER,
    ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
    ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_TIKIMAN,
    ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN,
    ENT_TYPE.MONS_FIREBUG_UNCHAINED, ENT_TYPE.MONS_IMP, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS,
    ENT_TYPE.MONS_CATMUMMY, ENT_TYPE.MONS_NECROMANCER,
    ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_ALIEN,
    ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER,
    ENT_TYPE.MONS_OLMITE_HELMET, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED,
    ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG}
local random_crap = {ENT_TYPE.ITEM_TV, ENT_TYPE.ITEM_VAULTCHEST, ENT_TYPE.ITEM_PUNISHBALL, ENT_TYPE.ITEM_ROCK}
local olmec_ammo = join(join(random_crap, enemies_small), traps_item)
local tiamat_ammo = {ENT_TYPE.ITEM_ACIDSPIT, ENT_TYPE.ITEM_INKSPIT, ENT_TYPE.ITEM_FLY, ENT_TYPE.ITEM_FIREBALL, ENT_TYPE.ITEM_FREEZERAYSHOT, ENT_TYPE.ITEM_LAMASSU_LASER_SHOT}
local crab_items = {ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK, ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.ITEM_CHEST, ENT_TYPE.ITEM_VAULTCHEST, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_PRESENT, ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_EGGPLANT, ENT_TYPE.ITEM_IDOL, ENT_TYPE.ITEM_KEY, ENT_TYPE.ITEM_SNAP_TRAP, ENT_TYPE.ITEM_LAVAPOT, ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_SCRAP, ENT_TYPE.ITEM_SKULL, ENT_TYPE.ITEM_TV, ENT_TYPE.ITEM_USHABTI, ENT_TYPE.MONS_YANG}

local function enemy_small_spawn(x, y, l)
    local uid = spawn_entity_snapped_to_floor(pick(enemies_small), x, y, l)
    local ent = get_entity(uid)
    if prng:random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
    end
    if prng:random() < options.enemy_curse_chance/100 then
        attach_ball_and_chain(uid, 0.5, 0)
    end
end
local function enemy_small_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 82 and y <= 90 then return false end
    local floor = get_grid_entity_at(x, y-1, l)
    local air = get_grid_entity_at(x, y, l)
    if floor ~= -1 and air == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local enemy_small_chance = define_procedural_spawn("enemy_small", enemy_small_spawn, enemy_small_valid)

local function enemy_big_spawn(x, y, l)
    local id = pick(enemies_big)
    local uid = spawn_entity_snapped_to_floor(id, x, y, l)
    local ent = get_entity(uid)
    if id == ENT_TYPE.MOUNT_MECH then
        local rider = spawn_entity(pick({ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_ALIEN}), x, y, l, 0, 0)
        carry(uid, rider)
    elseif prng:random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
    end
end
local function enemy_big_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 82 and y <= 90 then return false end
    local floor = get_grid_entity_at(x, y-1, l)
    local air = get_grid_entity_at(x, y, l)
    local air2 = get_grid_entity_at(x-1, y, l)
    local air3 = get_grid_entity_at(x, y+1, l)
    local air4 = get_grid_entity_at(x-1, y+1, l)
    if floor ~= -1 and air == -1 and air2 == -1 and air3 == -1 and air4 == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local enemy_big_chance = define_procedural_spawn("enemy_big", enemy_big_spawn, enemy_big_valid)

local function enemy_climb_spawn(x, y, l)
    local ladder = get_grid_entity_at(x, y, l)
    if ladder ~= -1 then
        ladder = get_entity(ladder)
        if not test_flag(ladder.flags, ENT_FLAG.CLIMBABLE) then
            return
        end
        local uid = spawn_entity_over(pick(enemies_climb), ladder.uid, 0, 0)
        local ent = get_entity(uid)
        if prng:random() < options.enemy_curse_chance/100 then
            ent:set_cursed(true)
        end
    end
end
local function enemy_climb_valid(x, y, l)
    local ladder = get_grid_entity_at(x, y, l)
    if ladder ~= -1 then
        ladder = get_entity(ladder)
        return test_flag(ladder.flags, ENT_FLAG.CLIMBABLE)
    end
    return false
end
local enemy_climb_chance = define_procedural_spawn("enemy_climb", enemy_climb_spawn, enemy_climb_valid)

local function enemy_ceiling_spawn(x, y, l)
    local uid = spawn_entity(pick(enemies_ceiling), x, y, l, 0, 0)
    local ent = get_entity(uid)
    if prng:random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
    end
end
local function enemy_ceiling_valid(x, y, l)
    local floor = get_grid_entity_at(x, y+1, l)
    local air = get_grid_entity_at(x, y, l)
    if floor ~= -1 and air == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local enemy_ceiling_chance = define_procedural_spawn("enemy_ceiling", enemy_ceiling_spawn, enemy_ceiling_valid)

local function enemy_air_spawn(x, y, l)
    local uid = spawn_entity(pick(enemies_air), x, y, l, 0, 0)
    local ent = get_entity(uid)
    if prng:random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
    end
end
local function enemy_air_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 82 and y <= 90 then return false end
    local air = get_grid_entity_at(x, y, l)
    return air == -1
end
local enemy_air_chance = define_procedural_spawn("enemy_air", enemy_air_spawn, enemy_air_valid)

set_callback(function(ctx)
    --math.randomseed(read_prng()[2])
    ctx:set_procedural_spawn_chance(enemy_small_chance, get_chance(options.enemy_min, options.enemy_max))
    ctx:set_procedural_spawn_chance(enemy_big_chance, get_chance(options.enemy_min, options.enemy_max) * 6)
    ctx:set_procedural_spawn_chance(enemy_climb_chance, get_chance(options.enemy_min, options.enemy_max) * 2)
    ctx:set_procedural_spawn_chance(enemy_ceiling_chance, get_chance(options.enemy_min, options.enemy_max) * 5)
    ctx:set_procedural_spawn_chance(enemy_air_chance, get_chance(options.enemy_min, options.enemy_max) * 12)

    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SNAKE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SPIDER, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.HANGSPIDER, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.GIANTSPIDER, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.BAT, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CAVEMAN, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.HORNEDLIZARD, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MOLE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MANTRAP, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.TIKIMAN, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.WITCHDOCTOR, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MOSQUITO, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.MONKEY, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.ROBOT, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FIREBUG, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.IMP, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.LAVAMANDER, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.VAMPIRE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CROCMAN, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.COBRA, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SORCERESS, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CAT, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.NECROMANCER, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.JIANGSHI, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FEMALE_JIANGSHI, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.OCTOPUS, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.HERMITCRAB, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.UFO, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.YETI, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.OLMITE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FROG, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.FIREFROG, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.TADPOLE, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.GIANTFLY, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.LEPRECHAUN, 0)
    ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.CRABMAN, 0)
end, ON.POST_ROOM_GENERATION)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.MONS_REDSKELETON)

set_post_entity_spawn(function(ent)
    ent.carried_entity_type = pick(crab_items)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_HERMITCRAB)

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.ABZU then return end
    local x, y, l = get_position(ent.uid)
    spawn_entity_nonreplaceable(pick(enemies_kingu), x, y, l, prng:random()*0.3-0.15, prng:random()*0.1+0.1)
end, SPAWN_TYPE.SYSTEMIC, 0, {ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI, ENT_TYPE.MONS_OCTOPUS})

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.OLMEC then
        return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.MONS_UFO)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.ICE_CAVES or l ~= LAYER.BACK then
        return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.MONS_YETI)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.ICE_CAVES or l ~= LAYER.BACK then
        return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(pick(enemies_big, ENT_TYPE.MOUNT_MECH), x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, {ENT_TYPE.MONS_YETIQUEEN, ENT_TYPE.MONS_YETIKING})

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.DWELLING or state.level ~= 4 then
        return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(pick(enemies_big, ENT_TYPE.MOUNT_MECH), x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, {ENT_TYPE.MONS_CAVEMAN_BOSS})

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.OLMEC then return end
    local x, y, l = get_position(ent.uid)
    local players = get_entities_at(0, MASK.PLAYER, x, y, l, 0.5)
    if #players > 0 then return end
    spawn_entity_nonreplaceable(pick(olmec_ammo), x, y, l, prng:random()*0.5-0.25, prng:random()*0.1+0.1)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_BOMB)

local function get_tiamat()
    local tiamat = get_entities_by_type(ENT_TYPE.MONS_TIAMAT)
    if #tiamat < 1 then return nil end
    tiamat = get_entity(tiamat[1])
    if test_flag(tiamat.flags, 29) then return nil end
    return tiamat
end

local function tiamat_scream()
    local tiamat = get_tiamat()
    if not tiamat then return end
    if tiamat.move_state == 0 then
        tiamat.move_state = 2
    end
    --math.randomseed(read_prng()[5])
    set_timeout(tiamat_scream, prng:random(180, 480))
end

local function tiamat_attack()
    local tiamat = get_tiamat()
    if not tiamat then return end
    if tiamat.move_state == 6 then
        --math.randomseed(read_prng()[6])
        local vx = prng:random()*0.8-0.4
        local vy = prng:random()*0.8-0.4
        local ammo = pick(tiamat_ammo)
        local dx = 0
        local dy = 0
        if math.abs(vx) > math.abs(vy) then
            if vx < 0 then
                dx = -1
            elseif vx >= 0 then
                dx = 1
            end
        else
            if vy < 0 then
                dy = -1.5
            elseif vy >= 0 then
                dy = 1.5
            end
        end
        local uid = spawn_entity_nonreplaceable(ammo, tiamat.x+dx, tiamat.y+dy, LAYER.FRONT, vx, vy)
        local ent = get_entity(uid)
        ent.owner_uid = tiamat.uid
        ent.last_owner_uid = tiamat.uid
    end
end

set_callback(function()
    if state.theme ~= THEME.TIAMAT then return end
    --math.randomseed(read_prng()[5])
    set_timeout(tiamat_scream, 60)
    set_interval(tiamat_attack, 2)
end, ON.LEVEL)

--[[ROOMS]]
register_option_float("room_shop_chance", "Extra shop chance", 15, 0, 100)
register_option_float("room_big_chance", "Huge level chance", 15, 0, 100)
register_option_int("room_big_min", "Huge level min height", 8, 8, 15)
register_option_int("room_big_max", "Huge level max height", 15, 8, 15)
register_option_float("room_dark", "Dark level chance", 4, 0, 100)

local valid_rooms_with_shop_next = {
    [ROOM_TEMPLATE.PATH_NORMAL] = true,
    [ROOM_TEMPLATE.PATH_DROP] = true,
    [ROOM_TEMPLATE.PATH_NOTOP] = true,
    [ROOM_TEMPLATE.PATH_DROP_NOTOP] = true,
    [ROOM_TEMPLATE.EXIT] = true,
    [ROOM_TEMPLATE.EXIT_NOTOP] = true,
    [ROOM_TEMPLATE.ENTRANCE] = true,
    [ROOM_TEMPLATE.ENTRANCE_DROP] = true,
    [ROOM_TEMPLATE.ALTAR] = true,
}
local new_rooms = {ROOM_TEMPLATE.SHOP}
local new_rooms_left = {ROOM_TEMPLATE.SHOP_LEFT}

set_callback(function(ctx)
    --math.randomseed(read_prng()[5])
    local orig_width = state.width
    local orig_height = state.height
    local exit_x = 1
    local exit_y = 3
    local big_ocean = false
    if state.world < 7 and not (state.world == 6 and state.level == 2) and state.width == 4 and state.height == 4 and prng:random() < options.room_big_chance/100 then
        state.width = prng:random(4,5)
        state.height = prng:random(math.max(8, options.room_big_min), math.min(15, options.room_big_max))
        toast("My voice REALLY echoes in here!")
    elseif state.theme == THEME.COSMIC_OCEAN and prng:random() < options.room_big_chance/100 then
        state.width = prng:random(4, 8)
        state.height = prng:random(3, 8)
        big_ocean = true
    end
    for x = 0, state.width - 1 do
        for y = 0, state.height - 1 do
            local here = get_room_template(x, y, 0)
            if (here == ROOM_TEMPLATE.EXIT or here == ROOM_TEMPLATE.EXIT_NOTOP) and state.height > orig_height then
                ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.PATH_DROP_NOTOP)
                exit_x = x
                exit_y = y
            end
        end
    end
    if state.height > orig_height and state.world < 7 then
        local no_exit = true
        local x = exit_x
        local y = exit_y
        local dirs = {"left", "down", "right"}
        local last_dir = "down"
        local dir = "down"
        while no_exit do
            last_dir = dir
            if last_dir == "left" then
                dir = dirs[prng:random(1, 2)]
            elseif last_dir == "right" then
                dir = dirs[prng:random(2, 3)]
            else
                dir = dirs[prng:random(3)]
            end
            if (dir == "left" and x == 0) or (dir == "right" and x == state.width - 1) then
                dir = "down"
            end
            if dir == "down" and y < state.height - 1 then
                if last_dir == "down" then
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.PATH_DROP_NOTOP)
                else
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.PATH_DROP)
                end
                y = y + 1
            elseif dir == "left" and x > 0 then
                if last_dir == "down" then
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.PATH_NOTOP)
                else
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.PATH_NORMAL)
                end
                x = x - 1
            elseif dir == "right" and x < state.width - 1 then
                if last_dir == "down" then
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.PATH_NOTOP)
                else
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.PATH_NORMAL)
                end
                x = x + 1
            else
                if last_dir == "down" then
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.EXIT_NOTOP)
                else
                    ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.EXIT)
                end
                no_exit = false
            end
        end
    elseif big_ocean then
        for x = 0, state.width - 1 do
            for y = 0, state.height - 1 do
                if x == 0 or y == 0 or x == state.width - 1 or y == state.height - 1 then
                    ctx:set_room_template(x, y, 0, 9)
                else
                    ctx:set_room_template(x, y, 0, 0)
                end
            end
        end
        local spawn_x = prng:random(1, state.width-2)
        local spawn_y = prng:random(1, state.height-2)
        repeat
            exit_x = prng:random(1, state.width-2)
            exit_y = prng:random(1, state.height-2)
        until(spawn_x ~= exit_x or spawn_y ~= exit_y)
        ctx:set_room_template(spawn_x, spawn_y, 0, ROOM_TEMPLATE.ENTRANCE)
        ctx:set_room_template(exit_x, exit_y, 0, ROOM_TEMPLATE.EXIT)
        state.level_gen.spawn_room_x = spawn_x
        state.level_gen.spawn_room_y = spawn_y
    end
    if state.level_gen.shop_type ~= SHOP_TYPE.DICE_SHOP and state.level_gen.shop_type ~= SHOP_TYPE.TUSK_DICE_SHOP then
        for x = 0, state.width - 1 do
            for y = 0, state.height - 1 do
                local here = get_room_template(x, y, 0)
                if here == ROOM_TEMPLATE.SIDE then
                    local left = get_room_template(x - 1, y, 0)
                    if valid_rooms_with_shop_next[left] then
                        --math.randomseed(read_prng()[6]+x+y)
                        if prng:random() < options.room_shop_chance/100 then
                            ctx:set_room_template(x, y, 0, pick(new_rooms_left))
                        end
                    else
                        local right = get_room_template(x + 1, y, 0)
                        if valid_rooms_with_shop_next[right] then
                            --math.randomseed(read_prng()[6]+x+y)
                            if prng:random() < options.room_shop_chance/100 then
                                ctx:set_room_template(x, y, 0, pick(new_rooms))
                            end
                        end
                    end
                end
            end
        end
    end
    if prng:random() < options.room_dark/100 then
        state.level_flags = set_flag(state.level_flags, 18)
    end
end, ON.POST_ROOM_GENERATION)

--[[SHOPS]]
local shop_items = {ENT_TYPE.ITEM_PICKUP_ROPEPILE, ENT_TYPE.ITEM_PICKUP_BOMBBAG, ENT_TYPE.ITEM_PICKUP_BOMBBOX, ENT_TYPE.ITEM_PICKUP_PARACHUTE, ENT_TYPE.ITEM_PICKUP_SPECTACLES, ENT_TYPE.ITEM_PICKUP_SKELETON_KEY, ENT_TYPE.ITEM_PICKUP_COMPASS, ENT_TYPE.ITEM_PICKUP_SPRINGSHOES, ENT_TYPE.ITEM_PICKUP_SPIKESHOES, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_PICKUP_PITCHERSMITT, ENT_TYPE.ITEM_PICKUP_CLIMBINGGLOVES, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_MACHETE, ENT_TYPE.ITEM_BOOMERANG, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_MATTOCK, ENT_TYPE.ITEM_TELEPORTER, ENT_TYPE.ITEM_FREEZERAY, ENT_TYPE.ITEM_METAL_SHIELD, ENT_TYPE.ITEM_PURCHASABLE_CAPE, ENT_TYPE.ITEM_PURCHASABLE_HOVERPACK, ENT_TYPE.ITEM_PURCHASABLE_TELEPORTER_BACKPACK, ENT_TYPE.ITEM_PURCHASABLE_POWERPACK, ENT_TYPE.ITEM_PURCHASABLE_JETPACK, ENT_TYPE.ITEM_PRESENT, ENT_TYPE.ITEM_PICKUP_HEDJET, ENT_TYPE.ITEM_PICKUP_ROYALJELLY, ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_SKULL, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY}
local extra_shop_items = {ENT_TYPE.ITEM_LIGHT_ARROW, ENT_TYPE.ITEM_PICKUP_GIANTFOOD, ENT_TYPE.ITEM_PICKUP_ELIXIR, ENT_TYPE.ITEM_PICKUP_CLOVER, ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, ENT_TYPE.ITEM_PICKUP_UDJATEYE, ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_CROWN, ENT_TYPE.ITEM_PICKUP_EGGPLANTCROWN, ENT_TYPE.ITEM_PICKUP_TRUECROWN, ENT_TYPE.ITEM_PICKUP_ANKH, ENT_TYPE.ITEM_CLONEGUN, ENT_TYPE.ITEM_HOUYIBOW, ENT_TYPE.ITEM_WOODEN_SHIELD, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SNAP_TRAP} --scepter, vlads cape and the swords don't work
local all_shop_items = join(shop_items, extra_shop_items)
local shop_guns = {ENT_TYPE.ITEM_SHOTGUN, ENT_TYPE.ITEM_PLASMACANNON, ENT_TYPE.ITEM_FREEZERAY, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_CROSSBOW}
local extra_shop_guns = {ENT_TYPE.ITEM_CLONEGUN}
local all_shop_guns = join(shop_guns, extra_shop_guns)
local shop_mounts = {ENT_TYPE.MOUNT_AXOLOTL, ENT_TYPE.MOUNT_MECH, ENT_TYPE.MOUNT_QILIN, ENT_TYPE.MOUNT_ROCKDOG, ENT_TYPE.MOUNT_TURKEY}
local shop_rooms = {ROOM_TEMPLATE.SHOP, ROOM_TEMPLATE.SHOP_LEFT, ROOM_TEMPLATE.CURIOSHOP, ROOM_TEMPLATE.CURIOSHOP_LEFT, ROOM_TEMPLATE.CAVEMANSHOP, ROOM_TEMPLATE.CAVEMANSHOP_LEFT}

set_callback(function()
    local in_shop = {}
    local items = get_entities_by_mask(MASK.ITEM | MASK.MOUNT | MASK.PLAYER | MASK.MONSTER)
    for i,v in ipairs(items) do
        local e = get_entity(v)
        if test_flag(e.flags, ENT_FLAG.SHOP_ITEM) then
            in_shop[#in_shop+1] = e
        end
    end
    for i,v in ipairs(in_shop) do
        --math.randomseed(read_prng()[8]+i)
        v.price = prng:random(1000, math.min(20000, 2*get_money())+prng:random(1500, 6000))
    end
end, ON.POST_LEVEL_GENERATION)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        --math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        return spawn_entity_nonreplaceable(pick(all_shop_items), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN, MASK.ITEM, shop_items)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        --math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        return spawn_entity_nonreplaceable(pick(shop_mounts), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN, MASK.MOUNT, shop_mounts)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        --math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        local item = prng:random(ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_EGGPLANT_CHILD)
        if item == ENT_TYPE.CHAR_CLASSIC_GUY + 1 then
            item = ENT_TYPE.CHAR_EGGPLANT_CHILD
        end
        return spawn_entity_nonreplaceable(item, x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.CHAR_HIREDHAND)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        --math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        return spawn_entity_nonreplaceable(pick(all_shop_guns), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, MASK.ITEM, shop_guns)

--[[CONTAINERS]]
register_option_float("pot_chance", "Pot contents chance", 25, 0, 100)
register_option_float("ushabti_chance", "Correct ushabti chance", 25, 0, 100)

local pot_items = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_HANGSPIDER, ENT_TYPE.MONS_GIANTSPIDER,
         ENT_TYPE.MONS_BAT, ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_REDSKELETON,
         ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD, ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_MANTRAP,
         ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_MOSQUITO, ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN,
         ENT_TYPE.MONS_ROBOT, ENT_TYPE.MONS_FIREBUG_UNCHAINED, ENT_TYPE.MONS_IMP, ENT_TYPE.MONS_VAMPIRE,
         ENT_TYPE.MONS_VLAD, ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS,
         ENT_TYPE.MONS_CATMUMMY, ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI,
         ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_UFO, ENT_TYPE.MONS_ALIEN,
         ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER, ENT_TYPE.MONS_SHOPKEEPERCLONE, ENT_TYPE.MONS_OLMITE_HELMET,
         ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED, ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_AMMIT,
         ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG, ENT_TYPE.MONS_GRUB, ENT_TYPE.MONS_TADPOLE, ENT_TYPE.MONS_JUMPDOG,
         ENT_TYPE.MONS_SCARAB, ENT_TYPE.MONS_SHOPKEEPER, ENT_TYPE.MONS_MERCHANT, ENT_TYPE.MONS_YANG,
         ENT_TYPE.MONS_SISTER_PARSLEY, ENT_TYPE.MONS_SISTER_PARSNIP, ENT_TYPE.MONS_SISTER_PARMESAN,
         ENT_TYPE.MONS_OLD_HUNTER, ENT_TYPE.MONS_THIEF, ENT_TYPE.MONS_MADAMETUSK, ENT_TYPE.MONS_BODYGUARD,
         ENT_TYPE.MONS_HUNDUNS_SERVANT, ENT_TYPE.MONS_GOLDMONKEY, ENT_TYPE.MONS_LEPRECHAUN, ENT_TYPE.MONS_MEGAJELLYFISH,
         ENT_TYPE.MONS_GHOST_SMALL_SURPRISED,
         ENT_TYPE.MONS_GHOST_SMALL_HAPPY, ENT_TYPE.MONS_CRITTERDUNGBEETLE, ENT_TYPE.MONS_CRITTERBUTTERFLY,
         ENT_TYPE.MONS_CRITTERSNAIL, ENT_TYPE.MONS_CRITTERFISH, ENT_TYPE.MONS_CRITTERCRAB, ENT_TYPE.MONS_CRITTERLOCUST,
         ENT_TYPE.MONS_CRITTERPENGUIN, ENT_TYPE.MONS_CRITTERFIREFLY, ENT_TYPE.MONS_CRITTERDRONE,
         ENT_TYPE.MONS_CRITTERSLIME, ENT_TYPE.ITEM_BOMB, ENT_TYPE.ITEM_PASTEBOMB, ENT_TYPE.ITEM_IDOL,
         ENT_TYPE.ITEM_MADAMETUSK_IDOL, ENT_TYPE.ITEM_MADAMETUSK_IDOLNOTE, ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_WEB,
         ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_BROKEN_ARROW, ENT_TYPE.ITEM_METAL_ARROW, ENT_TYPE.ITEM_LIGHT_ARROW,
         ENT_TYPE.ITEM_SCEPTER_ANUBISSPECIALSHOT, ENT_TYPE.ITEM_CHEST, ENT_TYPE.ITEM_VAULTCHEST, ENT_TYPE.ITEM_KEY,
         ENT_TYPE.ITEM_LOCKEDCHEST, ENT_TYPE.ITEM_LOCKEDCHEST_KEY, ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_BOOMBOX,
         ENT_TYPE.ITEM_TORCH, ENT_TYPE.ITEM_PRESENT, ENT_TYPE.ITEM_BROKEN_MATTOCK, ENT_TYPE.ITEM_PUNISHBALL,
         ENT_TYPE.ITEM_COFFIN, ENT_TYPE.ITEM_FLY, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_CURSING_CLOUD,
         ENT_TYPE.ITEM_USHABTI, ENT_TYPE.ITEM_HONEY, ENT_TYPE.ITEM_DIE, ENT_TYPE.ITEM_ANUBIS_COFFIN,
         ENT_TYPE.ITEM_AXOLOTL_BUBBLESHOT, ENT_TYPE.ITEM_POTOFGOLD, ENT_TYPE.ITEM_FROZEN_LIQUID,
         ENT_TYPE.ITEM_SNAP_TRAP, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_CURSEDPOT, ENT_TYPE.ITEM_SKULL, ENT_TYPE.ITEM_BONES,
         ENT_TYPE.ITEM_COOKFIRE, ENT_TYPE.ITEM_LAVAPOT, ENT_TYPE.ITEM_SCRAP, ENT_TYPE.ITEM_EGGPLANT,
         ENT_TYPE.ITEM_GOLDBAR, ENT_TYPE.ITEM_GOLDBARS, ENT_TYPE.ITEM_DIAMOND, ENT_TYPE.ITEM_EMERALD,
         ENT_TYPE.ITEM_SAPPHIRE, ENT_TYPE.ITEM_RUBY, ENT_TYPE.ITEM_NUGGET, ENT_TYPE.ITEM_GOLDCOIN,
         ENT_TYPE.ITEM_EMERALD_SMALL, ENT_TYPE.ITEM_SAPPHIRE_SMALL, ENT_TYPE.ITEM_RUBY_SMALL,
         ENT_TYPE.ITEM_NUGGET_SMALL, ENT_TYPE.ITEM_PICKUP_ROPE, ENT_TYPE.ITEM_PICKUP_ROPEPILE,
         ENT_TYPE.ITEM_PICKUP_BOMBBAG, ENT_TYPE.ITEM_PICKUP_BOMBBOX, ENT_TYPE.ITEM_PICKUP_ROYALJELLY,
         ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY, ENT_TYPE.ITEM_PICKUP_GIANTFOOD, ENT_TYPE.ITEM_PICKUP_ELIXIR,
         ENT_TYPE.ITEM_PICKUP_CLOVER, ENT_TYPE.ITEM_PICKUP_SEEDEDRUNSUNLOCKER, ENT_TYPE.ITEM_PICKUP_SPECTACLES,
         ENT_TYPE.ITEM_PICKUP_CLIMBINGGLOVES, ENT_TYPE.ITEM_PICKUP_PITCHERSMITT, ENT_TYPE.ITEM_PICKUP_SPRINGSHOES,
         ENT_TYPE.ITEM_PICKUP_SPIKESHOES, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_PICKUP_COMPASS,
         ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, ENT_TYPE.ITEM_PICKUP_PARACHUTE, ENT_TYPE.ITEM_PICKUP_UDJATEYE,
         ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_HEDJET, ENT_TYPE.ITEM_PICKUP_CROWN,
         ENT_TYPE.ITEM_PICKUP_EGGPLANTCROWN, ENT_TYPE.ITEM_PICKUP_TRUECROWN, ENT_TYPE.ITEM_PICKUP_ANKH,
         ENT_TYPE.ITEM_PICKUP_TABLETOFDESTINY, ENT_TYPE.ITEM_PICKUP_SKELETON_KEY, ENT_TYPE.ITEM_PICKUP_PLAYERBAG,
         ENT_TYPE.ITEM_CAPE, ENT_TYPE.ITEM_VLADS_CAPE, ENT_TYPE.ITEM_JETPACK, ENT_TYPE.ITEM_TELEPORTER_BACKPACK,
         ENT_TYPE.ITEM_HOVERPACK, ENT_TYPE.ITEM_POWERPACK, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_SHOTGUN,
         ENT_TYPE.ITEM_FREEZERAY, ENT_TYPE.ITEM_CROSSBOW, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_TELEPORTER,
         ENT_TYPE.ITEM_MATTOCK, ENT_TYPE.ITEM_BOOMERANG, ENT_TYPE.ITEM_MACHETE, ENT_TYPE.ITEM_EXCALIBUR,
         ENT_TYPE.ITEM_BROKENEXCALIBUR, ENT_TYPE.ITEM_PLASMACANNON, ENT_TYPE.ITEM_SCEPTER, ENT_TYPE.ITEM_CLONEGUN,
         ENT_TYPE.ITEM_HOUYIBOW, ENT_TYPE.ITEM_WOODEN_SHIELD, ENT_TYPE.ITEM_METAL_SHIELD, ENT_TYPE.ACTIVEFLOOR_BOULDER,
         ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK, ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP,
         ENT_TYPE.ACTIVEFLOOR_ELEVATOR, ENT_TYPE.FX_EXPLOSION, ENT_TYPE.FX_POWEREDEXPLOSION, ENT_TYPE.MOUNT_TURKEY,
         ENT_TYPE.MOUNT_ROCKDOG, ENT_TYPE.MOUNT_AXOLOTL}
local crate_items = {ENT_TYPE.ITEM_LIGHT_ARROW, ENT_TYPE.ITEM_PRESENT, ENT_TYPE.ITEM_PICKUP_BOMBBOX,
         ENT_TYPE.ITEM_PICKUP_ROYALJELLY, ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY, ENT_TYPE.ITEM_PICKUP_GIANTFOOD,
         ENT_TYPE.ITEM_PICKUP_ELIXIR, ENT_TYPE.ITEM_PICKUP_CLOVER, ENT_TYPE.ITEM_PICKUP_SPECTACLES,
         ENT_TYPE.ITEM_PICKUP_CLIMBINGGLOVES, ENT_TYPE.ITEM_PICKUP_PITCHERSMITT, ENT_TYPE.ITEM_PICKUP_SPRINGSHOES,
         ENT_TYPE.ITEM_PICKUP_SPIKESHOES, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_PICKUP_COMPASS,
         ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, ENT_TYPE.ITEM_PICKUP_PARACHUTE, ENT_TYPE.ITEM_PICKUP_UDJATEYE,
         ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_HEDJET, ENT_TYPE.ITEM_PICKUP_CROWN,
         ENT_TYPE.ITEM_PICKUP_EGGPLANTCROWN, ENT_TYPE.ITEM_PICKUP_TRUECROWN, ENT_TYPE.ITEM_PICKUP_ANKH,
         ENT_TYPE.ITEM_PICKUP_SKELETON_KEY, ENT_TYPE.ITEM_PICKUP_PLAYERBAG, ENT_TYPE.ITEM_CAPE,
         ENT_TYPE.ITEM_VLADS_CAPE, ENT_TYPE.ITEM_JETPACK, ENT_TYPE.ITEM_TELEPORTER_BACKPACK, ENT_TYPE.ITEM_HOVERPACK,
         ENT_TYPE.ITEM_POWERPACK, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_SHOTGUN, ENT_TYPE.ITEM_FREEZERAY,
         ENT_TYPE.ITEM_CROSSBOW, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_TELEPORTER, ENT_TYPE.ITEM_MATTOCK,
         ENT_TYPE.ITEM_BOOMERANG, ENT_TYPE.ITEM_MACHETE, ENT_TYPE.ITEM_EXCALIBUR, ENT_TYPE.ITEM_BROKENEXCALIBUR,
         ENT_TYPE.ITEM_PLASMACANNON, ENT_TYPE.ITEM_SCEPTER, ENT_TYPE.ITEM_CLONEGUN, ENT_TYPE.ITEM_HOUYIBOW,
         ENT_TYPE.ITEM_METAL_SHIELD, ENT_TYPE.ITEM_USHABTI}
local abzu_crate_items = {ENT_TYPE.ITEM_EXCALIBUR, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_BROKENEXCALIBUR, ENT_TYPE.ITEM_PICKUP_BOMBBOX}

set_post_entity_spawn(function(ent)
    --math.randomseed(read_prng()[5]+ent.uid)
    if prng:random() < options.pot_chance/100 then
        ent.inside = pick(pot_items)
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_POT)

set_post_entity_spawn(function(ent)
    --math.randomseed(read_prng()[6]+ent.uid)
    if state.theme == THEME.ABZU and prng:random() < 0.5 then
        ent.inside = pick(abzu_crate_items)
    else
        ent.inside = pick(crate_items)
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_CRATE)

set_post_entity_spawn(function(ent)
    local _, _, l = get_position(ent.uid)
    if l == LAYER.BACK and state.theme == THEME.NEO_BABYLON then return end
    ent.animation_frame = get_ushabti_frame()
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_USHABTI)

set_callback(function()
    local coffins = get_entities_by_type(ENT_TYPE.ITEM_COFFIN)
    for i,v in ipairs(coffins) do
        local ent = get_entity(v)
        --math.randomseed(read_prng()[7]+ent.uid)
        local item = prng:random(ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_EGGPLANT_CHILD)
        if item == ENT_TYPE.CHAR_CLASSIC_GUY + 1 then
            item = ENT_TYPE.CHAR_HIREDHAND
        end
        ent.inside = item
    end
end, ON.POST_LEVEL_GENERATION)

--[[STATS]]
register_option_int("stats_health_max", "Max starting health", 20, 4, 99)
register_option_int("stats_bombs_max", "Max starting bombs", 20, 4, 99)
register_option_int("stats_ropes_max", "Max starting ropes", 20, 4, 99)

set_callback(function()
    --math.randomseed(read_prng()[1])
    for i,p in ipairs(players) do
        p.health = prng:random_int(4, options.stats_health_max, 1)
        p.inventory.bombs = prng:random_int(4, options.stats_bombs_max, 1)
        p.inventory.ropes = prng:random_int(4, options.stats_ropes_max, 1)
    end
end, ON.START)

--[[DOORS]]
register_option_int("door_min_levels", "Min levels between midbosses", 3, 1, 100)
register_option_int("door_max_levels", "Max levels between midbosses", 6, 1, 100)
register_option_int("door_bosses", "Amount of midbosses", 4, 0, 4)
register_option_bool("door_transitions", "Neat transitions (maybe crashy)", false)

local level_order = {}

local theme = {1,1,2,2,3,3,5,5,6,6,7,8,8,9,10,10,11}
local bosses = {THEME.OLMEC, THEME.ABZU, THEME.DUAT, THEME.HUNDUN}
local world = {1,2,2,3,4,4,5,6,7,8,4,4,4,6,7,7,1}
local dead = true
local co_level = 5

local insert_bosses = {}
local bosses_killed = {}
local bosses_added = 0
local orig_chain_items = {ENT_TYPE.ITEM_PICKUP_UDJATEYE, ENT_TYPE.ITEM_PICKUP_CROWN, ENT_TYPE.ITEM_PICKUP_HEDJET, ENT_TYPE.ITEM_PICKUP_TABLETOFDESTINY, ENT_TYPE.ITEM_PICKUP_ANKH, ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_ELIXIR, ENT_TYPE.ITEM_PICKUP_SKELETON_KEY}
local chain_items = {}
local boss_warp = false

local critters_spawned = true
local critters = {}
critters[THEME.DWELLING] = ENT_TYPE.MONS_CRITTERDUNGBEETLE
critters[THEME.JUNGLE] = ENT_TYPE.MONS_CRITTERBUTTERFLY
critters[THEME.VOLCANA] = ENT_TYPE.MONS_CRITTERSNAIL
critters[THEME.TIDE_POOL] = ENT_TYPE.MONS_CRITTERCRAB
critters[THEME.TEMPLE] = ENT_TYPE.MONS_CRITTERLOCUST
critters[THEME.ICE_CAVES] = ENT_TYPE.MONS_CRITTERPENGUIN
critters[THEME.NEO_BABYLON] = ENT_TYPE.MONS_CRITTERDRONE
critters[THEME.SUNKEN_CITY] = ENT_TYPE.MONS_CRITTERSLIME
critters[THEME.CITY_OF_GOLD] = ENT_TYPE.MONS_SCARAB
for i,v in ipairs(bosses) do
    critters[v] = ENT_TYPE.ITEM_LITWALLTORCH
end

local function set_doors()
    if #players > 0 then
        local px, py, pl = get_position(players[1].uid)
        local doors = get_entities_at(ENT_TYPE.FLOOR_DOOR_EXIT, 0, px, py, pl, 15)
        for i,v in ipairs(doors) do
            local x, y, layer = get_position(v)
            if state.theme == THEME.HUNDUN then
                unlock_door_at(x, y)
            end
            if #level_order >= state.level_count+2 then
                local nexttheme = level_order[state.level_count+2].t
                if not critters_spawned and critters[nexttheme] ~= nil then
                    spawn(critters[nexttheme], x-0.7, y+0.5, layer, 0, 0)
                    spawn(critters[nexttheme], x+0.7, y+0.5, layer, 0, 0)
                    critters_spawned = true
                end
            end
        end
    end
end

local function add_level(w, l, t)
    level_order[#level_order+1] = { w = w, l = l, t = t, b = has(bosses, t) or t == THEME.TIAMAT }
end

local function bosses_left()
    return options.door_bosses+1-#bosses_killed
end

local function add_boss(boss)
    for k,v in ipairs(insert_bosses) do
        if v == boss then
            table.remove(insert_bosses, k)
            bosses_added = bosses_added + 1
            return
        end
    end
end

local function kill_boss(boss)
    if not has(bosses_killed, boss) then
        bosses_killed[#bosses_killed+1] = boss
        toast("Boss defeated!\nBosses remaining: "..tostring(bosses_left()))
    end
end

local function insert_chain(t, l)
    for i,v in ipairs(level_order) do
        if v.t == t then
            table.insert(level_order, i - prng:random_int(0, options.door_min_levels, 0), l)
            break
        end
    end
end

local function fix_chain()
    --math.randomseed(read_prng()[1])
    --insert_chain(THEME.ABZU, { w = 4, l = 2, t = THEME.TIDE_POOL, b = false})
    insert_chain(THEME.TIAMAT, { w = 6, l = 2, t = THEME.NEO_BABYLON, b = false})
end

local function get_chain_item(x, y)
    if #chain_items > 0 then
        return table.remove(chain_items, 1)
    end
    return pick(crate_items)
end

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.VOLCANA then return end
    local x, y, l = get_position(ent.uid)
    local rx, ry = get_room_index(x, y)
    local room = get_room_template(rx, ry, l)
    if room == ROOM_TEMPLATE.OLDHUNTER_REWARDROOM then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, (prng:random()-0.5)*0.2, 0.2)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_DIAMOND)

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.DWELLING then return end
    local x, y, l = get_position(ent.uid)
    local rx, ry = get_room_index(x, y)
    local room = get_room_template(rx, ry, l)
    if room == ROOM_TEMPLATE.UDJATTOP then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, (prng:random()-0.5)*0.2, 0.2)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_PICKUP_UDJATEYE)

set_callback(function()
    if state.theme == THEME.VOLCANA then
        local crowns = get_entities_by(ENT_TYPE.ITEM_PICKUP_CROWN, 0, LAYER.BACK)
        for i,v in ipairs(crowns) do
            local x, y, l = get_position(v)
            kill_entity(v)
            local item = spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
            item = get_entity(item)
            item.flags = set_flag(item.flags, ENT_FLAG.NO_GRAVITY)
        end
    elseif state.theme == THEME.OLMEC then
        local ankhs = get_entities_by(ENT_TYPE.ITEM_PICKUP_ANKH, 0, LAYER.BACK)
        for i,v in ipairs(ankhs) do
            local x, y, l = get_position(v)
            kill_entity(v)
            local item = spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
        end
    elseif state.theme == THEME.TIDE_POOL then
        local notes = get_entities_by(ENT_TYPE.ITEM_MADAMETUSK_IDOLNOTE, 0, LAYER.BACK)
        for i,v in ipairs(notes) do
            local x, y, l = get_position(v)
            kill_entity(v)
            local item = spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
        end
    end
end, ON.LEVEL)

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.ABZU and state.theme ~= THEME.DUAT then return end
    if #chain_items > 0 then
        local x, y, l = get_position(ent.uid)
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_PICKUP_TABLETOFDESTINY)

set_post_entity_spawn(function(ent)
    local x, y, l = get_position(ent.uid)
    if l == LAYER.BACK and state.world == 2 then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_HOUYIBOW)
set_post_entity_spawn(function(ent)
    kill_entity(ent.uid)
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_METAL_ARROW)

set_post_entity_spawn(function(ent)
    local x, y, l = get_position(ent.uid)
    if state.theme == THEME.SUNKEN_CITY and l == LAYER.BACK then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_LIGHT_ARROW)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme == THEME.ICE_CAVES and l == LAYER.BACK then
        return spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_PLASMACANNON)

set_post_entity_spawn(function(ent)
    local x, y, l = get_position(ent.uid)
    if state.world == 4 and l == LAYER.BACK then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(x, y), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_CLONEGUN)

local ice_themes = {THEME.DWELLING, THEME.ICE_CAVES, THEME.OLMEC, THEME.TEMPLE, THEME.CITY_OF_GOLD}
local function shuffle_tile_codes()
    for k,v in pairs(floor_tilecodes) do
        floor_tilecodes[k] = pick(floor_types, ENT_TYPE.FLOORSTYLED_COG)
        if prng:random() < 0.15 and has(ice_themes, state.theme_next) then
            floor_tilecodes[k] = ENT_TYPE.FLOOR_ICE
        end
        local type = k
        set_pre_tile_code_callback(function(x, y, layer)
            --if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 82 and y <= 90 then return false end
            local above = get_grid_entity_at(x, y+1, layer)
            if above ~= -1 then
                above = get_entity(above)
                if above.type.id == ENT_TYPE.FLOOR_SPIKES then
                    return
                end
            end
            spawn_grid_entity(floor_tilecodes[type], x, y, layer);
            return true
        end, type)
    end
    floor_tilecodes["shop_wall"] = floor_tilecodes["floor"]
    floor_tilecodes["shop_sign"] = floor_tilecodes["floor"]
end

local function init_run()
    if test_flag(state.quest_flags, 7) then
        seed_prng(state.seed)
    end
    --message("Started new run")
    state.level_count = 0
    --math.randomseed(read_prng()[1])
    level_order = {}
    insert_bosses = {table.unpack(bosses)}
    chain_items = {table.unpack(orig_chain_items)}
    shuffle(chain_items)
    bosses_killed = {}
    bosses_added = 0
    boss_warp = false
    local normal_levels = 0
    local done = false
    while not done do
        if prng:random_int(1, 120, 0) == 1 then
            add_level(7, 2, THEME.EGGPLANT_WORLD)
        elseif bosses_added < options.door_bosses and #insert_bosses > 0 and normal_levels >= options.door_min_levels and prng:random_int(1, options.door_max_levels, 0) <= normal_levels then
            normal_levels = 0
            local t = pick(insert_bosses)
            local l = 4
            if t == THEME.OLMEC then
                l = 1
            end
            local w = world[t]
            add_level(w, l, t)
            add_boss(t)
        elseif (bosses_added >= options.door_bosses or #insert_bosses == 0) and normal_levels >= options.door_min_levels and prng:random_int(1, options.door_max_levels, 0) <= normal_levels then
            add_level(6, 4, THEME.TIAMAT)
            done = true
        else
            normal_levels = normal_levels+1
            local lasttheme = nil
            if #level_order > 0 then
                lasttheme = level_order[#level_order].t
            end
            local t = pick(theme, lasttheme)
            local l = 1
            if t == THEME.NEO_BABYLON or t == THEME.SUNKEN_CITY then
                l = prng:random_int(1, 3, 0)
            elseif t == THEME.CITY_OF_GOLD then
                l = 3
            elseif t == THEME.COSMIC_OCEAN then
                l = prng:random_int(5, 97, 0)
            elseif t == THEME.ICE_CAVES then
                l = 1
            else
                l = prng:random_int(1, 4, 0)
            end
            local w = world[t]
            add_level(w, l, t)
        end
    end
    fix_chain()
    --[[level_order = {
        { w = 1, l = 1, t = THEME.DWELLING, b = false },
        { w = 1, l = 2, t = THEME.DWELLING, b = false },
        { w = 1, l = 3, t = THEME.DWELLING, b = false },
        { w = 1, l = 4, t = THEME.DWELLING, b = false },
        { w = 4, l = 3, t = THEME.CITY_OF_GOLD, b = false },
        { w = 2, l = 3, t = THEME.VOLCANA, b = false },
        { w = 4, l = 4, t = THEME.ABZU, b = false },
        { w = 5, l = 1, t = THEME.ICE_CAVES, b = false },
        { w = 4, l = 3, t = THEME.CITY_OF_GOLD, b = false },
        { w = 7, l = 1, t = THEME.SUNKEN_CITY, b = false },
        { w = 4, l = 2, t = THEME.TEMPLE, b = false },
        { w = 2, l = 1, t = THEME.JUNGLE, b = false },
        { w = 6, l = 4, t = THEME.TIAMAT, b = true },
        { w = 1, l = 1, t = THEME.DWELLING, b = false },
        { w = 2, l = 1, t = THEME.JUNGLE, b = false },
        { w = 7, l = 4, t = THEME.HUNDUN, b = true },
    }]]
    if options.tilecode == true then shuffle_tile_codes() end
end

local function dead_olmec()
    if state.theme == THEME.OLMEC then
        local olmecs = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_OLMEC)
        if #olmecs > 0 then
            local x, y, l = get_position(olmecs[1])
            if y < 71 then -- this olmec is low enough
                kill_boss(THEME.OLMEC)
            end
        end
    end
end

local function dead_tiamat()
    if state.theme == THEME.TIAMAT then
        local tiamats = get_entities_by_type(ENT_TYPE.MONS_TIAMAT)
        if #tiamats > 0 then
            local tiamat = get_entity(tiamats[1])
            if testflag(tiamat.flags, 29) then -- this tiamat is dead
                kill_boss(THEME.TIAMAT)
            end
        end
    end
end

local function dead_hundun()
    if state.theme == THEME.HUNDUN then
        local hunduns = get_entities_by_type(ENT_TYPE.MONS_HUNDUN)
        if #hunduns > 0 then
            if get_entity_ai_state(hunduns[1]) == 4 then -- this hundun is just chillin on the floor
                kill_boss(THEME.HUNDUN)
            end
        end
    end
end

local function dead_kingu()
    if test_flag(state.journal_flags, 16) then
        kill_boss(THEME.ABZU)
    end
end

local function dead_osiris()
    if test_flag(state.journal_flags, 17) then
        kill_boss(THEME.DUAT)
    end
end

local function duat_door()
    -- spawn duat skip door
    spawn_door(17, 106, 0, level_order[state.level_count+1].w, level_order[state.level_count+1].l, level_order[state.level_count+1].t)
    spawn_entity(ENT_TYPE.BG_DOOR_BACK_LAYER, 17, 106, 0, 0, 0)
end

set_callback(function()
    --message("Level")
    critters_spawned = false
    dead = false

    if state.theme == THEME.DUAT then
        set_timeout(duat_door, 60)
    end

    set_interval(function()
        set_doors()
        dead_olmec()
        dead_tiamat()
        dead_hundun()
        dead_kingu()
        dead_osiris()
    end, 15)

    if state.theme == THEME.HUNDUN then
        local doors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
        for i,v in ipairs(doors) do
            local x, y, l = get_position(v)
            move_entity(v, 70, y, 0, 0)
            spawn_door(x, y, l, state.world, state.level, state.theme)
        end
    end
    if state.theme == THEME.TIAMAT then
        set_interval(function()
            if bosses_left() <= 0 then
                local hp = 0
                for i,p in pairs(players) do
                    hp = hp + p.health
                end
                co_level = math.max(5, math.min(98, hp))
            end
        end, 15)
    end

    if #level_order > state.level_count+1 then
        --prinspect("next level", level_order[state.level_count+2].w, level_order[state.level_count+2].l, theme_name[level_order[state.level_count+2].t])
    end

end, ON.LEVEL)

set_callback(function()
    --message("Transition")
    toast("Level "..tostring(state.level_count).." completed!\nBosses remaining: "..tostring(bosses_left()))
    --message("Transition - Setting next level")
    local num = state.level_count+1
    if test_flag(state.quest_flags, 1) then
        num = 1
    end
    if #level_order > state.level_count then
        state.world_next = level_order[num].w
        state.level_next = level_order[num].l
        state.theme_next = level_order[num].t
        --state.quest_flags = 0
    end
end, ON.TRANSITION)

set_callback(function()
    --message("Loading")
    if state.screen_next ~= ON.LEVEL and (state.screen_next ~= ON.TRANSITION or not options.door_transitions) then return end
    --if state.loading ~= 1 then return end
    if (#level_order == 0 or test_flag(state.quest_flags, 1)) then
        --message("Running init")
        init_run()
    end
    local num = state.level_count+1
    if test_flag(state.quest_flags, 1) then
        num = 1
    end
    if boss_warp then
        while level_order[num].b ~= true and #level_order >= num do
            num = num + 1
        end
        state.level_count = num-1
    end
    if num > 1 and #level_order >= num and level_order[num].t == THEME.COSMIC_OCEAN and level_order[num-1].t == THEME.JUNGLE then
        return
    end
    if #level_order > state.level_count then
        state.world_next = level_order[num].w
        state.level_next = level_order[num].l
        state.theme_next = level_order[num].t
        --prinspect("going to", level_order[num].w, level_order[num].l, theme_name[level_order[num].t])
    elseif #level_order <= state.level_count and state.theme_next ~= THEME.COSMIC_OCEAN then
        --prinspect("last level, going to", co_level)
        state.world_next = 8
        state.level_next = co_level
        state.theme_next = THEME.COSMIC_OCEAN
    end
    if options.tilecode == true then shuffle_tile_codes() end
end, ON.LOADING)

set_callback(function(ctx)
    if boss_warp and state.screen == ON.TRANSITION then
        local color = prng:random(0, 0xffffffff)
        local _, size = get_window_size()
        size = size / 5
        local text = 'WARP ZONE'
        local w, h = draw_text_size(size, text)
        ctx:draw_text(0-w/2, -0.57-h/2, size, text, color)
    end
end, ON.GUIFRAME)

local last_room = -1
local animals = {
    [ENT_TYPE.MONS_PET_CAT] = "a cat",
    [ENT_TYPE.MONS_PET_DOG] = "a dog",
    [ENT_TYPE.MONS_PET_HAMSTER] = "a hamster",
    [ENT_TYPE.MOUNT_ROCKDOG] = "a rockdog",
    [ENT_TYPE.MOUNT_AXOLOTL] = "an axolotl",
    [ENT_TYPE.MOUNT_QILIN] = "an updog",
    [ENT_TYPE.MONS_SCORPION] = "a scorpion",
    [ENT_TYPE.MONS_HORNEDLIZARD] = "a lizard",
    [ENT_TYPE.MONS_SHOPKEEPER] = "a person",
    [ENT_TYPE.MONS_SHOPKEEPERCLONE] = "a person",
    [ENT_TYPE.MONS_PROTOSHOPKEEPER] = "an abomination",
    [ENT_TYPE.MONS_CAVEMAN] = "a person",
    [ENT_TYPE.MONS_TIKIMAN] = "a person",
    [ENT_TYPE.MONS_WITCHDOCTOR] = "a person",
    [ENT_TYPE.MONS_VAMPIRE] = "a person",
    [ENT_TYPE.MONS_SORCERESS] = "a person",
    [ENT_TYPE.MONS_VLAD] = "a person",
    [ENT_TYPE.MONS_SISTER_PARSLEY] = "a person",
    [ENT_TYPE.MONS_SISTER_PARSNIP] = "a person",
    [ENT_TYPE.MONS_SISTER_PARMESAN] = "a person",
    [ENT_TYPE.MONS_HUNDUNS_SERVANT] = "a person",
    [ENT_TYPE.MONS_MERCHANT] = "a person",
    [ENT_TYPE.MONS_OLD_HUNTER] = "a person",
    [ENT_TYPE.MONS_THIEF] = "a person",
    [ENT_TYPE.MONS_BODYGUARD] = "a person",
    [ENT_TYPE.MONS_LEPRECHAUN] = "a person",
    [ENT_TYPE.MONS_FISH] = "a fish",
    [ENT_TYPE.MONS_OCTOPUS] = "an octopus",
    [ENT_TYPE.MONS_HERMITCRAB] = "a crab",
    [ENT_TYPE.MONS_YETI] = "a yeti",
    [ENT_TYPE.MONS_MANTRAP] = "a flower",
    [ENT_TYPE.MONS_MOLE] = "a mole",
    [ENT_TYPE.MONS_CROCMAN] = "a crocodile",
}

local function get_animal_name(uid)
    local ent = get_entity(uid)
    local name = "not a turkey"
    if ent then
        name = animals[ent.type.id]
        if test_flag(ent.flags, ENT_FLAG.DEAD) then
            name = name:gsub("an? ", "a dead ")
        end
        return name
    end
    return nil
end

set_callback(function()
    if not players[1] then return end
    if state.theme == THEME.TEMPLE then
        local x, y, l = get_position(players[1].uid)
        local uids = get_entities_at(ENT_TYPE.BG_DOOR_COG, 0, x, y, l, 2)
        if #uids > 0 then
            boss_warp = true
        else
            boss_warp = false
        end
    elseif state.theme == THEME.HUNDUN then
        local x, y, l = get_position(players[1].uid)
        local uids = get_entities_at(ENT_TYPE.LOGICAL_PORTAL, 0, x, y, l, 5)
        if #uids > 0 then
            boss_warp = true
        else
            boss_warp = false
        end
    elseif state.theme == THEME.SUNKEN_CITY and state.level == 1 then
        local x, y, l = get_position(players[1].uid)
        local uids = get_entities_at(ENT_TYPE.BG_DOOR_EGGPLANT_WORLD, 0, x, y, l, 2)
        if #uids > 0 then
            boss_warp = true
        else
            boss_warp = false
        end
    elseif state.theme == THEME.CITY_OF_GOLD then
        local x, y, l = get_position(players[1].uid)
        local uids = get_entities_at(ENT_TYPE.FLOOR_ALTAR, 0, x, y, l, 2)
        if #uids > 0 then
            boss_warp = true
        else
            boss_warp = false
        end
    elseif state.theme == THEME.TIDE_POOL and state.level == 3 then
        local _, y, _ = get_position(players[1].uid)
        if y < 90 then
            boss_warp = true
        else
            boss_warp = false
        end
    else
        boss_warp = false
    end
    if state.theme == THEME.DWELLING then
        local x, y, l = get_position(players[1].uid)
        local rx, ry = get_room_index(x, y)
        local roomtype = get_room_template(rx, ry, l)
        if roomtype == ROOM_TEMPLATE.PEN_ROOM and roomtype ~= last_room and players[1].holding_uid ~= -1 then
            local yang = get_entities_by_type(ENT_TYPE.MONS_YANG)
            local ent = get_entity(players[1].holding_uid)
            if ent and animals[ent.type.id] ~= nil and #yang > 0 then
                local name = get_animal_name(players[1].holding_uid)
                local gender = "Sir"
                if players[1]:is_female() then
                    gender = "Ma'am"
                end
                local msg = string.format("%s, I'm looking for turkeys. Thats %s.", gender, name)
                if name then
                    say(yang[1], msg, 3, false)
                end
            end
        end
        last_room = roomtype
    end
end, ON.FRAME)

set_callback(function()
    toast("Died after "..tostring(state.level_count).." levels!\nBosses remaining: "..tostring(bosses_left()))
end, ON.DEATH)

--[[PROJECTILES]]
register_option_bool("projectile", "Random projectiles", true)

local projectiles = {ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_LASERTRAP_SHOT, ENT_TYPE.ITEM_FREEZERAYSHOT}
local projectiles_pc = {ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_LASERTRAP_SHOT, ENT_TYPE.ITEM_FREEZERAYSHOT, ENT_TYPE.ITEM_CLONEGUNSHOT, ENT_TYPE.ITEM_PLASMACANNON_SHOT}
local projectiles_clone = {ENT_TYPE.ITEM_LASERTRAP_SHOT, ENT_TYPE.ITEM_FREEZERAYSHOT, ENT_TYPE.ITEM_CLONEGUNSHOT}
local projectiles_arrow = {ENT_TYPE.ITEM_LIGHT_ARROW, ENT_TYPE.ITEM_METAL_ARROW, ENT_TYPE.ITEM_METAL_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW}


set_pre_entity_spawn(function(type, x, y, l, overlay)
    if options.projectile then
        return spawn_entity_nonreplaceable(pick(projectiles), x, y, l, 0, 0)
    end
    return spawn(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_BULLET)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if options.projectile then
        return spawn_entity_nonreplaceable(pick(projectiles_pc), x, y, l, 0, 0)
    end
    return spawn(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_PLASMACANNON_SHOT)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if options.projectile then
        return spawn_entity_nonreplaceable(pick(projectiles), x, y, l, 0, 0)
    end
    return spawn(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_FREEZERAYSHOT)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if options.projectile then
        return spawn_entity_nonreplaceable(pick(projectiles_clone), x, y, l, 0, 0)
    end
    return spawn(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_CLONEGUNSHOT)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if options.projectile then
        return spawn_entity_nonreplaceable(pick(projectiles_arrow), x, y, l, 0, 0)
    end
    return spawn(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, projectiles_arrow)

--[[set_pre_entity_spawn(function(type, x, y, l, overlay)
    if options.projectile then
        return spawn_entity_nonreplaceable(pick(projectiles_clone), x, y, l, 0, 0)
    end
    return spawn(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_LASERTRAP_SHOT)]]

--[[STORAGE]]
register_option_bool("storage", "Random Waddler caches", true)
local storage_bad_rooms = {ROOM_TEMPLATE.SHOP, ROOM_TEMPLATE.SHOP_LEFT, ROOM_TEMPLATE.VAULT, ROOM_TEMPLATE.CURIOSHOP, ROOM_TEMPLATE.CAVEMANSHOP, ROOM_TEMPLATE.SHOP_ATTIC, ROOM_TEMPLATE.SHOP_ATTIC_LEFT, ROOM_TEMPLATE.SHOP_BASEMENT, ROOM_TEMPLATE.SHOP_BASEMENT_LEFT, ROOM_TEMPLATE.TUSKFRONTDICESHOP, ROOM_TEMPLATE.TUSKFRONTDICESHOP_LEFT, ROOM_TEMPLATE.PEN_ROOM}
set_callback(function()
    local storages = get_entities_by_type(ENT_TYPE.FLOOR_STORAGE)
    if #storages == 0 and options.storage and not (state.world == 6 and state.level == 3) then
        local spots = {}
        for x=3,state.width*10+2 do
            for y=122,122-state.height*8+1,-1 do
                local rx, ry = get_room_index(x, y)
                local roomtype = get_room_template(rx, ry, LAYER.BACK)
                if not has(storage_bad_rooms, roomtype) then
                    local floor = get_grid_entity_at(x, y, LAYER.BACK)
                    local floor2 = get_grid_entity_at(x+1, y, LAYER.BACK)
                    local air = get_grid_entity_at(x, y+1, LAYER.BACK)
                    local air2 = get_grid_entity_at(x+1, y+1, LAYER.BACK)
                    if air == -1 and air2 == -1 and floor ~= -1 and floor2 ~= -1 then
                        floor = get_entity(floor)
                        floor2 = get_entity(floor2)
                        local items = get_entities_at(0, MASK.ITEM | MASK.ACTIVEFLOOR, x+0.5, y+0.5, LAYER.BACK, 2)
                        if #items == 0 and has(valid_floors, floor.type.id) and has(valid_floors, floor2.type.id) then
                            spots[#spots+1] = { x = x, y = y }
                        end
                    end
                end
            end
        end
        if #spots > 0 then
            local spot = spots[prng:random(#spots)]
            local floor = get_grid_entity_at(spot.x, spot.y, LAYER.BACK)
            local floor2 = get_grid_entity_at(spot.x+1, spot.y, LAYER.BACK)
            if floor ~= -1 then kill_entity(floor) end
            if floor2 ~= -1 then kill_entity(floor2) end
            spawn_grid_entity(ENT_TYPE.FLOOR_STORAGE, spot.x, spot.y, LAYER.BACK)
            spawn_grid_entity(ENT_TYPE.FLOOR_STORAGE, spot.x+1, spot.y, LAYER.BACK)
            for i=0,9 do
                local type = waddler_entity_type_in_slot(i)
                if type ~= 0 then
                    waddler_remove_entity(type, 99)
                    spawn_critical(type, spot.x+0.5, spot.y+1, LAYER.BACK, 0, 0)
                end
            end
            --prinspect("Storage at", spot.x, spot.y)
        end
    end
end, ON.POST_LEVEL_GENERATION)

--[[STUFF]]
local ending_timer = 0
local ending_cb = -1
set_callback(function()
    if state.theme == THEME.TIAMAT and bosses_left() > 0 then
        ending_timer = 0
        ending_cb = set_global_interval(function()
            ending_timer = ending_timer + 1
            if ending_timer == 768 then
                for i,p in pairs(players) do
                    p:remove_powerup(ENT_TYPE.ITEM_POWERUP_ANKH)
                    p:light_on_fire()
                end
            elseif ending_timer == 930 then
                for i,p in pairs(players) do
                    kill_entity(p.uid)
                end
                clear_callback(ending_cb)
            end
            local ships = get_entities_by_type(ENT_TYPE.ITEM_PARENTSSHIP)
            if #ships > 0 then
                local ship = get_entity(ships[1])
                ship.x = 35
            end
        end, 1)
    end
end, ON.WIN)

register_option_bool("status", "Show level progress", true)
FakeWorld = 1
FakeLevel = 1
LevelNum = 1
set_callback(function()
    if not options.status then return end
    local worldcount = 1
    local levelcount = 1
    for i,v in ipairs(level_order) do
        if v.b and state.level_count+1 > i then
            levelcount = 1
            worldcount = worldcount + 1
        elseif state.level_count+1 == i then
            FakeWorld = worldcount
            FakeLevel = levelcount
        else
            levelcount = levelcount + 1
        end
    end
    LevelNum = state.level_count+1
end, ON.LEVEL)

set_callback(function(ctx)
    if options.status and state.screen >= ON.LEVEL and state.level_count+1 <= #level_order and state.loading == 0 then
        --ctx:draw_text(-0.065, 0.98, 28, F"{FakeWorld}-{FakeLevel}   {LevelNum}/{#level_order}", 0x44FFFFFF)
        ctx:draw_text(0.9, 0.81, 32, F"{LevelNum}/{#level_order}", 0xBBFFFFFF)
    end
end, ON.GUIFRAME)
