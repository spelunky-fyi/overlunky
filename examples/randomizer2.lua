meta.name = "Rando Two"
meta.description = "Second incarnation of The Randomizer with new api shenannigans. Everything is now rewritten from scratch and I tuned the crazyness down a notch. Still needs some balancing and less CoGs. There's a kinda new progression system forming with a high probability to be able to do chain (not finished), with multiple endings. It's still hard AF, good luck getting true ending!"
meta.version = "1.9999"
meta.author = "Dregu"

local function get_chance(min, max)
    min = math.floor(1/(min/100))
    max = math.floor(1/(max/100))
    return math.random(max, min)
end

local function pick(from)
    return from[math.random(#from)]
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

local theme_name = {}
for i,v in pairs(THEME) do
    theme_name[v] = i
end

--[[TRAPS]]
register_option_float("trap_max", "Max trap chance", 5, 0, 100)
register_option_float("trap_min", "Min trap chance", 1.5, 0, 100)

local traps_ceiling = {ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_FACTORY_GENERATOR, ENT_TYPE.FLOOR_SHOPKEEPER_GENERATOR}
local traps_floor = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_TIMED_FORCEFIELD, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP, ENT_TYPE.ACTIVEFLOOR_ELEVATOR}
local traps_wall = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_LASER_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP}
local traps_flip = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_LASER_TRAP}
local traps_generic = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
local traps_item = {ENT_TYPE.FLOOR_SPRING_TRAP, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SNAP_TRAP, ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
local traps_totem = {ENT_TYPE.FLOOR_TOTEM_TRAP, ENT_TYPE.FLOOR_LION_TRAP}
local trap_arrows = {ENT_TYPE.ITEM_LIGHT_ARROW, ENT_TYPE.ITEM_METAL_ARROW, ENT_TYPE.ITEM_WOODEN_ARROW}
local valid_floors = {ENT_TYPE.FLOOR_GENERIC, ENT_TYPE.FLOORSTYLED_TEMPLE, ENT_TYPE.FLOORSTYLED_COG, ENT_TYPE.FLOORSTYLED_BABYLON, ENT_TYPE.FLOORSTYLED_DUAT, ENT_TYPE.FLOORSTYLED_STONE}

local function trap_ceiling_spawn(x, y, l)
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        kill_entity(floor)
    end
    spawn_grid_entity(pick(traps_ceiling), x, y, l)
end
local function trap_ceiling_valid(x, y, l)
    if has({THEME.CITY_OF_GOLD, THEME.ICE_CAVES, THEME.TIAMAT}, state.theme) then
        return false
    end
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
            math.randomseed(read_prng()[1])
            if math.random() < 0.5 then
                flip_entity(ent)
            end
        elseif left == -1 then
            flip_entity(ent)
        end
    end
end
local function trap_wall_valid(x, y, l)
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
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local trap_generic_chance = define_procedural_spawn("trap_generic", trap_generic_spawn, trap_generic_valid)

local function trap_item_spawn(x, y, l)
    spawn_entity_snapped_to_floor(pick(traps_item), x, y, l)
end
local function trap_item_valid(x, y, l)
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
    local floor = get_grid_entity_at(x, y, l)
    local box = AABB:new()
    box.left = x-1
    box.right = x+1
    box.top = y+3
    box.bottom = y+1
    local air = get_entities_overlapping_hitbox(0, MASK.FLOOR, box, l)
    if floor ~= -1 and #air == 0 then
        floor = get_entity(floor)
        return has(valid_floors, floor.type.id)
    end
    return false
end
local trap_totem_chance = define_procedural_spawn("trap_totem", trap_totem_spawn, trap_totem_valid)

set_callback(function(ctx)
    math.randomseed(read_prng()[1])
    ctx:set_procedural_spawn_chance(trap_ceiling_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_floor_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_wall_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_generic_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_item_chance, get_chance(options.trap_min, options.trap_max))
    ctx:set_procedural_spawn_chance(trap_totem_chance, get_chance(options.trap_min, options.trap_max))
end, ON.POST_ROOM_GENERATION)

set_callback(function()
    set_interval(function()
        set_arrowtrap_projectile(pick(trap_arrows), pick(trap_arrows))
    end, 60)
end, ON.LEVEL)

--[[ENEMIES]]
register_option_float("enemy_max", "Max enemy chance", 12, 0, 100)
register_option_float("enemy_min", "Min enemy chance", 4, 0, 100)
register_option_float("enemy_curse_chance", "Enemy handicap chance", 5, 0, 100)

local enemies_small = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER,
    ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
    ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_MANTRAP, ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_WITCHDOCTOR,
    ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.MONS_ROBOT,
    ENT_TYPE.MONS_FIREBUG_UNCHAINED, ENT_TYPE.MONS_IMP,
    ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS,
    ENT_TYPE.MONS_CATMUMMY, ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI,
    ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_UFO, ENT_TYPE.MONS_ALIEN,
    ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER, ENT_TYPE.MONS_SHOPKEEPERCLONE,
    ENT_TYPE.MONS_OLMITE_HELMET, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED,
    ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_AMMIT, ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG,
    ENT_TYPE.MONS_JUMPDOG, ENT_TYPE.MONS_SCARAB, ENT_TYPE.MONS_LEPRECHAUN, ENT_TYPE.MOUNT_TURKEY,
    ENT_TYPE.MOUNT_ROCKDOG, ENT_TYPE.MOUNT_AXOLOTL}
local enemies_big = {ENT_TYPE.MONS_CAVEMAN_BOSS, ENT_TYPE.MONS_LAVAMANDER, ENT_TYPE.MONS_MUMMY, ENT_TYPE.MONS_ANUBIS,
    ENT_TYPE.MONS_GIANTFISH, ENT_TYPE.MONS_YETIKING, ENT_TYPE.MONS_YETIQUEEN, ENT_TYPE.MONS_ALIENQUEEN,
    ENT_TYPE.MONS_LAMASSU, ENT_TYPE.MONS_QUEENBEE, ENT_TYPE.MONS_GIANTFLY, ENT_TYPE.MONS_CRABMAN,
    ENT_TYPE.MOUNT_MECH}
local enemies_climb = {ENT_TYPE.MONS_FIREBUG, ENT_TYPE.MONS_MONKEY}
local enemies_ceiling = {ENT_TYPE.MONS_BAT, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_VAMPIRE, ENT_TYPE.MONS_VLAD, ENT_TYPE.MONS_HANGSPIDER}
local enemies_air = {ENT_TYPE.MONS_MOSQUITO, ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_GRUB}
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

local function enemy_small_spawn(x, y, l)
    local uid = spawn_entity_snapped_to_floor(pick(enemies_small), x, y, l)
    local ent = get_entity(uid)
    if math.random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
    end
    if math.random() < options.enemy_curse_chance/100 then
        attach_ball_and_chain(uid, 0.5, 0)
    end
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
local enemy_small_chance = define_procedural_spawn("enemy_small", enemy_small_spawn, enemy_small_valid)

local function enemy_big_spawn(x, y, l)
    local id = pick(enemies_big)
    local uid = spawn_entity_snapped_to_floor(id, x-0.5, y+0.5, l)
    local ent = get_entity(uid)
    if id == ENT_TYPE.MOUNT_MECH then
        local rider = spawn_entity(pick({ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_ALIEN}), x, y, l, 0, 0)
        carry(uid, rider)
    elseif math.random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
    end
end
local function enemy_big_valid(x, y, l)
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
        if math.random() < options.enemy_curse_chance/100 then
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
    if math.random() < options.enemy_curse_chance/100 then
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
    if math.random() < options.enemy_curse_chance/100 then
        ent:set_cursed(true)
    end
end
local function enemy_air_valid(x, y, l)
    local air = get_grid_entity_at(x, y, l)
    return air == -1
end
local enemy_air_chance = define_procedural_spawn("enemy_air", enemy_air_spawn, enemy_air_valid)

set_callback(function(ctx)
    math.randomseed(read_prng()[2])
    ctx:set_procedural_spawn_chance(enemy_small_chance, get_chance(options.enemy_min, options.enemy_max))
    ctx:set_procedural_spawn_chance(enemy_big_chance, get_chance(options.enemy_min, options.enemy_max) * 6)
    ctx:set_procedural_spawn_chance(enemy_climb_chance, get_chance(options.enemy_min, options.enemy_max) * 2)
    ctx:set_procedural_spawn_chance(enemy_ceiling_chance, get_chance(options.enemy_min, options.enemy_max) * 4)
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
    math.randomseed(read_prng()[1]+state.time_total)
    return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.MONS_REDSKELETON)

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.ABZU then return end
    ent = ent:as_movable()
    local x, y, l = get_position(ent.uid)
    math.randomseed(read_prng()[1]+state.time_total)
    spawn_entity_nonreplaceable(pick(enemies_kingu), x, y, l, math.random()*0.3-0.15, math.random()*0.1+0.1)
end, SPAWN_TYPE.SYSTEMIC, 0, {ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI, ENT_TYPE.MONS_OCTOPUS})

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.OLMEC then
        return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.MONS_UFO)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.ICE_CAVES or l ~= LAYER.BACK then
        return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(pick(enemies_small), x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.MONS_YETI)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    if state.theme ~= THEME.ICE_CAVES or l ~= LAYER.BACK then
        return spawn_entity_nonreplaceable(type, x, y, l, 0, 0)
    end
    return spawn_entity_nonreplaceable(pick(enemies_big), x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, {ENT_TYPE.MONS_YETIQUEEN, ENT_TYPE.MONS_YETIKING})

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.OLMEC then return end
    ent = ent:as_movable()
    local x, y, l = get_position(ent.uid)
    local players = get_entities_at(0, MASK.PLAYER, x, y, l, 0.5)
    if #players > 0 then return end
    spawn_entity_nonreplaceable(pick(olmec_ammo), x, y, l, math.random()*0.5-0.25, math.random()*0.1+0.1)
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
    math.randomseed(read_prng()[3])
    set_timeout(tiamat_scream, math.random(180, 480))
end

local function tiamat_attack()
    local tiamat = get_tiamat()
    if not tiamat then return end
    if tiamat.move_state == 6 then
        math.randomseed(read_prng()[7])
        local ammo = pick(tiamat_ammo)
        local vx = math.random()*0.8-0.4
        local vy = math.random()*0.8-0.4
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
    math.randomseed(read_prng()[5])
    set_timeout(tiamat_scream, 60)
    set_interval(tiamat_attack, 2)
end, ON.LEVEL)

--[[ROOMS]]
register_option_float("room_shop_chance", "Extra shop chance", 15, 0, 100)
register_option_float("room_big_chance", "Huge level chance", 15, 0, 100)
register_option_int("room_big_min", "Huge level min height", 8, 8, 15)
register_option_int("room_big_max", "Huge level max height", 15, 8, 15)

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
    math.randomseed(read_prng()[5])
    local orig_width = state.width
    local orig_height = state.height
    local exit_x = 0
    local exit_y = 3
    if state.world < 7 and not (state.world == 6 and state.level == 2) and state.width == 4 and state.height == 4 and math.random() < options.room_big_chance/100 then
        state.width = math.random(4,5)
        state.height = math.random(math.max(8, options.room_big_min), math.min(15, options.room_big_max))
        toast("My voice REALLY echoes in here!")
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
    if state.height > orig_height then
        local no_exit = true
        local x = exit_x
        local y = exit_y
        local dirs = {"left", "down", "right"}
        local last_dir = "down"
        local dir = "down"
        while no_exit do
            last_dir = dir
            if last_dir == "left" then
                dir = dirs[math.random(1, 2)]
            elseif last_dir == "right" then
                dir = dirs[math.random(2, 3)]
            else
                dir = dirs[math.random(3)]
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
    end
    for x = 0, state.width - 1 do
        for y = 0, state.height - 1 do
            local here = get_room_template(x, y, 0)
            if here == ROOM_TEMPLATE.SIDE then
                local left = get_room_template(x - 1, y, 0)
                if valid_rooms_with_shop_next[left] then
                    math.randomseed(read_prng()[6]+x+y)
                    if math.random() < options.room_shop_chance/100 then
                        ctx:set_room_template(x, y, 0, pick(new_rooms_left))
                    end
                else
                    local right = get_room_template(x + 1, y, 0)
                    if valid_rooms_with_shop_next[right] then
                        math.randomseed(read_prng()[6]+x+y)
                        if math.random() < options.room_shop_chance/100 then
                            ctx:set_room_template(x, y, 0, pick(new_rooms))
                        end
                    end
                end
            end
        end
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
    local items = get_entities_by_mask(MASK.ITEM)
    local shop_items = {}
    for i,v in ipairs(items) do
        local e = get_entity(v)
        if test_flag(e.flags, ENT_FLAG.SHOP_ITEM) then
            shop_items[#shop_items+1] = e
        end
    end
    for i,v in ipairs(shop_items) do
        math.randomseed(read_prng()[8]+i)
        v.price = math.random(1000, math.min(20000, 2*get_money())+math.random(1000, 2000))
    end
end, ON.POST_LEVEL_GENERATION)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        return spawn_entity_nonreplaceable(pick(all_shop_items), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, MASK.ITEM, shop_items)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        return spawn_entity_nonreplaceable(pick(shop_mounts), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, MASK.MOUNT, shop_mounts)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        local item = math.random(ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_EGGPLANT_CHILD)
        if item == ENT_TYPE.CHAR_CLASSIC_GUY + 1 then
            item = ENT_TYPE.CHAR_EGGPLANT_CHILD
        end
        return spawn_entity_nonreplaceable(item, x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.CHAR_HIREDHAND)

set_pre_entity_spawn(function(type, x, y, l, overlay)
    local rx, ry = get_room_index(x, y)
    local roomtype = get_room_template(rx, ry, l)
    if has(shop_rooms, roomtype) then
        math.randomseed(read_prng()[8]+math.floor(x)+math.floor(y))
        return spawn_entity_nonreplaceable(pick(all_shop_guns), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, MASK.ITEM, shop_guns)

--[[CONTAINERS]]
register_option_float("pot_chance", "Pot contents chance", 20, 0, 100)
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
         ENT_TYPE.ITEM_METAL_SHIELD}

set_post_entity_spawn(function(ent)
    ent = ent:as_container()
    math.randomseed(read_prng()[5]+ent.uid)
    if math.random() < options.pot_chance/100 then
        ent.inside = pick(pot_items)
    end
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_POT)

set_post_entity_spawn(function(ent)
    ent = ent:as_container()
    math.randomseed(read_prng()[6]+ent.uid)
    ent.inside = pick(crate_items)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_CRATE)

set_callback(function()
    local coffins = get_entities_by_type(ENT_TYPE.ITEM_COFFIN)
    for i,v in ipairs(coffins) do
        local ent = get_entity(v)
        math.randomseed(read_prng()[7]+ent.uid)
        local item = math.random(ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_EGGPLANT_CHILD)
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
    math.randomseed(read_prng()[1])
    for i,p in ipairs(players) do
        p.health = math.random(4, options.stats_health_max)
        p.inventory.bombs = math.random(4, options.stats_bombs_max)
        p.inventory.ropes = math.random(4, options.stats_ropes_max)
    end
end, ON.START)

set_callback(function()
    local ps = {}
    for i,p in ipairs(players) do
        ps[#ps+1] = p.uid
    end
    local chars = get_entities_by_mask(MASK.PLAYER)
    for i,v in ipairs(chars) do
        if not has(ps, v) then
            attach_ball_and_chain(v, 0.5, -0.5)
        end
    end
    local terras = get_entities_by_type(ENT_TYPE.MONS_MARLA_TUNNEL)
    for i,v in ipairs(terras) do
        if not has(ps, v) then
            attach_ball_and_chain(v, -1.5, -0.5)
        end
    end
end, ON.CAMP)

--[[DOORS]]
register_option_int("door_min_levels", "Min levels between midbosses", 3, 0, 100)
register_option_int("door_max_levels", "Max levels between midbosses", 6, 0, 100)
register_option_int("door_bosses", "Amount of midbosses", 4, 0, 4)

local level_order = {}

local theme = {1,2,3,5,6,7,8,9,10,11}
--local bosses = {THEME.OLMEC, THEME.ABZU, THEME.DUAT, THEME.TIAMAT, THEME.HUNDUN}
local bosses = {THEME.OLMEC, THEME.ABZU, THEME.DUAT, THEME.HUNDUN}
local world = {1,2,2,3,4,4,5,6,7,8,4,4,4,6,7,7,1}
local dead = true
local co_level = 5

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
                --set_door_target(v, nextworld, nextlevel, nexttheme)
                unlock_door_at(x, y)
            end
            --[[if not critters_spawned and critters[realtheme] ~= nil then
                spawn(critters[realtheme], x-0.7, y+0.5, layer, 0, 0)
                spawn(critters[realtheme], x+0.7, y+0.5, layer, 0, 0)
                critters_spawned = true
            end]]
        end
    end
end

local function add_level(w, l, t)
    level_order[#level_order+1] = { w = w, l = l, t = t, b = has(bosses, t) or t == THEME.TIAMAT }
end

local insert_bosses = {}
local bosses_killed = {}
local bosses_added = 0

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
            table.insert(level_order, i - math.random(0, options.door_min_levels), l)
            break
        end
    end
end

local function fix_chain()
    math.randomseed(read_prng()[1])
    insert_chain(THEME.ABZU, { w = 4, l = 2, t = THEME.TIDE_POOL, b = false})
    insert_chain(THEME.TIAMAT, { w = 6, l = 2, t = THEME.NEO_BABYLON, b = false})
end

local orig_chain_items = {ENT_TYPE.ITEM_PICKUP_CROWN, ENT_TYPE.ITEM_EXCALIBUR, ENT_TYPE.ITEM_PICKUP_TABLETOFDESTINY}
local chain_items = {}
local function get_chain_item()
    return table.remove(chain_items, 1)
end

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.OLMEC then return end
    if #chain_items > 0 then
        local x, y, l = get_position(ent.uid)
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_PICKUP_ANKH)

set_post_entity_spawn(function(ent)
    if state.theme ~= THEME.ABZU and state.theme ~= THEME.DUAT then return end
    if #chain_items > 0 then
        local x, y, l = get_position(ent.uid)
        kill_entity(ent.uid)
        spawn_entity_nonreplaceable(get_chain_item(), x, y, l, 0, 0)
    end
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_PICKUP_TABLETOFDESTINY)

local function init_run()
    --message("Started new run")
    state.level_count = 0
    math.randomseed(read_prng()[1])
    level_order = {}
    insert_bosses = {table.unpack(bosses)}
    chain_items = {table.unpack(orig_chain_items)}
    bosses_killed = {}
    bosses_added = 0
    local normal_levels = 0
    local done = false
    while not done do
        if math.random(120) == 1 then
            add_level(7, 2, THEME.EGGPLANT_WORLD)
        elseif bosses_added < options.door_bosses and #insert_bosses > 0 and normal_levels >= options.door_min_levels and math.random(options.door_max_levels) <= normal_levels then
            normal_levels = 0
            local t = insert_bosses[math.random(#insert_bosses)]
            local l = 4
            if t == THEME.OLMEC then
                l = 1
            end
            local w = world[t]
            add_level(w, l, t)
            add_boss(t)
        elseif (bosses_added >= options.door_bosses or #insert_bosses == 0) and normal_levels >= options.door_min_levels and math.random(options.door_max_levels) <= normal_levels then
            add_level(6, 4, THEME.TIAMAT)
            done = true
        else
            normal_levels = normal_levels+1
            local t = theme[math.random(#theme)]
            local l = 1
            if t == THEME.NEO_BABYLON or t == THEME.SUNKEN_CITY then
                l = math.random(3)
            elseif t == THEME.CITY_OF_GOLD then
                l = 3
            elseif t == THEME.COSMIC_OCEAN then
                l = math.random(5, 97)
            elseif t == THEME.ICE_CAVES then
                l = 1
            else
                l = math.random(4)
            end
            local w = world[t]
            add_level(w, l, t)
        end
    end
    fix_chain()
    --[[level_order = {
        { w = 2, l = 4, t = THEME.JUNGLE, b = false },
        { w = 8, l = 42, t = THEME.COSMIC_OCEAN, b = false },
        { w = 6, l = 4, t = THEME.TIAMAT, b = true }
    }]]
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
    if state.screen_next ~= ON.LEVEL then return end
    if (#level_order == 0 or test_flag(state.quest_flags, 1)) then
        --message("Running init")
        init_run()
    end
    local num = state.level_count+1
    if test_flag(state.quest_flags, 1) then
        num = 1
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
end, ON.LOADING)

set_callback(function()
    --message("Reset - Init, state.reset == "..tostring(state.reset))
    --init_run()
end, ON.RESET)

set_callback(function()
    --message("Camp - Init")
    --init_run()
end, ON.CAMP)

set_callback(function()
    --message("Death - Init")
    toast("Died after "..tostring(state.level_count).." levels!\nBosses remaining: "..tostring(bosses_left()))
    --dead = true
end, ON.DEATH)

local ending_timer = 0
local ending_cb = -1
set_callback(function()
    if state.theme == THEME.TIAMAT and bosses_left() > 0 then
        ending_timer = 0
        ending_cb = set_global_interval(function()
            ending_timer = ending_timer + 1
            if ending_timer == 768 then
                players[1]:light_on_fire()
            elseif ending_timer == 930 then
                kill_entity(players[1].uid)
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

--[[
TODO:
mount & hh prices in shops
duat snaptraps
chain
]]
