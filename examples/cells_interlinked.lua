meta.name = "Cells Interlinked"
meta.version = "1.0"
meta.author = "Dregu"
meta.description = "Cellular automaton (Game of Life) based level gen with some enemies and traps thrown in from the randomizer. It's endless. You can't win. You have no purpose."

register_option_float("_enemy", "Enemy chance", 10, 0, 100)
register_option_float("_trap", "Trap chance", 5, 0, 100)
register_option_float("alive", "Cell alive chance", 39, 0, 100)
register_option_float("alive_variance", "Cell alive variance", 3, 0, 100)
register_option_int("simstep", "Cell automaton steps", 4, 0, 10)
register_option_bool("start_compass", "Start with compass", true)
register_option_bool("start_cape", "Start with Vlad's cape", true)
register_option_bool("start_gifts", "Start with goodies", true)
register_option_int("xmin", "Level min width", 2, 2, 8)
register_option_int("xmax", "Level max width", 6, 2, 8)
register_option_int("ymin", "Level min height", 2, 2, 8)
register_option_int("ymax", "Level max height", 6, 2, 15)

local map = { level={}, alt={}, back={} }
local mapwidth, mapheight = 40, 32
local entrance, exit
local themes = {}
local current = nil

local function in_level()
    return state.screen == SCREEN.LEVEL
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

local function pick(from)
    return from[prng:random(#from)]
end

local function kil(uid)
    local ent = get_entity(uid)
    ent.flags = set_flag(ent.flags, ENT_FLAG.DEAD)
    ent:destroy()
end

local theme_types = {THEME.DWELLING, THEME.JUNGLE, THEME.VOLCANA, THEME.OLMEC, THEME.TIDE_POOL, THEME.TEMPLE, THEME.ICE_CAVES, THEME.SUNKEN_CITY}
local floor_types = {ENT_TYPE.FLOOR_GENERIC, ENT_TYPE.FLOOR_JUNGLE, ENT_TYPE.FLOORSTYLED_MINEWOOD, ENT_TYPE.FLOORSTYLED_STONE, ENT_TYPE.FLOORSTYLED_TEMPLE, ENT_TYPE.FLOORSTYLED_PAGODA, ENT_TYPE.FLOORSTYLED_BABYLON, ENT_TYPE.FLOORSTYLED_SUNKEN, ENT_TYPE.FLOORSTYLED_BEEHIVE, ENT_TYPE.FLOORSTYLED_VLAD, ENT_TYPE.FLOORSTYLED_MOTHERSHIP, ENT_TYPE.FLOORSTYLED_DUAT, ENT_TYPE.FLOORSTYLED_PALACE, ENT_TYPE.FLOORSTYLED_GUTS, ENT_TYPE.FLOOR_SURFACE, ENT_TYPE.FLOOR_ICE}
local valid_floors = floor_types

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
local friends = {ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_MARGARET_TUNNEL, ENT_TYPE.CHAR_COLIN_NORTHWARD, ENT_TYPE.CHAR_ROFFY_D_SLOTH, ENT_TYPE.CHAR_BANDA, ENT_TYPE.CHAR_GREEN_GIRL, ENT_TYPE.CHAR_AMAZON, ENT_TYPE.CHAR_LISE_SYSTEM, ENT_TYPE.CHAR_COCO_VON_DIAMONDS, ENT_TYPE.CHAR_MANFRED_TUNNEL, ENT_TYPE.CHAR_OTAKU, ENT_TYPE.CHAR_TINA_FLAN, ENT_TYPE.CHAR_VALERIE_CRUMP, ENT_TYPE.CHAR_AU, ENT_TYPE.CHAR_DEMI_VON_DIAMONDS, ENT_TYPE.CHAR_PILOT, ENT_TYPE.CHAR_PRINCESS_AIRYN, ENT_TYPE.CHAR_DIRK_YAMAOKA, ENT_TYPE.CHAR_GUY_SPELUNKY, ENT_TYPE.CHAR_CLASSIC_GUY, ENT_TYPE.CHAR_HIREDHAND, ENT_TYPE.CHAR_EGGPLANT_CHILD}

local traps_ceiling = {ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_FACTORY_GENERATOR, ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_FACTORY_GENERATOR, ENT_TYPE.FLOOR_SHOPKEEPER_GENERATOR}
local traps_floor = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_TIMED_FORCEFIELD, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP, ENT_TYPE.ACTIVEFLOOR_ELEVATOR}
local traps_wall = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_LASER_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP}
local traps_flip = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_LASER_TRAP}
local traps_generic = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
local traps_item = {ENT_TYPE.FLOOR_SPRING_TRAP, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SNAP_TRAP, ENT_TYPE.ACTIVEFLOOR_POWDERKEG}
local traps_totem = {ENT_TYPE.FLOOR_TOTEM_TRAP, ENT_TYPE.FLOOR_LION_TRAP}

local function enemy_small_spawn(x, y, l)
    local enemy = pick(enemies_small)
    local uid = spawn_entity_snapped_to_floor(enemy, x, y, l)
end
local function enemy_small_valid(x, y, l)
    local floor = get_grid_entity_at(x, y-1, l)
    local air = get_grid_entity_at(x, y, l)
    if floor ~= -1 and air == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end

local function enemy_big_spawn(x, y, l)
    local id = pick(enemies_big)
    local uid = spawn_entity_snapped_to_floor(id, x, y, l)
    local ent = get_entity(uid)
    if id == ENT_TYPE.MOUNT_MECH then
        local rider = spawn_entity(pick({ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_ALIEN}), x, y, l, 0, 0)
        carry(uid, rider)
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

local function enemy_climb_spawn(x, y, l)
    local ladder = get_grid_entity_at(x, y, l)
    if ladder ~= -1 then
        ladder = get_entity(ladder)
        if not test_flag(ladder.flags, ENT_FLAG.CLIMBABLE) then
            return
        end
        local uid = spawn_entity_over(pick(enemies_climb), ladder.uid, 0, 0)
        local ent = get_entity(uid)
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

local function enemy_ceiling_spawn(x, y, l)
    local uid = spawn_entity(pick(enemies_ceiling), x, y, l, 0, 0)
    local ent = get_entity(uid)
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

local function enemy_air_spawn(x, y, l)
    local uid = spawn_entity(pick(enemies_air), x, y, l, 0, 0)
    local ent = get_entity(uid)
end
local function enemy_air_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 82 and y <= 90 then return false end
    if state.theme == THEME.TIAMAT then return false end
    local air = get_grid_entity_at(x, y, l)
    return air == -1
end

local friend_spawned = false
local function friend_spawn(x, y, l)
    if not friend_spawned then
        local uid = spawn_companion(pick(friends), x, y, l)
        local ent = get_entity(uid)
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

local function trap_ceiling_spawn(x, y, l)
    local item = pick(traps_ceiling)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kil(floor)
    end
    spawn_grid_entity(item, x, y, l)
end
local function trap_ceiling_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
    if has({THEME.CITY_OF_GOLD, THEME.ICE_CAVES, THEME.TIAMAT, THEME.OLMEC}, state.theme) then
        return false
    end
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

local function trap_floor_spawn(x, y, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kil(floor)
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
    local floor = get_grid_entity_at(x, y, l)
    local above = get_grid_entity_at(x, y+1, l)
    if floor ~= -1 and above == -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end

local function trap_wall_spawn(x, y, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kil(floor)
    end
    local id = pick(traps_wall)
    local ent = spawn_grid_entity(id, x, y, l)
    local left = get_grid_entity_at(x-1, y, l)
    local right = get_grid_entity_at(x+1, y, l)
    if has(traps_flip, id) then
        if left == -1 and right == -1 then
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

local function trap_generic_spawn(x, y, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kil(floor)
    end
    spawn_grid_entity(pick(traps_generic), x, y, l)
end
local function trap_generic_valid(x, y, l)
    if state.theme == THEME.TIDE_POOL and state.level == 3 and y >= 80 and y <= 90 then return false end
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

local function trap_frog_spawn(x, y, l)
    local id = ENT_TYPE.FLOOR_BIGSPEAR_TRAP
    local uid = get_grid_entity_at(x, y, l)
    if uid ~= -1 then
        kil(uid)
    end
    uid = get_grid_entity_at(x+1, y, l)
    if uid ~= -1 then
        kil(uid)
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

local function count_neighbours(name, x, y)
    local count = 0
    for i = -1, 1 do
        for j = -1, 1 do
            local n_X = x + i
            local n_Y = y + j
            if i == 0 and j == 0 then
            elseif n_X < 1 or n_Y < 1 or n_X > #map[name][1] or n_Y > #map[name] then
                count = count + 1
            elseif map[name][n_Y][n_X] == 1 then
                count = count + 1
            end
        end
    end
    return count
end

local function initialize(name, x, y)
    for a = 1, y do
        table.insert(map[name], {})
        for b = 1, x do
            if prng:random() < (options.alive+(prng:random()-0.5)*options.alive_variance*2) / 100 then
                table.insert(map[name][a], 1)
            else
                table.insert(map[name][a], 0)
            end
        end
    end
end

local function simstep(name)
    local birth = 4
    local death = 3
    for a = 1, #map[name] do
        for b = 1, #map[name][1] do
            local newval = count_neighbours(name, b, a)
            if map[name][a][b] == 1 then
                if newval < death then
                    map[name][a][b] = 0
                else
                    map[name][a][b] = 1
                end
            else
                if newval > birth then
                    map[name][a][b] = 1
                else
                    map[name][a][b] = 0
                end
            end
        end
    end
end

local function create_map(name)
    mapwidth = 10*state.width
    mapheight = 8*state.height
    map[name] = {}
    initialize(name, mapwidth, mapheight)
    for s = 1, options.simstep do
        simstep(name)
    end
    for t = 1, #map[name] do
        map[name][t][1] = 1
        map[name][t][#map[name][1]] = 1
    end
end

local function dist(a, b)
    return math.sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y))
end

local function spawn_enemies(name)
    friend_spawned = false
    for y = 1, #map[name] do
        for x = 1, #map[name][1] do
            local p = Vec2:new(x + 2, 123 - y)
            if dist(p, entrance) > 6 then
                if prng:random() < options["_enemy"] / 100 and enemy_small_valid(p.x, p.y, LAYER.FRONT) then
                    enemy_small_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() * 3 < options["_enemy"] / 100 and enemy_big_valid(p.x, p.y, LAYER.FRONT) then
                    enemy_big_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() * 4 < options["_enemy"] / 100 and enemy_ceiling_valid(p.x, p.y, LAYER.FRONT) then
                    enemy_ceiling_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() * 5 < options["_enemy"] / 100 and enemy_air_valid(p.x, p.y, LAYER.FRONT) then
                    enemy_air_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() * 5 < options["_enemy"] / 100 and friend_valid(p.x, p.y, LAYER.FRONT) then
                    friend_spawn(p.x, p.y, LAYER.FRONT)
                end
            end
        end
    end
end

local function spawn_traps(name)
    for y = 1, #map[name] do
        for x = 1, #map[name][1] do
            local p = Vec2:new(x + 2, 123 - y)
            if dist(p, entrance) > 6 then
                if prng:random() < options["_trap"] / 100 and trap_ceiling_valid(p.x, p.y, LAYER.FRONT) then
                    trap_ceiling_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() < options["_trap"] / 100 and trap_floor_valid(p.x, p.y, LAYER.FRONT) then
                    trap_floor_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() < options["_trap"] / 100 and trap_generic_valid(p.x, p.y, LAYER.FRONT) then
                    trap_generic_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() < options["_trap"] / 100 and trap_item_valid(p.x, p.y, LAYER.FRONT) then
                    trap_item_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() < options["_trap"] / 100 and trap_totem_valid(p.x, p.y, LAYER.FRONT) then
                    trap_totem_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() < options["_trap"] / 100 and trap_wall_valid(p.x, p.y, LAYER.FRONT) then
                    trap_wall_spawn(p.x, p.y, LAYER.FRONT)
                end
                if prng:random() < options["_trap"] / 100 and trap_frog_valid(p.x, p.y, LAYER.FRONT) then
                    trap_frog_spawn(p.x, p.y, LAYER.FRONT)
                end
            end
        end
    end
end

local function init_rooms()
    -- can't do this in POST_ROOM_GENERATION because it never happens
    local ctx = PostRoomGenerationContext:new()
    for x=0,state.width-1 do
        for y=0,state.height-1 do
            ctx:set_room_template(x, y, LAYER.FRONT, ROOM_TEMPLATE.PATH_NORMAL)
        end
    end
    ctx:set_room_template(state.level_gen.spawn_room_x, state.level_gen.spawn_room_y, LAYER.FRONT, ROOM_TEMPLATE.ENTRANCE)
    local ex, ey = get_room_index(exit.x, exit.y)
    ctx:set_room_template(ex, ey, LAYER.FRONT, ROOM_TEMPLATE.EXIT)
end

local function init_level()
    create_map("level")
    create_map("alt")
    local on_floor = {}
    for y = 1, #map["level"] do
        for x = 1, #map["level"][1] do
            if map["level"][y][x] == 1 then
                local wx, wy = x + 2, 123 - y
                local floor_type = current.floor
                if map["alt"][y][x] == 1 then floor_type = current.alt_floor end
                if y > 2 and x > 1 and x < #map["level"][1] and map["level"][y-1][x] == 0 and map["level"][y-1][x-1] == 0 and map["level"][y-1][x+1] == 0 and map["level"][y][x-1] == 1 and map["level"][y][x+1] == 1 then
                    on_floor[#on_floor+1] = Vec2:new(wx, wy + 1)
                end
            end
        end
    end
    if #on_floor < 2 then
        for y = 1, #map["level"] do
            for x = 1, #map["level"][1] do
                if map["level"][y][x] == 1 then
                    local wx, wy = x + 2, 123 - y
                    if y > 2 and map["level"][y-1][x] == 0 then
                        on_floor[#on_floor+1] = Vec2:new(wx, wy + 1)
                    end
                end
            end
        end
    end
    if #on_floor < 2 then
        local x = prng:random(2, #map["level"][1]-1)
        local y = prng:random(2, #map["level"]-1)
        entrance = Vec2:new(x + 2, 123 - y)
        map["level"][y][x] = 0
        map["level"][y][x-1] = 0
        map["level"][y][x+1] = 0
        repeat
            x = prng:random(2, #map["level"][1]-1)
            y = prng:random(2, #map["level"]-1)
            exit = Vec2:new(x + 2, 123 - y)
        until entrance.x ~= exit.x or entrance.y ~= exit.y
        map["level"][y][x] = 0
    else
        entrance = pick(on_floor)
        local exit_dist = 30
        repeat
            exit = pick(on_floor)
            exit_dist = exit_dist - 1
        until dist(entrance, exit) > exit_dist and (entrance.x ~= exit.x or entrance.y ~= exit.y)
    end
    state.level_gen.spawn_x = entrance.x
    state.level_gen.spawn_y = entrance.y
    state.level_gen.spawn_room_x, state.level_gen.spawn_room_y = get_room_index(entrance.x, entrance.y)
    init_rooms()
end

local function spawn_level()
    for y = 1, #map["level"] do
        for x = 1, #map["level"][1] do
            if map["level"][y][x] == 1 then
                local wx, wy = x + 2, 123 - y
                local floor_type = current.floor
                if map["alt"][y][x] == 1 then floor_type = current.alt_floor end
                spawn_grid_entity(floor_type, wx, wy, LAYER.FRONT)
            end
        end
    end

    spawn(ENT_TYPE.BG_DOOR, entrance.x, entrance.y+0.3, LAYER.FRONT, 0, 0)
    local uid = spawn(ENT_TYPE.LOGICAL_DOOR, exit.x, exit.y, LAYER.FRONT, 0, 0)
    local ent = get_entity(uid)
    ent.door_type = ENT_TYPE.FLOOR_DOOR_EXIT
    ent.platform_type = ENT_TYPE.FLOOR_DOOR_PLATFORM
    spawn_over(ENT_TYPE.FX_COMPASS, uid, 0, 0)
    spawn(ENT_TYPE.BG_DOOR, exit.x, exit.y+0.3, LAYER.FRONT, 0, 0)
    unlock_door_at(exit.x, exit.y)

    current.custom:spawn_procedural()
    spawn_traps("level")
    spawn_enemies("level")
end

set_callback(function(ctx)
    if not in_level() then return end
    ctx:override_level_files({})
    current = pick(themes)
    state.theme = current.theme
    current.custom:override(THEME_OVERRIDE.SPAWN_LEVEL, spawn_level)
    force_custom_theme(current.custom)
end, ON.PRE_LOAD_LEVEL_FILES)

set_callback(function()
    if not in_level() then return end
    state.width = prng:random(options.xmin, options.xmax)
    state.height = prng:random(options.ymin, options.ymax)
    state.level_gen.themes[THEME.DWELLING]:spawn_border()
    init_level()
end, ON.PRE_LEVEL_GENERATION)

set_callback(function()
    if not in_level() then return end
    if state.level_count == 0 then
        get_player(1).health = 20
        get_player(1).inventory.bombs = 20
        get_player(1).inventory.ropes = 20
        if options.start_compass then
            spawn_on_floor(ENT_TYPE.ITEM_PICKUP_COMPASS, 0, 0, LAYER.PLAYER)
        end
        if options.start_cape then
            pick_up(get_player(1).uid, spawn_on_floor(ENT_TYPE.ITEM_VLADS_CAPE, 0, 0, LAYER.PLAYER))
        end
    end
    state.world = 1
    state.level = state.level_count + 1
    if options.start_gifts then
        spawn_on_floor(ENT_TYPE.ITEM_CRATE, -1, 0, LAYER.PLAYER)
        spawn_on_floor(ENT_TYPE.ITEM_PRESENT, 1, 0, LAYER.PLAYER)
    end
end, ON.POST_LEVEL_GENERATION)

set_callback(function()
    for i=100,130 do
        local base = pick(theme_types)
        local theme = {
            theme=base,
            floor=pick(floor_types),
            alt_floor=pick(floor_types),
            custom=CustomTheme:new(i, base)
        }
        themes[#themes+1] = theme
    end
end, ON.LOAD)
