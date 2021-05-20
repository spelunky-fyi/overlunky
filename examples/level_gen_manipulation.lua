meta = {
	name = "Level Gen",
	version = "WIP",
	description = "Example for level gen manipulation",
	author = "Malacath"
}

-- Adds a new tilecode that just spawns a mech with a caveman
define_tile_code("cave_mech")
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
    local ent_uid = spawn_entity(ENT_TYPE.ITEM_ICESPIRE, x, y, layer, 0.0, 0.0);
    local ent = get_entity(ent_uid)
    local ent_flags = ent.flags
    ent_flags = clr_flag(ent_flags, 18)
    if math.random(2) == 2 then
        ent_flags = set_flag(ent_flags, 17)
    end
    ent.flags = ent_flags
    ent.animation_frame = 13 * 16 + 13
    local mov = ent:as_movable()
    mov.offsety = mov.offsety + 0.1
    return true
end, "spikes")

-- Puts food on the table
set_pre_tile_code_callback(function(x, y, layer)
    spawn_entity(ENT_TYPE.ITEM_PICKUP_GIANTFOOD, x, y + 0.5, layer, 0.0, 0.0);
    spawn_entity(ENT_TYPE.ITEM_PICKUP_GIANTFOOD, x + 1.0, y + 0.5, layer, 0.0, 0.0);
    return false
end, "diningtable")

-- Randomly distributes treasure in minewood_floor
set_post_tile_code_callback(function(x, y, layer)
    local rand = math.random(100)
    if rand > 65 then
        local ents = get_entities_overlapping(ENT_TYPE.FLOORSTYLED_MINEWOOD, 0, x - 0.45, y - 0.45, x + 0.45, y + 0.45, layer);
        if #ents == 1 then -- if not 1 then something else was spawned here already
            local entity_type = nil
            local ent_uid = nil
            if rand > 95 then
                -- TODO: Fix spawning entities in the floor
                -- entity_type = ENT_TYPE.ITEM_CRATE
                entity_type = ENT_TYPE.EMBED_GOLD_BIG
            elseif rand > 80 then
                entity_type = ENT_TYPE.EMBED_GOLD_BIG
            else
                entity_type = ENT_TYPE.EMBED_GOLD
            end
            local ent_uid = spawn_entity_over(entity_type, ents[1], 0, 0)
            --local ent = get_entity(ent_uid)
            --local ent_flags = ent.flags
            --ent_flags = set_flag(ent_flags, 10)
            --ent_flags = clr_flag(ent_flags, 13)
            --ent.flags = ent_flags
        end
    end
end, "minewood_floor")

-- Render all tiles where a lamp was spawned
local lamp_tiles = {}
set_post_tile_code_callback(function(x, y, layer)
    table.insert(lamp_tiles, { x = x, y = y, layer = layer })
end, "lamp_hang")
set_callback(function()
    for _, tile in pairs(lamp_tiles) do
        local sx, sy = screen_position(tile.x - 0.5, tile.y + 0.5) -- top left
        local sx2, sy2 = screen_position(tile.x + 0.5, tile.y - 0.5) -- bottom right
        draw_rect(sx, sy, sx2, sy2, 2, 0, rgba(255, 0, 255, 255))
    end
end, ON.GUIFRAME)
