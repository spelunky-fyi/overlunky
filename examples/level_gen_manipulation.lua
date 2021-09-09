meta = {
	name = "Level Gen",
	version = "WIP",
	description = "Example for level gen manipulation",
	author = "Malacath"
}

-- Adds a new tilecode that just spawns a mech with a caveman
local cave_mech_tile_code = define_tile_code("cave_mech")
set_pre_tile_code_callback(function(x, y, layer)
    local mount_id = spawn_entity(ENT_TYPE.MOUNT_MECH, x, y, layer, 0.0, 0.0)
    local rider_id = spawn_entity(ENT_TYPE.MONS_CAVEMAN, x, y, layer, 0.0, 0.0)
    carry(mount_id, rider_id)
    return false
end, "cave_mech")

-- Replaces all "floor" tile codes with cog floor
set_pre_tile_code_callback(function(x, y, layer)
    spawn_entity(ENT_TYPE.FLOORSTYLED_COG, x, y, layer, 0.0, 0.0);
    return true
end, "floor")

-- Replaces all spikes with snowmen
set_pre_tile_code_callback(function(x, y, layer)
    local ent_uid = spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_ICESPIRE, x, y, layer, 0.0, 0.0);
    local ent = get_entity(ent_uid)
    local ent_flags = ent.flags
    ent_flags = clr_flag(ent_flags, 18)
    if math.random(2) == 2 then
        ent_flags = set_flag(ent_flags, 17)
    end
    ent.flags = ent_flags
    ent.animation_frame = 13 * 16 + 13
    ent.offsety = ent.offsety + 0.1
    return true
end, "spikes")

-- Puts food on the table
set_pre_tile_code_callback(function(x, y, layer)
    spawn_entity(ENT_TYPE.ITEM_PICKUP_GIANTFOOD, x, y + 0.5, layer, 0.0, 0.0);
    spawn_entity(ENT_TYPE.ITEM_PICKUP_GIANTFOOD, x + 1.0, y + 0.5, layer, 0.0, 0.0);
    -- returns nil, the result is the same as returning false
end, "diningtable")

-- Randomly distributes treasure in minewood_floor
set_post_tile_code_callback(function(x, y, layer)
    local rand = math.random(100)
    if rand > 65 then
        local aabb = AABB:new():offset(x, y):extrude(0.45)
        local ents = get_entities_overlapping_hitbox(ENT_TYPE.FLOORSTYLED_MINEWOOD, 0, aabb, layer);
        if #ents == 1 then -- if not 1 then something else was spawned here already
            local entity_type = nil
            local ent_uid = nil
            if rand > 95 then
                entity_type = ENT_TYPE.ITEM_JETPACK
            elseif rand > 80 then
                entity_type = ENT_TYPE.EMBED_GOLD_BIG
            else
                entity_type = ENT_TYPE.EMBED_GOLD
            end

            -- thanks to ha#9722 for the embed code
            -- note, embed code does not work properly for all entities!
            if entity_type ~= ENT_TYPE.EMBED_GOLD and entity_type ~= ENT_TYPE.EMBED_GOLD_BIG then
                local entity_db = get_type(entity_type)
                local previous_draw, previous_flags = entity_db.draw_depth, entity_db.default_flags
                entity_db.draw_depth = 9
                entity_db.default_flags = set_flag(entity_db.default_flags, 1)
                entity_db.default_flags = set_flag(entity_db.default_flags, 4)
                entity_db.default_flags = set_flag(entity_db.default_flags, 10)
                entity_db.default_flags = clr_flag(entity_db.default_flags, 13)

                local entity = get_entity(spawn_entity_over(entity_type, ents[1], 0, 0))

                entity_db.draw_depth = previous_draw
                entity_db.default_flags = previous_flags
            else
                spawn_entity_over(entity_type, ents[1], 0, 0)
            end
        end
    end
end, "minewood_floor")

-- Render all tiles where a lamp was spawned
local lamp_tiles = {}
set_post_tile_code_callback(function(x, y, layer)
    table.insert(lamp_tiles, { x = x, y = y, layer = layer })
end, "lamp_hang")
set_callback(function(draw_ctx)
    for _, tile in pairs(lamp_tiles) do
        local sx, sy = screen_position(tile.x - 0.5, tile.y + 0.5) -- top left
        local sx2, sy2 = screen_position(tile.x + 0.5, tile.y - 0.5) -- bottom right
        draw_ctx:draw_rect(sx, sy, sx2, sy2, 2, 0, rgba(255, 0, 255, 255))
    end
end, ON.GUIFRAME)

-- Add a chance for powderkeg spawns, needs to be used as `\?sample_powderkeg 120` in a .lvl file
local function spawn_powderkeg(x, y, l)
    spawn_entity(ENT_TYPE.ACTIVEFLOOR_POWDERKEG, x, y, l, 0, 0)
end
local function is_valid_powderkeg_spawn(x, y, l)
    -- Only spawn where the powderkeg has floor left or right of it
    local not_entity_here = get_grid_entity_at(x, y, l) == -1
    if not_entity_here then
        local entity_below = get_grid_entity_at(x, y - 1, l) >= 0
        if entity_below then
            local entity_left = get_grid_entity_at(x - 1, y, l) >= 0
            local entity_right = get_grid_entity_at(x + 1, y, l) >= 0
            return entity_left ~= entity_right
        end
    end
    return false
end
define_procedural_spawn("sample_powderkeg", spawn_powderkeg, is_valid_powderkeg_spawn)

-- Spawn spark traps on every level
local function spawn_sparktrap(x, y, l)
    spawn_grid_entity(ENT_TYPE.FLOOR_SPARK_TRAP, x, y, l)
end
local function is_valid_sparktrap_spawn(x, y, l)
    -- Only spawn where there is floor
    local floor = get_grid_entity_at(x, y, l)
    if floor ~= -1 then
        floor = get_entity(floor)
        return test_flag(floor.flags, 3) 
    end
    return false
end
local sparktrap_chance = define_procedural_spawn("sample_sparktrap", spawn_sparktrap, is_valid_sparktrap_spawn)
set_callback(function(room_gen_ctx)
    local current_sparktrap_chance = get_procedural_spawn_chance(PROCEDURAL_CHANCE.SPARKTRAP_CHANCE)
    if current_sparktrap_chance == 0 then
        current_sparktrap_chance = 20
    end

    prinspect(current_sparktrap_chance)
    room_gen_ctx:set_procedural_spawn_chance(sparktrap_chance, current_sparktrap_chance)
    -- Disable the original sparktrap spawns so we don't get double spawns
    room_gen_ctx:set_procedural_spawn_chance(PROCEDURAL_CHANCE.SPARKTRAP_CHANCE, 0)
end, ON.POST_ROOM_GENERATION)

-- Fill in shops in all the valid ROOM_TEMPLATE.SIDE spots
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
set_callback(function(room_gen_ctx)
    for x = 0, state.width - 1 do
        for y = 0, state.height - 1 do
            -- Check that this is a side
            local room_template_here = get_room_template(x, y, 0)
            if room_template_here == ROOM_TEMPLATE.SIDE then
                -- Check if left of this is a valid room
                local room_template_left = get_room_template(x - 1, y, 0)
                if valid_rooms_with_shop_next[room_template_left] then
                    -- And spawn a shop facing left
                    room_gen_ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.SHOP_LEFT)
                else
                    -- Or if right of it is
                    local room_template_right = get_room_template(x + 1, y, 0)
                    if valid_rooms_with_shop_next[room_template_right] then
                        -- And spawn a shop facing right
                        room_gen_ctx:set_room_template(x, y, 0, ROOM_TEMPLATE.SHOP)
                    end
                end
            end
        end
    end
end, ON.POST_ROOM_GENERATION)

-- Spawn exactly 3 radios in each dwelling level in the front layer and one in the back layer
local radio_spawns = define_extra_spawn(function(x, y, l)
    spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_BOOMBOX, x, y, l)
end, nil, 3, 1)
set_callback(function(ctx)
    if state.theme == THEME.DWELLING then
        ctx:set_num_extra_spawns(radio_spawns, 3, 1)
    else
        ctx:set_num_extra_spawns(radio_spawns, 0, 0)
    end
end, ON.POST_ROOM_GENERATION)

-- Create a caveman_asleep50%cave_mech short tile code
local random_cave_mech_code
set_callback(function(ctx)
    if state.theme == THEME.DWELLING then
        local short_tile_code = ShortTileCodeDef:new()
        short_tile_code.tile_code = TILE_CODE.CAVEMAN_ASLEEP
        short_tile_code.alt_tile_code = cave_mech_tile_code
        short_tile_code.chance = 50
        random_cave_mech_code = ctx:define_short_tile_code(short_tile_code)
    end
end, ON.POST_ROOM_GENERATION)

-- Replace sleeping caveman tiles with caveman_asleep50%cave_mech
set_callback(function(x, y, room_template, ctx)
    if state.theme == THEME.DWELLING then
        -- Note: Don't do this! There is a specific function for replacing short tile codes, use that instead.
        -- This is just an example for using `ctx:get_short_tile_code` and `ctx:set_short_tile_code`
        for tx = 0, CONST.ROOM_WIDTH - 1 do
            for ty = 0, CONST.ROOM_HEIGHT - 1 do
                local short_tile_code = ctx:get_short_tile_code(tx, ty, LAYER.FRONT)
                local short_tile_code_def = get_short_tile_code_definition(short_tile_code)
                if short_tile_code_def then
                    if short_tile_code_def.tile_code == TILE_CODE.CAVEMAN_ASLEEP then
                        ctx:set_short_tile_code(x, y, LAYER.FRONT, random_cave_mech_code)
                    end
                end
            end
        end
    end
end, ON.PRE_HANDLE_ROOM_TILES)

-- Add a backroom to the altar room that contains an altar, front layer only contains a idol altar
set_callback(function(x, y, room_template, ctx)
    if room_template == ROOM_TEMPLATE.ALTAR and not ctx:has_back_layer() then
        local altar_short_code
        local bone_altar_short_code
        local floor_short_code
        local floor_50_short_code
        local floor_hard_short_code
        local door2_short_code

        do
            local short_code_def = ShortTileCodeDef:new()
            short_code_def.tile_code = TILE_CODE.ALTAR
            altar_short_code = get_short_tile_code(short_code_def)
            short_code_def.tile_code = TILE_CODE.IDOL_FLOOR
            bone_altar_short_code = get_short_tile_code(short_code_def)
            short_code_def.tile_code = TILE_CODE.FLOOR
            floor_short_code = get_short_tile_code(short_code_def)
            short_code_def.chance = 50
            floor_50_short_code = get_short_tile_code(short_code_def)
            short_code_def.tile_code = TILE_CODE.FLOOR_HARD
            short_code_def.chance = 100
            floor_hard_short_code = get_short_tile_code(short_code_def)
            short_code_def.tile_code = TILE_CODE.DOOR2
            door2_short_code = get_short_tile_code(short_code_def)
        end

        -- All of this should be available in generic.lvl but who knows
        if not altar_short_code or
            not floor_short_code or
            not floor_50_short_code or
            not floor_hard_short_code or
            not door2_short_code then

            print("Did not find some of the required short tile codes")
            return
        end

        ctx:add_copied_back_layer()
        ctx:replace_short_tile_code(LAYER.BACK, floor_short_code, floor_hard_short_code)
        ctx:replace_short_tile_code(LAYER.BACK, floor_50_short_code, floor_hard_short_code)
        ctx:replace_short_tile_code(LAYER.FRONT, altar_short_code, bone_altar_short_code)

        local altar_positions = ctx:find_all_short_tile_codes(LAYER.BACK, altar_short_code)
        for _, pos in pairs(altar_positions) do
            local tx, ty, _ = table.unpack(pos)
            if ctx:get_short_tile_code(tx + 1, ty, LAYER.BACK) == altar_short_code then
                ctx:set_short_tile_code(tx - 1, ty, LAYER.BOTH, door2_short_code)
            else
                ctx:set_short_tile_code(tx + 1, ty, LAYER.BOTH, door2_short_code)
            end
        end
    end
end, ON.PRE_HANDLE_ROOM_TILES)
