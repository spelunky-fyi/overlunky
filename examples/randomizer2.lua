meta.name = "Randomizer Two"
meta.description = [[THIS REQUIRES 'PLAYLUNKY VERSION > NIGHTLY' (IN MODLUNKY) IF YOU GET ANY RED ERRORS AT THE INTRO, OR THE BLEEDING EDGE FEATURES WON'T WORK!

Fair, balanced, beginner friendly... These are not words I would use to describe The Randomizer. Fun though? Abso-hecking-lutely.

Second incarnation of The Randomizer with new API shenannigans. Most familiar things from 1.2 are still there, but better! Progression is changed though, shops are random, level gen is crazy, chain item stuff, multiple endings, secrets... I can't possibly test all of this so fingers crossed it doesn't crash a lot.]]
meta.version = "2.6a"
meta.author = "Dregu"

--[[OPTIONS]]
local real_default_options = {
    tilecode = true,
    trap = true,
    trap_max = 4,
    trap_min = 2,
    enemy = true,
    enemy_max = 12,
    enemy_min = 5,
    enemy_curse_chance = 5,
    friend = true,
    friend_evil = true,
    friend_max = 1.5,
    friend_min = 0.2,
    room_shop_chance = 15,
    room_big_chance = 25,
    room_big_minx = 2,
    room_big_maxx = 6,
    room_big_miny = 2,
    room_big_maxy = 12,
    room_dark = 4,
    pot_chance = 25,
    treasure_chance = 40,
    ushabti_chance = 25,
    stats_health_min = 8,
    stats_bombs_min = 8,
    stats_ropes_min = 8,
    stats_health_max = 20,
    stats_bombs_max = 20,
    stats_ropes_max = 20,
    chain = true,
    door = true,
    door_min_levels = 3,
    door_max_levels = 6,
    door_bosses = 4,
    door_transitions = false,
    projectile = true,
    storage = true,
    status = true,
    hard = true,
    hard_abzu = true,
    hard_duat = true,
    hard_hundun = true,
    hard_olmec = true,
    hard_olmec_phase = true,
    hard_tiamat = true,
    shop = true,
    liquid_chance = 33,
    trap_spikes = 25,
    trap_fields = 50,
    bias_01 = 15,
    bias_02 = 15,
    bias_03 = 15,
    bias_05 = 15,
    bias_06 = 15,
    bias_07 = 10,
    bias_08 = 12,
    bias_09 = 8,
    bias_10 = 10,
    bias_11 = 6,
    bias_15 = 4,
    drill = 20,
    kali = true,
    jellyless_chance = 40
}
local default_options = table.unpack({real_default_options})
local function register_options()
    register_option_bool("tilecode", "Random floor styles", default_options.tilecode)
    register_option_bool("trap", "Random traps", default_options.trap)
    register_option_float("trap_max", "Max trap chance", default_options.trap_max, 0, 100)
    register_option_float("trap_min", "Min trap chance", default_options.trap_min, 0, 100)
    register_option_bool("enemy", "Random enemies", default_options.enemy)
    register_option_float("enemy_max", "Max enemy chance", default_options.enemy_max, 0, 100)
    register_option_float("enemy_min", "Min enemy chance", default_options.enemy_min, 0, 100)
    register_option_float("enemy_curse_chance", "Enemy handicap chance", default_options.enemy_curse_chance, 0, 100)
    register_option_bool("friend", "Random friends", default_options.friend)
    register_option_bool("friend_evil", "Evil doppelgängers", default_options.friend_evil)
    register_option_float("friend_max", "Max friend chance", default_options.friend_max, 0, 100)
    register_option_float("friend_min", "Min friend chance", default_options.friend_min, 0, 100)
    register_option_float("room_shop_chance", "Extra shop chance", default_options.room_shop_chance, 0, 100)
    register_option_float("room_big_chance", "Level size chance", default_options.room_big_chance, 0, 100)
    register_option_int("room_big_minx", "Level min width", default_options.room_big_minx, 2, 8)
    register_option_int("room_big_maxx", "Level max width", default_options.room_big_maxx, 2, 8)
    register_option_int("room_big_miny", "Level min height", default_options.room_big_miny, 2, 15)
    register_option_int("room_big_maxy", "Level max height", default_options.room_big_maxy, 2, 15)
    register_option_float("room_dark", "Dark level chance", default_options.room_dark, 0, 100)
    register_option_float("pot_chance", "Pot contents chance", default_options.pot_chance, 0, 100)
    --register_option_float("treasure_chance", "Treasure chance", default_options.treasure_chance, 0, 100)
    register_option_float("ushabti_chance", "Correct ushabti chance", default_options.ushabti_chance, 0, 100)
    register_option_int("stats_health_min", "Min starting health", default_options.stats_health_min, 4, 99)
    register_option_int("stats_bombs_min", "Min starting bombs", default_options.stats_bombs_min, 4, 99)
    register_option_int("stats_ropes_min", "Min starting ropes", default_options.stats_ropes_min, 4, 99)
    register_option_int("stats_health_max", "Max starting health", default_options.stats_health_max, 4, 99)
    register_option_int("stats_bombs_max", "Max starting bombs", default_options.stats_bombs_max, 4, 99)
    register_option_int("stats_ropes_max", "Max starting ropes", default_options.stats_ropes_max, 4, 99)
    register_option_bool("chain", "Random chain/quest/challenge items", default_options.chain)
    register_option_bool("shop", "Random shop items", default_options.shop)
    register_option_bool("door", "Random level/boss order", default_options.door)
    register_option_int("door_min_levels", "Min levels between midbosses", default_options.door_min_levels, 1, 100)
    register_option_int("door_max_levels", "Max levels between midbosses", default_options.door_max_levels, 1, 100)
    register_option_int("door_bosses", "Amount of midbosses", default_options.door_bosses, 0, 4)
    register_option_bool("door_transitions", "Neat transitions (maybe crashy)", default_options.door_transitions)
    register_option_bool("projectile", "Random projectiles", default_options.projectile)
    --register_option_bool("storage", "Random Waddler caches", default_options.storage) --TODO
    register_option_bool("status", "Show level progress", default_options.status)
    register_option_bool("hard_abzu", "Hard bosses: Abzu", default_options.hard_abzu)
    register_option_bool("hard_duat", "Hard bosses: Duat", default_options.hard_duat)
    register_option_bool("hard_hundun", "Hard bosses: Hundun", default_options.hard_hundun)
    register_option_bool("hard_olmec", "Hard bosses: Olmec projectiles", default_options.hard_olmec)
    register_option_bool("hard_olmec_phase", "Hard bosses: Olmec phases", default_options.hard_olmec_phase)
    register_option_bool("hard_tiamat", "Hard bosses: Tiamat", default_options.hard_tiamat)
    register_option_float("liquid_chance", "Swap liquid chance", default_options.liquid_chance, 0, 100)
    register_option_float("trap_spikes", "Replace spikes chance", default_options.trap_spikes, 0, 100)
    register_option_float("trap_fields", "Replace forcefields chance", default_options.trap_fields, 0, 100)
    register_option_int("bias_01", "Theme bias: Dwelling", default_options.bias_01, 0, 15)
    register_option_int("bias_02", "Theme bias: Jungle", default_options.bias_02, 0, 15)
    register_option_int("bias_03", "Theme bias: Volcana", default_options.bias_03, 0, 15)
    register_option_int("bias_05", "Theme bias: Tide Pool", default_options.bias_05, 0, 15)
    register_option_int("bias_06", "Theme bias: Temple", default_options.bias_06, 0, 15)
    register_option_int("bias_07", "Theme bias: Ice Caves", default_options.bias_07, 0, 15)
    register_option_int("bias_08", "Theme bias: Neo Babylon", default_options.bias_08, 0, 15)
    register_option_int("bias_09", "Theme bias: Sunken City", default_options.bias_09, 0, 15)
    register_option_int("bias_10", "Theme bias: Cosmic Ocean", default_options.bias_10, 0, 15)
    register_option_int("bias_11", "Theme bias: City of Gold", default_options.bias_11, 0, 15)
    register_option_int("bias_15", "Theme bias: Eggplant World", default_options.bias_15, 0, 15)
    register_option_int("drill", "Drill chance (x2 in echoes)", default_options.drill, 0, 100)
    register_option_bool("kali", "Random kali items", default_options.kali)
    register_option_float("jellyless_chance", "Easy CO chance (no jelly)", default_options.jellyless_chance, 0, 100)
    register_option_button("_reset", "Reset options to defaults", function()
        default_options = table.unpack({real_default_options})
        register_options()
    end)
end
register_options()

set_callback(function(ctx)
    local save_str = json.encode(options)
    ctx:save(save_str)
end, ON.SAVE)

set_callback(function(ctx)
    local load_str = ctx:load()
    if load_str ~= "" then
        default_options = json.decode(load_str)
    end
    for i,v in pairs(real_default_options) do
        if default_options[i] == nil then
            default_options[i] = v
        end
    end
    register_options()
end, ON.LOAD)

local function get_chance(min, max)
    if max == 0 then return 0 end
    if min == 0 then min = 0.00001 end
    if min > max then min, max = max, min end
    min = math.floor(1/(min/100))
    max = math.floor(1/(max/100))
    local chance = prng:random(max, min)
    if state.theme == THEME.HUNDUN then
        chance = math.floor(chance*1.67)
    end
    return chance
end

local function pick(from, ignore)
    local item = -1
    for i=1,10 do
        item = from[prng:random(#from)]
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

local function tile_key(x, y, l)
    return tostring(math.floor(x)).."-"..tostring(math.floor(y)).."-"..tostring(l)
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
local floor_types = {ENT_TYPE.FLOOR_GENERIC, ENT_TYPE.FLOOR_JUNGLE, ENT_TYPE.FLOORSTYLED_MINEWOOD, ENT_TYPE.FLOORSTYLED_STONE, ENT_TYPE.FLOORSTYLED_TEMPLE, ENT_TYPE.FLOORSTYLED_PAGODA, ENT_TYPE.FLOORSTYLED_BABYLON, ENT_TYPE.FLOORSTYLED_SUNKEN, ENT_TYPE.FLOORSTYLED_BEEHIVE, ENT_TYPE.FLOORSTYLED_VLAD, ENT_TYPE.FLOORSTYLED_MOTHERSHIP, ENT_TYPE.FLOORSTYLED_DUAT, ENT_TYPE.FLOORSTYLED_PALACE, ENT_TYPE.FLOORSTYLED_GUTS, ENT_TYPE.FLOOR_SURFACE}
local floor_tilecodes = {floor=-1, minewood_floor=-1, stone_floor=-1, pagoda_floor=-1, babylon_floor=-1, beehive_floor=-1, cog_floor=-1, duat_floor=-1, sunken_floor=-1, icefloor=-1, palace_floor=-1, temple_floor=-1, vlad_floor=-1, shop_wall=-1, pen_floor=-1, shop_sign=-1}

--[[TRAPS]]
local traps_ceiling = {ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_FACTORY_GENERATOR, ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_FACTORY_GENERATOR, ENT_TYPE.FLOOR_SHOPKEEPER_GENERATOR}
local traps_floor = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_TIMED_FORCEFIELD, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP, ENT_TYPE.ACTIVEFLOOR_ELEVATOR}
local traps_wall = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_LASER_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP}
local traps_flip = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_LASER_TRAP}
local traps_generic = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
local traps_item = {ENT_TYPE.FLOOR_SPRING_TRAP, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SNAP_TRAP, ENT_TYPE.ACTIVEFLOOR_POWDERKEG}
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
    if (map(x, y-1) & MASK.ACTIVEFLOOR) > 0 then return false end
    local rx, ry = get_room_index(x, y)
    if y == state.level_gen.spawn_y and (ry >= state.level_gen.spawn_room_y and ry <= state.level_gen.spawn_room_y-1) then return false end
    local box = AABB:new()
    box.left = x-1
    box.right = x+1
    box.top = y-1
    box.bottom = y-2
    local air = get_entities_overlapping_hitbox(0, MASK.FLOOR | MASK.ACTIVEFLOOR, box, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 and #air == 0 then
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
    if prng:random() < 0.04 or (state.theme == THEME.ABZU and prng:random() < 0.06) then
        floor = spawn_grid_entity(ENT_TYPE.FLOOR_EXCALIBUR_STONE, x, y, l)
        spawn_entity_over(ENT_TYPE.ITEM_EXCALIBUR, floor, 0, 0)
        return
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
    local floor = get_grid_entity_at(x, y-1, l)
    local box = AABB:new()
    box.left = x-1
    box.right = x+1
    box.top = y+1
    box.bottom = y
    local air = get_entities_overlapping_hitbox(0, MASK.FLOOR | MASK.ACTIVEFLOOR, box, l)
    local left = get_grid_entity_at(x-1, y-1, l)
    local right = get_grid_entity_at(x+1, y-1, l)
    if floor ~= -1 and #air == 0 and left ~= -1 and right ~= -1 then
        floor = get_entity(floor)
        left = get_entity(left)
        right = get_entity(right)
        return has(valid_floors, floor.type.id) and has(valid_floors, left.type.id) and has(valid_floors, right.type.id)
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
    if options.trap then
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
    end
end, ON.POST_ROOM_GENERATION)

local forced_tiles = {}
set_callback(function()
    level_map = {}
    forced_tiles = {}
end, ON.PRE_LEVEL_GENERATION)

set_post_entity_spawn(function(ent)
    if state.theme == THEME.DUAT then
        kill_entity(ent.uid)
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_SNAP_TRAP)

local swapping_liquid = false
local duat_spawn_x = -1
local duat_spawn_y = -1

set_callback(function()
    if state.theme ~= THEME.DUAT then return end
    duat_spawn_x = state.level_gen.spawn_x
    duat_spawn_y = state.level_gen.spawn_y
    state.level_gen.spawn_x = 17
    state.level_gen.spawn_y = 106
    if state.level_gen.spawn_y < 47 or not options.hard_duat then return end
    local box = AABB:new()
    box.top = duat_spawn_y - 2
    box.bottom = duat_spawn_y - 3
    box.left = duat_spawn_x - 5
    box.right = duat_spawn_x + 5
    local floor = get_entities_overlapping_hitbox(0, MASK.FLOOR | MASK.ACTIVEFLOOR, box, LAYER.FRONT)
    for i,v in ipairs(floor) do
        kill_entity(v)
    end
    set_interval(function()
        if state.theme ~= THEME.DUAT then return false end
        if #get_entities_by(0, MASK.LIQUID, LAYER.BOTH) <= 1180 and #get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR) == 0 then
            local liquid_type = ENT_TYPE.LIQUID_LAVA
            if swapping_liquid then
                liquid_type = ENT_TYPE.LIQUID_WATER
            end
            spawn_liquid(liquid_type, duat_spawn_x, duat_spawn_y - 2)
        elseif #get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR) == 0 then
            spawn(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR, 17.5, 36, LAYER.FRONT, 0, 0)
            spawn(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR, 2.5, 36, LAYER.FRONT, 0, 0)
            spawn(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR, 32.5, 36, LAYER.FRONT, 0, 0)
            return false
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

local enemies_small = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER,
    ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
    ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_MANTRAP, ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_WITCHDOCTOR,
    ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.MONS_ROBOT,
    ENT_TYPE.MONS_FIREBUG_UNCHAINED,
    ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS,
    ENT_TYPE.MONS_CATMUMMY, ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI,
    ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_ALIEN,
    ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER,
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
local crab_items = {ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK, ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.ITEM_CHEST, ENT_TYPE.ITEM_VAULTCHEST, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_PRESENT, ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_EGGPLANT, ENT_TYPE.ITEM_IDOL, ENT_TYPE.ITEM_KEY, ENT_TYPE.ITEM_SNAP_TRAP, ENT_TYPE.ITEM_LAVAPOT, ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_SCRAP, ENT_TYPE.ITEM_SKULL, ENT_TYPE.ITEM_TV, ENT_TYPE.ITEM_USHABTI, ENT_TYPE.MONS_YANG, ENT_TYPE.ITEM_ICESPIRE}
local friends = {ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_MARGARET_TUNNEL, ENT_TYPE.CHAR_COLIN_NORTHWARD, ENT_TYPE.CHAR_ROFFY_D_SLOTH, ENT_TYPE.CHAR_BANDA, ENT_TYPE.CHAR_GREEN_GIRL, ENT_TYPE.CHAR_AMAZON, ENT_TYPE.CHAR_LISE_SYSTEM, ENT_TYPE.CHAR_COCO_VON_DIAMONDS, ENT_TYPE.CHAR_MANFRED_TUNNEL, ENT_TYPE.CHAR_OTAKU, ENT_TYPE.CHAR_TINA_FLAN, ENT_TYPE.CHAR_VALERIE_CRUMP, ENT_TYPE.CHAR_AU, ENT_TYPE.CHAR_DEMI_VON_DIAMONDS, ENT_TYPE.CHAR_PILOT, ENT_TYPE.CHAR_PRINCESS_AIRYN, ENT_TYPE.CHAR_DIRK_YAMAOKA, ENT_TYPE.CHAR_GUY_SPELUNKY, ENT_TYPE.CHAR_CLASSIC_GUY, ENT_TYPE.CHAR_HIREDHAND, ENT_TYPE.CHAR_EGGPLANT_CHILD}
local enemies_challenge = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER,
    ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
    ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_MANTRAP, ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_WITCHDOCTOR, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.MONS_FIREBUG_UNCHAINED,
    ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS, ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI,
    ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_ALIEN,
    ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER,
    ENT_TYPE.MONS_OLMITE_HELMET, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED, ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG, ENT_TYPE.MONS_LEPRECHAUN}
local enemies_sisters = {ENT_TYPE.MONS_SISTER_PARSLEY, ENT_TYPE.MONS_SISTER_PARSNIP, ENT_TYPE.MONS_SISTER_PARMESAN}
local enemies_generator = {ENT_TYPE.MONS_PROTOSHOPKEEPER, ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_ALIEN, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_TIKIMAN}

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local divider = 100
    if test_flag(state.level_flags, 18) then
        divider = 75
    end
    divider = divider - (state.height + state.width)*2
    if state.time_level < 2 and prng:random() < options.jellyless_chance/divider then
        return spawn_critical(ENT_TYPE.FX_SHADOW, x, y, l, 0, 0)
    end
    return spawn_critical(type, x, y, l, 0, 0)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_MEGAJELLYFISH)

local function enemy_small_spawn(x, y, l)
    local enemy = pick(enemies_small)
    if prng:random() < 0.03 or (state.theme == THEME.JUNGLE and prng:random() < 0.08) then
        enemy = pick(enemies_sisters)
    end
    local uid = spawn_entity_snapped_to_floor(enemy, x, y, l)
    local ent = get_entity(uid)
    if prng:random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
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
    if state.theme == THEME.TIAMAT then return false end
    local air = get_grid_entity_at(x, y, l)
    return air == -1
end
local enemy_air_chance = define_procedural_spawn("enemy_air", enemy_air_spawn, enemy_air_valid)

local friend_spawned = false
local function friend_spawn(x, y, l)
    if not friend_spawned then
        local uid = spawn_companion(pick(friends), x, y, l)
        local ent = get_entity(uid)
        if prng:random() < options.enemy_curse_chance/100 then
            attach_ball_and_chain(uid, 0.5, 0)
        end
        friend_spawned = true
    end
end
local function friend_valid(x, y, l)
    local floor = get_grid_entity_at(x, y-1, l)
    local air = get_grid_entity_at(x, y, l)
    if floor ~= -1 and air == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local friend_chance = define_procedural_spawn("friend", friend_spawn, friend_valid)

local function snowman_spawn(x, y, l)
    local uid = spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_ICESPIRE, x, y, l, 0, 0);
    local ent = get_entity(uid)
    if prng:random() < 0.5 then
        flip_entity(uid)
    end
    ent.animation_frame = 221
    ent.offsety = ent.offsety + 0.05
end
local function snowman_valid(x, y, l)
    local floor = get_grid_entity_at(x, y-1, l)
    local air = get_grid_entity_at(x, y, l)
    if floor ~= -1 and air == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local snowman_chance = define_procedural_spawn("snowman", snowman_spawn, snowman_valid)
set_post_entity_spawn(function(ent)
    if ent.overlay and ent.overlay.type.id == ENT_TYPE.MONS_HERMITCRAB then
        ent.animation_frame = 221
        ent.offsety = ent.offsety + 0.05
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_ICESPIRE)

set_callback(function()
    if options.friend and options.friend_evil then
        set_interval(function()
            for i,p in ipairs(players) do
                for j,v in ipairs(get_entities_by_type(p.type.id)) do
                    local ent = get_entity(v)
                    if ent.ai ~= nil then
                        ent.ai.state = 8
                        ent.ai.target_uid = p.uid
                        --ent:set_cursed(true)
                        ent.color.g = 0.3
                        ent.color.b = 0.3
                        if p.stun_timer > 0 and ent.stun_timer == 0 then
                            ent:stun(p.stun_timer + 15)
                        end
                    end
                end
            end
        end, 15)
    end
end, ON.LEVEL)

set_callback(function(ctx)
    if options.enemy then
        ctx:set_procedural_spawn_chance(enemy_small_chance, get_chance(options.enemy_min, options.enemy_max))
        ctx:set_procedural_spawn_chance(enemy_big_chance, get_chance(options.enemy_min, options.enemy_max) * 6)
        ctx:set_procedural_spawn_chance(enemy_climb_chance, get_chance(options.enemy_min, options.enemy_max) * 3)
        ctx:set_procedural_spawn_chance(enemy_ceiling_chance, get_chance(options.enemy_min, options.enemy_max) * 3)
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

        change_sunchallenge_spawns(enemies_challenge)
        change_altar_damage_spawns(enemies_challenge)
        if prng:random() < 0.007 then
            replace_drop(DROP.SHOPKEEPER_GENERATOR_1, ENT_TYPE.MONS_GOLDMONKEY)
        else
            replace_drop(DROP.SHOPKEEPER_GENERATOR_1, pick(enemies_generator))
        end
    end

    if options.friend then
        ctx:set_procedural_spawn_chance(friend_chance, get_chance(options.friend_min, options.friend_max))
        if state.items.player_inventory[1].companion_count > 0 then
            friend_spawned = true
        else
            friend_spawned = false
        end
    end

    ctx:set_procedural_spawn_chance(snowman_chance, get_chance(options.enemy_min, options.enemy_max) * 10)
end, ON.POST_ROOM_GENERATION)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.MONS_REDSKELETON)

set_post_entity_spawn(function(ent)
    if state.theme == THEME.NEO_BABYLON and state.level == 2 then return end --ushabti != 100
    ent.carried_entity_type = pick(crab_items)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_HERMITCRAB)

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.ABZU or not options.hard_abzu then return end
    local x, y, l = get_position(ent.uid)
    if y > 105 then
        if prng:random() < 0.6 then ent:destroy() end
        spawn_entity_nonreplaceable(pick(enemies_kingu), x, y, l, prng:random()*0.3-0.15, prng:random()*0.1+0.1)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, {ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI, ENT_TYPE.MONS_OCTOPUS})

--[[TODO: everything spawns behind the shell and some enemies crash
set_post_entity_spawn(function(ent)
    set_interval(function()
        replace_drop(DROP.KINGU_FEMALE_JIANGSHI, pick(enemies_kingu))
        replace_drop(DROP.KINGU_JIANGSHI, pick(enemies_kingu))
        replace_drop(DROP.KINGU_OCTOPUS, pick(enemies_kingu))
    end, 30)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_KINGU)
]]

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.OLMEC or not options.hard_olmec then
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
    if state.theme ~= THEME.OLMEC or not options.hard_olmec then return end
    local x, y, l = get_position(ent.uid)
    local players = get_entities_at(0, MASK.PLAYER, x, y, l, 0.5)
    if #players > 0 then return end
    local newent = get_entity(spawn_entity_nonreplaceable(pick(olmec_ammo), x, y, l, 0, 0))
    if newent.stun_timer ~= nil then
        newent.stun_timer = 15
        set_timeout(function()
            newent.velocityx = prng:random()*0.66-0.33
            newent.velocityy = prng:random()*0.1+0.1
        end, 1)
    end
    ent:destroy()
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
    if state.theme ~= THEME.TIAMAT or not options.hard_tiamat then return end
    set_timeout(tiamat_scream, 60)
    set_interval(tiamat_attack, 2)
end, ON.LEVEL)

local hundun_fireball_timer = -1
local hundun_
set_callback(function()
    if state.theme ~= THEME.HUNDUN or not options.hard_hundun then return end
    for i,v in ipairs(get_entities_by_type(ENT_TYPE.MONS_HUNDUN)) do
        local x, y, l = get_position(v)
        move_entity(v, x, 140, 0, 0)
        set_timeout(function()
            move_entity(v, x, y, 0, 0)
            set_post_statemachine(v, function(ent)
                ent.hundun_flags = set_flag(ent.hundun_flags, 2)
                ent.hundun_flags = set_flag(ent.hundun_flags, 3)
                ent.hundun_flags = set_flag(ent.hundun_flags, 4)
                if ent.fireball_timer > 120 then
                    ent.fireball_timer = prng:random(45, 120)
                end
            end)
        end, 1)
    end
end, ON.LEVEL)

--[[ROOMS]]

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
local big_level = false
local new_width = 4
local new_height = 4

set_callback(function()
    if state.screen_next ~= SCREEN.LEVEL then return end
    big_level = false
    if ((state.width == 4 and state.height == 4) or (state.width == 4 and state.height == 5 and state.theme == THEME.SUNKEN_CITY) or state.theme == THEME.CITY_OF_GOLD or state.theme == THEME.COSMIC_OCEAN) and prng:random() < options.room_big_chance/100 then
        local capminx = 2
        local capmaxx = 8
        local capminy = 2
        local capmaxy = 15
        if state.theme == THEME.COSMIC_OCEAN then
            capminx = 4
            capminy = 3
        elseif state.theme == THEME.CITY_OF_GOLD then
            capminx = 2
            capmaxx = 8
            capminy = 4
        end
        local minx = math.max(capminx, options.room_big_minx)
        local maxx = math.min(capmaxx, options.room_big_maxx)
        local miny = math.max(capminy, options.room_big_miny)
        local maxy = math.min(capmaxy, options.room_big_maxy)
        if minx > maxx then minx = maxx end
        if miny > maxy then miny = maxy end
        if maxx < capminx then maxx = capminx end
        if maxy < capminy then maxy = capminy end
        new_width = prng:random(minx, maxx)
        new_height = prng:random(miny, maxy)
        if new_width == 2 and new_height == 2 then
            if prng:random() < 0.5 then
                new_width = 4
            else
                new_height = 4
            end
        elseif new_width >= 7 and new_height >= 8 then
            if prng:random() < 0.5 then
                new_height = 5
            elseif prng:random() < 0.5 then
                new_width = 5
            end
        end
        local realx = new_width
        local realy = new_height
        if state.theme == THEME.COSMIC_OCEAN then
            realx = realx - 2
            realy = realy - 2
        end
        if (state.level == 1 and state.theme ~= THEME.VOLCANA and state.theme ~= THEME.SUNKEN_CITY) or (new_width >= 4 and new_height >= 4) then
            state.width = new_width
            state.height = new_height
            big_level = true
        end
    end
end, ON.PRE_LEVEL_GENERATION)

set_callback(function(ctx)
    if state.screen ~= SCREEN.LEVEL then return end
    local exit_x = 1
    local exit_y = 1
    if big_level and state.theme == THEME.COSMIC_OCEAN then
        state.width = new_width
        state.height = new_height
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
    if big_level then
        if state.theme == THEME.COSMIC_OCEAN then
            if (state.width-2) * (state.height-2) >= 24 then
                toast("My voice would probably echo in here,\nbut in space no one can hear you scream.")
            end
        elseif state.width * state.height >= 28 then
            toast("My voice REALLY echoes in here!")
        end
    end
end, ON.POST_ROOM_GENERATION)

set_callback(function()
    if test_flag(state.level_flags, 18) then
        for i,v in ipairs(get_entities_by_type(ENT_TYPE.ITEM_FLOATING_ORB)) do
            local x, y, l = get_position(v)
            local light = get_entity(spawn(ENT_TYPE.LOGICAL_ROOM_LIGHT, x, y, l, 0, 0))
            light.illumination.light1.green = 0
            light.illumination.light1.size = 2
        end
    end
end, ON.LEVEL)

--[[SHOPS]]
local shop_items = {ENT_TYPE.ITEM_PICKUP_ROPEPILE, ENT_TYPE.ITEM_PICKUP_BOMBBAG, ENT_TYPE.ITEM_PICKUP_BOMBBOX, ENT_TYPE.ITEM_PICKUP_PARACHUTE, ENT_TYPE.ITEM_PICKUP_SPECTACLES, ENT_TYPE.ITEM_PICKUP_SKELETON_KEY, ENT_TYPE.ITEM_PICKUP_COMPASS, ENT_TYPE.ITEM_PICKUP_SPRINGSHOES, ENT_TYPE.ITEM_PICKUP_SPIKESHOES, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_PICKUP_PITCHERSMITT, ENT_TYPE.ITEM_PICKUP_CLIMBINGGLOVES, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_MACHETE, ENT_TYPE.ITEM_BOOMERANG, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_MATTOCK, ENT_TYPE.ITEM_TELEPORTER, ENT_TYPE.ITEM_FREEZERAY, ENT_TYPE.ITEM_METAL_SHIELD, ENT_TYPE.ITEM_PURCHASABLE_CAPE, ENT_TYPE.ITEM_PURCHASABLE_HOVERPACK, ENT_TYPE.ITEM_PURCHASABLE_TELEPORTER_BACKPACK, ENT_TYPE.ITEM_PURCHASABLE_POWERPACK, ENT_TYPE.ITEM_PURCHASABLE_JETPACK, ENT_TYPE.ITEM_PRESENT, ENT_TYPE.ITEM_PICKUP_HEDJET, ENT_TYPE.ITEM_PICKUP_ROYALJELLY, ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_SKULL, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY}
local extra_shop_items = {ENT_TYPE.ITEM_LIGHT_ARROW, ENT_TYPE.ITEM_PICKUP_GIANTFOOD, ENT_TYPE.ITEM_PICKUP_ELIXIR, ENT_TYPE.ITEM_PICKUP_CLOVER, ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, ENT_TYPE.ITEM_PICKUP_UDJATEYE, ENT_TYPE.ITEM_PICKUP_UDJATEYE, ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_CROWN, ENT_TYPE.ITEM_PICKUP_EGGPLANTCROWN, ENT_TYPE.ITEM_PICKUP_TRUECROWN, ENT_TYPE.ITEM_PICKUP_ANKH, ENT_TYPE.ITEM_CLONEGUN, ENT_TYPE.ITEM_HOUYIBOW, ENT_TYPE.ITEM_WOODEN_SHIELD, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SNAP_TRAP}
local all_shop_items = join(shop_items, extra_shop_items)
local shop_guns = {ENT_TYPE.ITEM_SHOTGUN, ENT_TYPE.ITEM_PLASMACANNON, ENT_TYPE.ITEM_FREEZERAY, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_CROSSBOW}
local extra_shop_guns = {ENT_TYPE.ITEM_CLONEGUN}
local all_shop_guns = join(shop_guns, extra_shop_guns)
local shop_mounts = {ENT_TYPE.MOUNT_AXOLOTL, ENT_TYPE.MOUNT_MECH, ENT_TYPE.MOUNT_QILIN, ENT_TYPE.MOUNT_ROCKDOG, ENT_TYPE.MOUNT_TURKEY}
local shop_rooms = {ROOM_TEMPLATE.SHOP, ROOM_TEMPLATE.SHOP_LEFT, ROOM_TEMPLATE.CURIOSHOP, ROOM_TEMPLATE.CURIOSHOP_LEFT, ROOM_TEMPLATE.CAVEMANSHOP, ROOM_TEMPLATE.CAVEMANSHOP_LEFT}
local shop_replace_rooms = {ROOM_TEMPLATE.SHOP, ROOM_TEMPLATE.SHOP_LEFT, ROOM_TEMPLATE.CAVEMANSHOP, ROOM_TEMPLATE.CAVEMANSHOP_LEFT}
local shop_names = {
    [ENT_TYPE.ITEM_LIGHT_ARROW] = "Light Arrow",
    [ENT_TYPE.ITEM_PICKUP_GIANTFOOD] = "Vegan Face Meat",
    [ENT_TYPE.ITEM_PICKUP_ELIXIR] = "Elixir",
    [ENT_TYPE.ITEM_PICKUP_CLOVER] = "Clover",
    [ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS] = "Alien Compass",
    [ENT_TYPE.ITEM_PICKUP_UDJATEYE] = "Udjat Eye",
    [ENT_TYPE.ITEM_PICKUP_KAPALA] = "Kapala",
    [ENT_TYPE.ITEM_PICKUP_CROWN] = "Crown",
    [ENT_TYPE.ITEM_PICKUP_EGGPLANTCROWN] = "Eggplant Crown",
    [ENT_TYPE.ITEM_PICKUP_TRUECROWN] = "True Crown",
    [ENT_TYPE.ITEM_PICKUP_ANKH] = "Ankh",
    [ENT_TYPE.ITEM_CLONEGUN] = "Clone Gun",
    [ENT_TYPE.ITEM_HOUYIBOW] = "Hawkeye Bow",
    [ENT_TYPE.ITEM_WOODEN_SHIELD] = "TOTALLY METAL SHIELD",
    [ENT_TYPE.ITEM_LANDMINE] = "Who put this here!?",
    [ENT_TYPE.ITEM_SNAP_TRAP] = "Who put this here?!",
    [ENT_TYPE.MOUNT_QILIN] = "Updog"
}
local wrong_stringid = 1948

set_callback(function()
    for i,v in ipairs(shop_names) do
        local etype = get_type(i)
        if etype.description == wrong_stringid or etype.description == wrong_stringid+1 or get_string(etype.description) ~= v then
            etype.description = add_string(v)
        end
    end
end, ON.START)

set_callback(function()
    local in_shop = {}
    local items = get_entities_by(0, MASK.ITEM | MASK.MOUNT | MASK.PLAYER | MASK.MONSTER, LAYER.BOTH)
    for i,v in ipairs(items) do
        local e = get_entity(v)
        if test_flag(e.flags, ENT_FLAG.SHOP_ITEM) then
            in_shop[#in_shop+1] = e
        end
    end
    for i,v in ipairs(in_shop) do
        v.price = prng:random(1000, math.min(20000, 2*get_money())+prng:random(1500, 6000))
        if prng:random(100) == 1 then
            v.price = 0
        end
    end
end, ON.POST_LEVEL_GENERATION)

set_post_entity_spawn(function(ent)
    set_timeout(function()
        ent.price = prng:random(1000, math.min(10000, 2*get_money())+prng:random(1000, 2000))
    end, 1)
    set_interval(function()
        if state.logic.diceshop == nil then
            return false
        end
        if state.logic.diceshop.bet_active == true then
            for i,v in ipairs(get_entities_by_type(ENT_TYPE.ITEM_DICE_BET)) do
                local bet = get_entity(v)
                bet.price = prng:random(1000, math.min(10000, 2*get_money())+prng:random(1500, 3000))
            end
        end
    end, 15)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_DICE_BET)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) and options.shop then
        local eid = pick(all_shop_items)
        local etype = get_type(eid)
        if etype.description == wrong_stringid or etype.description == wrong_stringid+1 then
            if shop_names[etype.id] ~= nil then
                etype.description = add_string(shop_names[etype.id])
            else
                etype.description = prng:random(1804, 1858)
            end
        end
        return spawn_entity_nonreplaceable(eid, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, MASK.ITEM, shop_items)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_replace_rooms, roomtype) and options.shop then
        local eid = pick(shop_mounts)
        local etype = get_type(eid)
        if etype.description == wrong_stringid or etype.description == wrong_stringid+1 then
            if shop_names[etype.id] ~= nil then
                etype.description = add_string(shop_names[etype.id])
            else
                etype.description = prng:random(1804, 1858)
            end
        end
        return spawn_entity_nonreplaceable(eid, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, MASK.MOUNT, shop_mounts)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_replace_rooms, roomtype) and options.shop then
        local item = prng:random(ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_EGGPLANT_CHILD)
        if item == ENT_TYPE.CHAR_CLASSIC_GUY + 1 then
            item = ENT_TYPE.CHAR_EGGPLANT_CHILD
        end
        return spawn_entity_nonreplaceable(item, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.CHAR_HIREDHAND)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_replace_rooms, roomtype) and options.shop then
        local eid = pick(all_shop_guns)
        local etype = get_type(eid)
        if etype.description == wrong_stringid or etype.description == wrong_stringid+1 then
            if shop_names[etype.id] ~= nil then
                etype.description = add_string(shop_names[etype.id])
            else
                etype.description = prng:random(1804, 1858)
            end
        end
        return spawn_entity_nonreplaceable(eid, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, MASK.ITEM, shop_guns)

--[[CONTAINERS]]

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
         ENT_TYPE.MONS_CRITTERDUNGBEETLE,
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
         ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, ENT_TYPE.ITEM_PICKUP_PARACHUTE, ENT_TYPE.ITEM_PICKUP_UDJATEYE, ENT_TYPE.ITEM_PICKUP_UDJATEYE,
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
         ENT_TYPE.MOUNT_ROCKDOG, ENT_TYPE.MOUNT_AXOLOTL, ENT_TYPE.ITEM_PICKUP_24BAG, ENT_TYPE.ITEM_PICKUP_12BAG}
local crate_items = {ENT_TYPE.ITEM_LIGHT_ARROW, ENT_TYPE.ITEM_PRESENT, ENT_TYPE.ITEM_PICKUP_BOMBBOX,
         ENT_TYPE.ITEM_PICKUP_ROYALJELLY, ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY, ENT_TYPE.ITEM_PICKUP_GIANTFOOD,
         ENT_TYPE.ITEM_PICKUP_ELIXIR, ENT_TYPE.ITEM_PICKUP_CLOVER, ENT_TYPE.ITEM_PICKUP_SPECTACLES,
         ENT_TYPE.ITEM_PICKUP_CLIMBINGGLOVES, ENT_TYPE.ITEM_PICKUP_PITCHERSMITT, ENT_TYPE.ITEM_PICKUP_SPRINGSHOES,
         ENT_TYPE.ITEM_PICKUP_SPIKESHOES, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_PICKUP_COMPASS,
         ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, ENT_TYPE.ITEM_PICKUP_PARACHUTE, ENT_TYPE.ITEM_PICKUP_UDJATEYE, ENT_TYPE.ITEM_PICKUP_UDJATEYE,
         ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_HEDJET, ENT_TYPE.ITEM_PICKUP_CROWN,
         ENT_TYPE.ITEM_PICKUP_EGGPLANTCROWN, ENT_TYPE.ITEM_PICKUP_TRUECROWN, ENT_TYPE.ITEM_PICKUP_ANKH,
         ENT_TYPE.ITEM_PICKUP_SKELETON_KEY, ENT_TYPE.ITEM_PICKUP_PLAYERBAG, ENT_TYPE.ITEM_PICKUP_24BAG, ENT_TYPE.ITEM_PICKUP_12BAG, ENT_TYPE.ITEM_CAPE,
         ENT_TYPE.ITEM_VLADS_CAPE, ENT_TYPE.ITEM_JETPACK, ENT_TYPE.ITEM_TELEPORTER_BACKPACK, ENT_TYPE.ITEM_HOVERPACK,
         ENT_TYPE.ITEM_POWERPACK, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_SHOTGUN, ENT_TYPE.ITEM_FREEZERAY,
         ENT_TYPE.ITEM_CROSSBOW, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_TELEPORTER, ENT_TYPE.ITEM_MATTOCK,
         ENT_TYPE.ITEM_BOOMERANG, ENT_TYPE.ITEM_MACHETE, ENT_TYPE.ITEM_EXCALIBUR, ENT_TYPE.ITEM_BROKENEXCALIBUR,
         ENT_TYPE.ITEM_PLASMACANNON, ENT_TYPE.ITEM_SCEPTER, ENT_TYPE.ITEM_CLONEGUN, ENT_TYPE.ITEM_HOUYIBOW,
         ENT_TYPE.ITEM_METAL_SHIELD, ENT_TYPE.ITEM_USHABTI, ENT_TYPE.ITEM_CRATE}
local abzu_crate_items = {ENT_TYPE.ITEM_EXCALIBUR, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_BROKENEXCALIBUR, ENT_TYPE.ITEM_PICKUP_BOMBBOX}
local monkey_crap = join(crate_items, {ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_BOMB, ENT_TYPE.ITEM_BOMB, ENT_TYPE.ITEM_BOMB})
local sister_items = {ENT_TYPE.ITEM_PICKUP_ROPEPILE, ENT_TYPE.ITEM_PICKUP_BOMBBAG, ENT_TYPE.ITEM_PICKUP_BOMBBOX, ENT_TYPE.ITEM_PICKUP_12BAG, ENT_TYPE.ITEM_PICKUP_24BAG, ENT_TYPE.ITEM_PICKUP_ROYALJELLY, ENT_TYPE.ITEM_PICKUP_COOKEDTURKEY, ENT_TYPE.ITEM_PICKUP_GIANTFOOD, ENT_TYPE.ITEM_PICKUP_ELIXIR, ENT_TYPE.ITEM_PICKUP_SPECTACLES, ENT_TYPE.ITEM_PICKUP_CLIMBINGGLOVES, ENT_TYPE.ITEM_PICKUP_PITCHERSMITT, ENT_TYPE.ITEM_PICKUP_SPRINGSHOES, ENT_TYPE.ITEM_PICKUP_SPIKESHOES, ENT_TYPE.ITEM_PICKUP_PASTE, ENT_TYPE.ITEM_PICKUP_COMPASS, ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, ENT_TYPE.ITEM_PICKUP_PARACHUTE, ENT_TYPE.ITEM_PICKUP_UDJATEYE, ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_HEDJET, ENT_TYPE.ITEM_PICKUP_CROWN, ENT_TYPE.ITEM_PICKUP_EGGPLANTCROWN, ENT_TYPE.ITEM_PICKUP_TRUECROWN, ENT_TYPE.ITEM_PICKUP_ANKH, ENT_TYPE.ITEM_PICKUP_TABLETOFDESTINY, ENT_TYPE.ITEM_PICKUP_SKELETON_KEY, ENT_TYPE.ITEM_CAPE, ENT_TYPE.ITEM_WEBGUN, ENT_TYPE.ITEM_SHOTGUN, ENT_TYPE.ITEM_FREEZERAY, ENT_TYPE.ITEM_CROSSBOW, ENT_TYPE.ITEM_CAMERA, ENT_TYPE.ITEM_TELEPORTER, ENT_TYPE.ITEM_MATTOCK, ENT_TYPE.ITEM_BOOMERANG, ENT_TYPE.ITEM_MACHETE, ENT_TYPE.ITEM_EXCALIBUR, ENT_TYPE.ITEM_BROKENEXCALIBUR, ENT_TYPE.ITEM_PLASMACANNON, ENT_TYPE.ITEM_SCEPTER, ENT_TYPE.ITEM_CLONEGUN, ENT_TYPE.ITEM_HOUYIBOW, ENT_TYPE.ITEM_WOODEN_SHIELD, ENT_TYPE.ITEM_SCRAP, ENT_TYPE.ITEM_BOOMBOX}

set_post_entity_spawn(function(ent)
    --math.randomseed(read_prng()[5]+ent.uid)
    if prng:random() < options.pot_chance/100 then
        ent.inside = pick(pot_items)
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_POT)

set_post_entity_spawn(function(ent)
    --math.randomseed(read_prng()[6]+ent.uid)
    if state.theme == THEME.ABZU and prng:random() < 0.75 then
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
    if state.theme == THEME.OLMEC then
        for i,v in ipairs(get_entities_by_type(ENT_TYPE.ITEM_CRATE)) do
            local ent = get_entity(v)
            ent.inside = pick(crate_items)
        end
    end
    change_diceshop_prizes(crate_items)
end, ON.POST_LEVEL_GENERATION)

set_post_entity_spawn(function(ent)
    set_interval(function()
        replace_drop(DROP.GOLDENMONKEY_SMALLRUBY, pick(monkey_crap))
        replace_drop(DROP.GOLDENMONKEY_SMALLSAPPHIRE, pick(monkey_crap))
    end, 60)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_GOLDMONKEY)

--[[STATS]]

set_callback(function()
    --math.randomseed(read_prng()[1])
    for i,p in ipairs(players) do
        local hmin = options.stats_health_min
        local hmax = options.stats_health_max
        local bmin = options.stats_bombs_min
        local bmax = options.stats_bombs_max
        local rmin = options.stats_ropes_min
        local rmax = options.stats_ropes_max
        if hmin > hmax then hmin = hmax end
        if bmin > bmax then bmin = bmax end
        if rmin > bmax then rmin = rmax end
        p.health = prng:random_int(hmin, hmax, 1)
        p.inventory.bombs = prng:random_int(bmin, bmax, 1)
        p.inventory.ropes = prng:random_int(rmin, rmax, 1)
    end
end, ON.START)

--[[DOORS]]

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
    if #players > 0 and options.door then
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
    if not has(bosses_killed, boss) and options.door then
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
    if has(theme, THEME.NEO_BABYLON) then
        insert_chain(THEME.TIAMAT, { w = 6, l = 2, t = THEME.NEO_BABYLON, b = false})
    end
end

local function get_chain_item()
    if #chain_items > 0 then
        return table.remove(chain_items, 1)
    end
    return pick(crate_items)
end

--[[TODO
    HUNDUN_FIREBALL
    TIAMAT_*
]]

set_post_entity_spawn(function(ent)
    replace_drop(DROP.HUMPHEAD_HIREDHAND, pick(friends))
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_GIANTFISH)

set_post_entity_spawn(function(ent)
    replace_drop(DROP.LOCKEDCHEST_UDJATEYE, get_chain_item())
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.LOCKEDCHEST)

set_post_entity_spawn(function(ent)
    set_interval(function()
        replace_drop(DROP.ANUBIS_COFFIN_SORCERESS, pick(enemies_small))
        replace_drop(DROP.ANUBIS_COFFIN_VAMPIRE, pick(enemies_small))
        replace_drop(DROP.ANUBIS_COFFIN_WITCHDOCTOR, pick(enemies_small))
    end, 30)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_ANUBIS2)

set_pre_tile_code_callback(function(x, y, layer)
    spawn_critical(get_chain_item(), x, y, layer, 0, 0)
    return true
end, "ankh")

set_pre_tile_code_callback(function(x, y, layer)
    spawn_critical(pick(crate_items), x, y, layer, 0, 0)
    return true
end, "clover")

set_post_entity_spawn(function(ent)
    if not options.chain then return end
    local x, y, l = get_position(ent.uid)
    local rx, ry = get_room_index(x, y)
    local room = get_room_template(rx, ry, l)
    if l == LAYER.BACK and room == ROOM_TEMPLATE.OLDHUNTER_REWARDROOM then
        replace_drop(DROP.VAN_HORSING_DIAMOND, get_chain_item())
    elseif l == LAYER.BACK and room == ROOM_TEMPLATE.OLDHUNTER_CURSEDROOM then
        replace_drop(DROP.VAN_HORSING_COMPASS, get_chain_item())
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_OLD_HUNTER)

set_post_entity_spawn(function(ent)
    if not options.chain then return end
    local x, y, l = get_position(ent.uid)
    local rx, ry = get_room_index(x, y)
    local room = get_room_template(rx, ry, l)
    if l == LAYER.BACK and room == ROOM_TEMPLATE.QUEST_THIEF1 then
        replace_drop(DROP.SPARROW_ROPEPILE, get_chain_item())
    elseif l == LAYER.BACK and room == ROOM_TEMPLATE.QUEST_THIEF2 then
        replace_drop(DROP.SPARROW_SKELETONKEY, get_chain_item())
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_THIEF)

local yama_food = {ENT_TYPE.ITEM_BOMB, ENT_TYPE.ITEM_LANDMINE}
set_post_entity_spawn(function(ent)
    replace_drop(DROP.YAMA_EGGPLANTCROWN, get_chain_item())
    replace_drop(DROP.YAMA_GIANTFOOD, pick(yama_food))
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_YAMA)

set_post_entity_spawn(function(ent)
    replace_drop(DROP.BEG_BOMBBAG, pick(crate_items))
    replace_drop(DROP.BEG_TRUECROWN, pick(crate_items))
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_HUNDUNS_SERVANT)

set_post_entity_spawn(function(ent)
    replace_drop(DROP.GHOSTJAR_DIAMOND, pick(crate_items))
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_CURSEDPOT)

--[[set_post_entity_spawn(function(ent)
    if not options.chain then return end
    local x, y, l = get_position(ent.uid)
    local rx, ry = get_room_index(x, y)
    local room = get_room_template(rx, ry, l)
    if room == ROOM_TEMPLATE.UDJATTOP then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(), x, y, l, (prng:random()-0.5)*0.2, 0.2)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_PICKUP_UDJATEYE)]]

set_callback(function()
    if not options.chain then return end
    if state.theme == THEME.VOLCANA then
        local crowns = get_entities_by(ENT_TYPE.ITEM_PICKUP_CROWN, 0, LAYER.BACK)
        for i,v in ipairs(crowns) do
            local x, y, l = get_position(v)
            kill_entity(v)
            local item = spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
            item = get_entity(item)
            item.flags = set_flag(item.flags, ENT_FLAG.NO_GRAVITY)
        end
    elseif state.theme == THEME.TIDE_POOL then
        local notes = get_entities_by(ENT_TYPE.ITEM_MADAMETUSK_IDOLNOTE, 0, LAYER.BACK)
        for i,v in ipairs(notes) do
            local x, y, l = get_position(v)
            kill_entity(v)
            local item = spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
        end
    end
    if state.theme == THEME.DUAT then
        replace_drop(DROP.OSIRIS_TABLETOFDESTINY, get_chain_item())
    elseif state.theme == THEME.ABZU then
        replace_drop(DROP.KINGU_TABLETOFDESTINY, get_chain_item())
    end
end, ON.LEVEL)

local items_kapala = {ENT_TYPE.ITEM_PICKUP_KAPALA, ENT_TYPE.ITEM_PICKUP_24BAG, ENT_TYPE.ITEM_PICKUP_ANKH, ENT_TYPE.ITEM_PICKUP_CROWN, ENT_TYPE.ITEM_PICKUP_ELIXIR}
set_callback(function()
    if not options.kali then return end
    set_interval(function()
        replace_drop(DROP.ALTAR_ROCK_WOODENARROW, pick(join(crate_items, enemies_small)))
        replace_drop(DROP.ALTAR_PRESENT_EGGPLANT, pick(orig_chain_items))
        replace_drop(DROP.ALTAR_KAPALA, pick(items_kapala))
    end, 30)
end, ON.LEVEL)

set_post_entity_spawn(function(ent)
    if not options.chain then return end
    local x, y, l = get_position(ent.uid)
    local rx, ry = get_room_index(x, y)
    local room = get_room_template(rx, ry, l)
    if l == LAYER.BACK and state.world == 2 and room == ROOM_TEMPLATE.CHALLENGE_SPECIAL then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_HOUYIBOW)

set_post_entity_spawn(function(ent)
    if not options.chain then return end
    local x, y, l = get_position(ent.uid)
    local rx, ry = get_room_index(x, y)
    local room = get_room_template(rx, ry, l)
    if room > ROOM_TEMPLATE.CHALLENGE_BOTTOM then
        kill_entity(ent.uid)
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_METAL_ARROW)

set_post_entity_spawn(function(ent)
    if not options.chain then return end
    local x, y, l = get_position(ent.uid)
    if state.theme == THEME.SUNKEN_CITY and l == LAYER.BACK then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_LIGHT_ARROW)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme == THEME.ICE_CAVES and l == LAYER.BACK and options.chain then
        return spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_PLASMACANNON)

set_post_entity_spawn(function(ent)
    if not options.chain then return end
    local x, y, l = get_position(ent.uid)
    if state.world == 4 and l == LAYER.BACK then
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_CLONEGUN)

local swapping_spikes = false
local ice_themes = {THEME.DWELLING, THEME.ICE_CAVES, THEME.OLMEC, THEME.TEMPLE, THEME.CITY_OF_GOLD}
local ice_subthemes = {COSUBTHEME.DWELLING, COSUBTHEME.ICE_CAVES, COSUBTHEME.TEMPLE}
local function shuffle_tile_codes()
    for k,v in pairs(floor_tilecodes) do
        if prng:random() < 0.05 and k ~= "floor" then
            floor_tilecodes[k] = ENT_TYPE.FLOORSTYLED_COG
        elseif prng:random() < 0.10 and has(ice_themes, state.theme_next) and (state.theme ~= THEME.COSMIC_OCEAN or has(ice_subthemes, get_co_subtheme())) then
            floor_tilecodes[k] = ENT_TYPE.FLOOR_ICE
        else
            floor_tilecodes[k] = pick(floor_types)
        end
        local type = k
        set_pre_tile_code_callback(function(x, y, layer)
            --if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 82 and y <= 90 then return false end
            local above = get_grid_entity_at(x, y+1, layer)
            if above ~= -1 then
                above = get_entity(above)
                if above.type.id == ENT_TYPE.FLOOR_SPIKES and not swapping_spikes then
                    return
                end
            end
            if forced_tiles[tile_key(x, y, layer)] ~= nil then
                spawn_grid_entity(forced_tiles[tile_key(x, y, layer)], x, y, layer)
            else
                spawn_grid_entity(floor_tilecodes[type], x, y, layer)
            end
            return true
        end, type)
    end
    floor_tilecodes["shop_wall"] = floor_tilecodes["floor"]
    floor_tilecodes["shop_sign"] = floor_tilecodes["floor"]
end

set_pre_entity_spawn(function(type, x, y, l, overlay, flags)
    if options.tilecode then
        return spawn_critical(floor_tilecodes["floor"], x, y, l, 0, 0)
    else
        return spawn_critical(type, x, y, l, 0, 0)
    end
end, SPAWN_TYPE.LEVEL_GEN_FLOOR_SPREADING, 0, nil)

local function set_theme_biases()
    theme = {}
    for i=1,options.bias_01 do theme[#theme+1] = 1 end
    for i=1,options.bias_02 do theme[#theme+1] = 2 end
    for i=1,options.bias_03 do theme[#theme+1] = 3 end
    for i=1,options.bias_05 do theme[#theme+1] = 5 end
    for i=1,options.bias_06 do theme[#theme+1] = 6 end
    for i=1,options.bias_07 do theme[#theme+1] = 7 end
    for i=1,options.bias_08 do theme[#theme+1] = 8 end
    for i=1,options.bias_09 do theme[#theme+1] = 9 end
    for i=1,options.bias_10 do theme[#theme+1] = 10 end
    for i=1,options.bias_11 do theme[#theme+1] = 11 end
    for i=1,options.bias_15 do theme[#theme+1] = 15 end
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
    set_theme_biases()
    while not done do
        if bosses_added < options.door_bosses and #insert_bosses > 0 and normal_levels >= options.door_min_levels and prng:random_int(1, options.door_max_levels, 0) <= normal_levels then
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
            elseif t == THEME.EGGPLANT_WORLD then
                l = 2
            else
                l = prng:random_int(1, 4, 0)
            end
            if has({THEME.DWELLING, THEME.JUNGLE, THEME.VOLCANA, THEME.TIDE_POOL}, t) and prng:random() < 0.2 then
                l = 1
            end
            local w = world[t]
            add_level(w, l, t)
        end
    end
    --fix_chain()
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
            if get_entity(hunduns[1]).move_state == 4 then -- this hundun is just chillin on the floor
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
    if not options.door then return end
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
    if options.door then
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
    if options.door then
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
    end
    if options.tilecode then shuffle_tile_codes() end
end, ON.LOADING)

set_callback(function(ctx)
    if boss_warp and state.screen == ON.TRANSITION and options.door then
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
    elseif state.theme == THEME.VOLCANA and test_flag(state.presence_flags, 3) then
        local _, y, _ = get_position(players[1].uid)
        if y < 90 then
            boss_warp = true
        else
            boss_warp = false
        end
    elseif state.theme == THEME.JUNGLE and test_flag(state.presence_flags, 2) then
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

    -- stupid crash fix
    if #get_entities_by_type(ENT_TYPE.MONS_YETIKING) > 0 then
        for i,v in ipairs(get_entities_by_type(ENT_TYPE.MONS_CRITTERBUTTERFLY)) do
            get_entity(v):destroy()
        end
    end
end, ON.FRAME)

set_callback(function()
    if options.door then
        toast("Died after "..tostring(state.level_count).." levels!\nBosses remaining: "..tostring(bosses_left()))
    end
end, ON.DEATH)

--[[PROJECTILES]]

local projectiles = {ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_LASERTRAP_SHOT, ENT_TYPE.ITEM_FREEZERAYSHOT}
local projectiles_pc = {ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_LASERTRAP_SHOT, ENT_TYPE.ITEM_FREEZERAYSHOT, ENT_TYPE.ITEM_CLONEGUNSHOT, ENT_TYPE.ITEM_PLASMACANNON_SHOT}
local projectiles_clone = {ENT_TYPE.ITEM_LASERTRAP_SHOT, ENT_TYPE.ITEM_FREEZERAYSHOT, ENT_TYPE.ITEM_CLONEGUNSHOT}
local projectiles_arrow = {ENT_TYPE.ITEM_LIGHT_ARROW, ENT_TYPE.ITEM_METAL_ARROW, ENT_TYPE.ITEM_METAL_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW}
local projectiles_web = {
    [0]=ENT_TYPE.ITEM_CHEST,
    [12]=ENT_TYPE.ITEM_NUGGET,
    [16]=ENT_TYPE.ITEM_ROCK,
    [17]=ENT_TYPE.ITEM_WOODEN_ARROW,
    [19]=ENT_TYPE.ITEM_POT,
    [49]=ENT_TYPE.ITEM_WEBGUN,
    [54]=ENT_TYPE.ITEM_TELEPORTER,
    [55]=ENT_TYPE.ITEM_BOOMERANG,
    [63]=ENT_TYPE.ITEM_SKULL,
    [68]=ENT_TYPE.ITEM_METAL_ARROW,
    [80]=ENT_TYPE.ITEM_BOMB,
    [107]=ENT_TYPE.ITEM_PUNISHBALL,
    [112]=ENT_TYPE.ITEM_MACHETE,
    [169]=ENT_TYPE.ITEM_LIGHT_ARROW,
    [173]=ENT_TYPE.ITEM_EGGPLANT,
    [195]=ENT_TYPE.ITEM_BROKENEXCALIBUR,
    [225]=ENT_TYPE.ITEM_GIANTSPIDER_WEBSHOT,
    [226]=ENT_TYPE.ITEM_ACIDSPIT,
}
local projectiles_web_ids = {}
for i,v in pairs(projectiles_web) do
    projectiles_web_ids[#projectiles_web_ids+1] = i
end
set_post_entity_spawn(function(ent)
    if not options.projectile then return end
    ent.animation_frame = pick(projectiles_web_ids)
    ent.flags = set_flag(ent.flags, ENT_FLAG.PASSES_THROUGH_EVERYTHING)
    local webshot = ent.uid
    set_interval(function()
        if get_entity(webshot) ~= nil and get_entity(webshot).layer < 2 and get_entity(webshot).overlay == nil and projectiles_web[get_entity(webshot).animation_frame] ~= nil then
            local webshot_ent = get_entity(webshot)
            local x, y, l = get_position(webshot)
            local vx, vy = webshot_ent.velocityx, webshot_ent.velocityy
            local af = webshot_ent.animation_frame
            local owner = webshot_ent.last_owner_uid
            move_entity(webshot, 0, 0, 0, 0)
            local newshot = spawn(projectiles_web[af], x, y, l, vx, vy)
            local newent = get_entity(newshot)
            newent.last_owner_uid = owner
            return false
        elseif get_entity(webshot) == nil then
            return false
        end
    end, 1)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_WEBSHOT)

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

set_post_entity_spawn(function(ent, flags)
    if options.projectile then
        set_arrowtrap_projectile(pick(projectiles_arrow), pick(projectiles_arrow))
    else
        set_arrowtrap_projectile(ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, projectiles_arrow)

--[[ this was a horrible idea
set_pre_entity_spawn(function(type, x, y, l, overlay)
    if options.projectile then
        return spawn_entity_nonreplaceable(pick(projectiles_clone), x, y, l, 0, 0)
    end
    return spawn(type, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_LASERTRAP_SHOT)]]

--[[STORAGE]]
local storage_bad_rooms = {ROOM_TEMPLATE.SHOP, ROOM_TEMPLATE.SHOP_LEFT, ROOM_TEMPLATE.VAULT, ROOM_TEMPLATE.CURIOSHOP, ROOM_TEMPLATE.CURIOSHOP_LEFT, ROOM_TEMPLATE.CAVEMANSHOP, ROOM_TEMPLATE.SHOP_ATTIC, ROOM_TEMPLATE.SHOP_ATTIC_LEFT, ROOM_TEMPLATE.SHOP_BASEMENT, ROOM_TEMPLATE.SHOP_BASEMENT_LEFT, ROOM_TEMPLATE.TUSKFRONTDICESHOP, ROOM_TEMPLATE.TUSKFRONTDICESHOP_LEFT, ROOM_TEMPLATE.PEN_ROOM, ROOM_TEMPLATE.TUSKDICESHOP, ROOM_TEMPLATE.TUSKDICESHOP_LEFT, ROOM_TEMPLATE.DICESHOP, ROOM_TEMPLATE.DICESHOP_LEFT, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100} --CHALLENGE ROOMS
--[[ TODO: destroying these is randomly crashing the game, maybe because they're still spawning in owned floors
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
end, ON.POST_LEVEL_GENERATION)]]

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
    if options.door and options.status and state.screen >= ON.LEVEL and state.level_count+1 <= #level_order and state.loading == 0 then
        --ctx:draw_text(-0.065, 0.98, 28, F"{FakeWorld}-{FakeLevel}   {LevelNum}/{#level_order}", 0x44FFFFFF)
        ctx:draw_text(0.9, 0.81, 32, F"{LevelNum}/{#level_order}", 0xBBFFFFFF)
    end
end, ON.GUIFRAME)

--[[LIQUIDS]]
--[[SPIKES]]
set_callback(function()
    swapping_liquid = state.theme ~= THEME.OLMEC and prng:random() < options.liquid_chance/100
    if state.theme == THEME.OLMEC then
        replace_drop(DROP.OLMEC_SISTERS_BOMBBOX, pick(crate_items))
        replace_drop(DROP.OLMEC_SISTERS_ROPEPILE, pick(sister_items))
    elseif state.theme == THEME.DUAT then
        replace_drop(DROP.DUATALTAR_BOMBBAG, pick(crate_items))
        replace_drop(DROP.DUATALTAR_BOMBBOX, pick(crate_items))
        --replace_drop(DROP.DUATALTAR_COOKEDTURKEY, pick(crate_items)) --doesnt work
    end
end, ON.PRE_LEVEL_GENERATION)

local function swap_liquid(liquid_type, x, y)
    if swapping_liquid then
        spawn_liquid(liquid_type, x, y)
        return true
    end
    return false
end

set_pre_tile_code_callback(function(x, y, l)
    return swap_liquid(ENT_TYPE.LIQUID_WATER, x, y)
end, "lava")

set_pre_tile_code_callback(function(x, y, l)
    if state.theme == THEME.HUNDUN and options.hard_hundun then return false end
    return swap_liquid(ENT_TYPE.LIQUID_LAVA, x, y)
end, "water")

set_pre_tile_code_callback(function(x, y, l)
    return swap_liquid(ENT_TYPE.LIQUID_COARSE_WATER, x, y)
end, "coarse_lava")

set_pre_tile_code_callback(function(x, y, l)
    return swap_liquid(ENT_TYPE.LIQUID_COARSE_LAVA, x, y)
end, "coarse_water")

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if swapping_liquid then
        return spawn_entity_nonreplaceable(ENT_TYPE.LIQUID_IMPOSTOR_LAKE, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(ENT_TYPE.LIQUID_IMPOSTOR_LAVA, x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.LIQUID_IMPOSTOR_LAVA)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if swapping_liquid then
        return spawn_entity_nonreplaceable(ENT_TYPE.LIQUID_IMPOSTOR_LAVA, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(ENT_TYPE.LIQUID_IMPOSTOR_LAKE, x, y, l, 0, 0)
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.LIQUID_IMPOSTOR_LAKE)

set_pre_tile_code_callback(function(x, y, l)
    return swap_liquid(ENT_TYPE.LIQUID_COARSE_LAVA, x, y)
end, "coarse_water")

local last_spike_room = -1
local spike_floors = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_TIMED_FORCEFIELD, ENT_TYPE.FLOOR_TOMB, ENT_TYPE.FLOOR_QUICKSAND}
local spike_items = {ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.FLOOR_SPRING_TRAP, ENT_TYPE.ITEM_SNAP_TRAP}
local spike_types = join(spike_floors, spike_items)
local spike_type = spike_types[1]

local function swap_spikes(x, y, l)
    local rx, ry = get_room_index(x, y)
    local rid = 100*rx+ry
    if rid ~= last_spike_room then
        last_spike_room = rid
        swapping_spikes = prng:random() < options.trap_spikes/100
        spike_type = pick(spike_types)
    end
    if options.trap and swapping_spikes and l == LAYER.FRONT then
        if has(spike_floors, spike_type) then
            forced_tiles[tile_key(x, y-1, l)] = spike_type
        else
            spawn_entity_snapped_to_floor(spike_type, x, y, l)
        end
        return true
    end
    return false
end

set_pre_tile_code_callback(function(x, y, l)
    return swap_spikes(x, y, l)
end, "spikes")

set_pre_tile_code_callback(function(x, y, l)
    local rx, ry = get_room_index(x, y)
    local rt = get_room_template(rx, ry, l)
    if l == LAYER.FRONT and options.trap and prng:random() < options.trap_fields/100 and not has(storage_bad_rooms, rt) then
        spawn_grid_entity(pick(traps_floor), x, y, l)
        return true
    end
    return false
end, "forcefield")

set_pre_tile_code_callback(function(x, y, l)
    local rx, ry = get_room_index(x, y)
    local rt = get_room_template(rx, ry, l)
    if l == LAYER.FRONT and options.trap and prng:random() < options.trap_fields/100 and not has(storage_bad_rooms, rt) then
        spawn_grid_entity(pick(traps_floor), x, y, l)
        return true
    end
    return false
end, "timed_forcefield")

set_callback(function()
    for i,v in ipairs(get_entities_by_type(ENT_TYPE.FLOOR_QUICKSAND)) do
        local decos = entity_get_items_by(v, ENT_TYPE.DECORATION_GENERIC, 0)
        for j,d in ipairs(decos) do
            get_entity(d):set_texture(TEXTURE.DATA_TEXTURES_FLOOR_TEMPLE_0)
        end
    end
    if swapping_liquid and state.theme == THEME.TIDE_POOL and state.level == 2 then
        local xmin, ymin, xmax, ymax = get_bounds()
        local liquid_tiles = {}
        for x = xmin, xmax, 1 do
            for y = ymin, ymax, -1 do
                local box = AABB:new(x, y, x+1, y-1)
                local blobs = #get_entities_overlapping_hitbox(0, MASK.WATER, box, LAYER.FRONT)
                if blobs > 0 then
                liquid_tiles[tostring(math.floor(x)).."-"..tostring(math.floor(y))] = {math.floor(x), math.floor(y), blobs}
                end
            end
        end
        for i,v in ipairs(get_entities_by_mask(MASK.WATER)) do
            kill_entity(v)
        end
        for i,v in pairs(liquid_tiles) do
            if v[3] > 16 then
                spawn_liquid(ENT_TYPE.LIQUID_LAVA, v[1]+1, v[2]+0.1)
            end
        end
    end
end, ON.POST_LEVEL_GENERATION)

--[[OLMEC PHASES]]
local olmec_phases = {0, 1, 2}
local last_olmec_state = -1
local last_olmec_change = -9999
local olmec_cooldown = 180
local olmec_floaters = {}
local olmec_callback = -1

local function olmec_phase(o)
    if o.attack_phase == 3 then return end
    last_olmec_change = state.time_level
    local old_phase = o.attack_phase
    local phase = pick(olmec_phases, old_phase)
    if phase == old_phase then return end
    if phase ~= 1 then
        o.flags = clr_flag(o.flags, ENT_FLAG.NO_GRAVITY)
        o.move_state = 0
    end
    o.unknown_attack_state = 0
    if phase == 2 then
        o.unknown_attack_state = -2
    else
        o.unknown_attack_state = 0
    end
    for i,v in ipairs(get_entities_by_type(ENT_TYPE.FX_OLMECPART_FLOATER)) do
        local e = get_entity(v)
        e.health = 1
        if phase == 1 then
            e.flags = clr_flag(e.flags, ENT_FLAG.INVISIBLE)
        else
            e.flags = set_flag(e.flags, ENT_FLAG.INVISIBLE)
            e.y = -1.6
        end
    end
    o.attack_phase = phase
    o.attack_timer = 120
end

set_callback(function()
    clear_callback(olmec_callback)
    olmec_callback = -1
    olmec_floaters = {}
    last_olmec_change = -9999
    if state.theme == THEME.OLMEC then
        if options.hard_olmec_phase then
            set_olmec_phase_y_level(1, 66)
            set_olmec_phase_y_level(2, 66)
            local olmecs = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_OLMEC)
            if #olmecs > 0 then
                local olmec = olmecs[1]
                set_interval(function()
                    local ent = get_entity(olmec)
                    if state.time_level >= last_olmec_change+olmec_cooldown and ((ent.attack_phase == 0 and ent.move_state == 0) or (ent.attack_phase == 1 and ent.move_state == 11) or (ent.attack_phase == 2 and ent.move_state == 0)) and ent.standing_on_uid ~= -1 then
                        olmec_phase(ent)
                    end
                    if ent.attack_phase > 2 then return false end
                end, 1)
                for i,v in ipairs(get_entities_by_type(ENT_TYPE.FX_OLMECPART_SMALL, ENT_TYPE.ITEM_OLMECCANNON_UFO)) do
                    local e = get_entity(v)
                    e.flags = clr_flag(e.flags, ENT_FLAG.INVISIBLE)
                end
                for i,v in ipairs(get_entities_by_type(ENT_TYPE.FX_OLMECPART_MEDIUM)) do
                    local e = get_entity(v)
                    if e.animation_frame == 13 then
                        e.flags = clr_flag(e.flags, ENT_FLAG.INVISIBLE)
                    end
                end
                for i,v in ipairs(get_entities_by_type(ENT_TYPE.FX_OLMECPART_FLOATER)) do
                    olmec_floaters[#olmec_floaters+1] = v
                end
                olmec_callback = set_callback(function(ctx, depth) --TODO: stupid hack
                    if state.theme ~= THEME.OLMEC or state.screen ~= SCREEN.LEVEL then
                        clear_callback(olmec_callback)
                        olmec_callback = -1
                        return
                    end
                    if depth == 34 and state.theme == THEME.OLMEC then
                        for i,v in ipairs(olmec_floaters) do
                            local e = get_entity(v)
                            if e ~= nil then
                                if e.health > 0 then
                                    e.animation_frame = 38
                                end
                            end
                        end
                    end
                end, ON.RENDER_PRE_DRAW_DEPTH)
            end
        else
            set_olmec_phase_y_level(1, 98)
            set_olmec_phase_y_level(2, 82)
        end
    end
end, ON.LEVEL)

set_callback(function()
    if state.loading == 1 then
        clear_callback(olmec_callback)
        olmec_callback = -1
    end
end, ON.LOADING)

--[[DRILL]]
local drill_char
local socket_char
local drill_spawned = false
local drill_themes = {THEME.DWELLING, THEME.JUNGLE, THEME.VOLCANA, THEME.TIDE_POOL, THEME.TEMPLE}
local valid_drill_rooms = {ROOM_TEMPLATE.SIDE, ROOM_TEMPLATE.PATH_NORMAL, ROOM_TEMPLATE.PATH_DROP}
local exit_rooms = {ROOM_TEMPLATE.EXIT, ROOM_TEMPLATE.EXIT_NOTOP}

local function flip(str)
    local ret = ""
    for line in str:gmatch("([^\n]*)\n?") do
        ret = ret .. string.reverse(line)
    end
    return ret
end

set_callback(function(ctx)
    if options.drill > 0 then
        local drill_code = ShortTileCodeDef:1new()
        drill_code.tile_code = TILE_CODE.DRILL
        local socket_code = ShortTileCodeDef:new()
        socket_code.tile_code = TILE_CODE.UDJAT_SOCKET
        drill_char = ctx:define_short_tile_code(drill_code)
        socket_char = ctx:define_short_tile_code(socket_code)
    end
    drill_spawned = test_flag(state.presence_flags, 3)
end, ON.POST_ROOM_GENERATION)

set_callback(function(x, y, l, r)
    local drill_chance = options.drill
    if state.height > 4 then
        drill_chance = drill_chance * 2
    end
    if not drill_spawned and drill_char and socket_char and y == 0 and l == LAYER.FRONT and has(valid_drill_rooms, r) and has(drill_themes, state.theme) and not has(exit_rooms, get_room_template(x, state.height-1, LAYER.FRONT)) and prng:random() < drill_chance/100 then
        drill_spawned = true
        local data = [[
1==XX==222
2==XX==000
02=]]..string.char(drill_char)..[[0==0==
00=00=000=
00=00=0]]..string.char(socket_char)..[[00
0000000=00
===00=====
2220011111]]
        if prng:random() < 0.5 then
            data = [[
222==XX==1
000==XX==2
==0==]]..string.char(drill_char)..[[0=20
=000=00=00
00]]..string.char(socket_char)..[[0=00=00
00=0000000
=====00===
1111100222]]
        end
        return data
    end
end, ON.PRE_GET_RANDOM_ROOM)
