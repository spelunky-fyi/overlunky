meta = {
    name = "Spawn Type Analyzer",
    description =
    "Colors all entities based on their SPAWN_TYPE, shows floor type spreading (magenta), floor filling/traps spawning in empty space (red), traps replacing floor (green), 50% tiles etc...",
    version = "1.0",
    author = "Dregu"
}

for k, v in pairs(MASK) do
    if v > 0 then
        register_option_bool(k, k, v == MASK.FLOOR and true or false)
    end
end

a = 0.8

colors = {
    [SPAWN_TYPE.LEVEL_GEN_TILE_CODE] = Color:new(0, 0.7, 1, a),
    [SPAWN_TYPE.LEVEL_GEN_PROCEDURAL] = Color:new(1, 1, 0, a),
    [SPAWN_TYPE.LEVEL_GEN_GENERAL] = Color:new(0, 1, 0, a),
    [SPAWN_TYPE.LEVEL_GEN_FLOOR_SPREADING] = Color:new(1, 0, 1, a),
    [SPAWN_TYPE.SYSTEMIC] = Color:new(1, 1, 1, a),
    [64] = Color:new(1, 0, 0, a),
}

uids = {}
perc = {}
map = {}

set_callback(function()
    uids = {}
    perc = {}
    map = {}
end, ON.PRE_LEVEL_GENERATION)

set_post_entity_spawn(function(ent, type)
    for k, v in pairs(colors) do
        if test_mask(type, k) then
            uids[ent.uid] = v
            if test_mask(ent.type.search_flags, MASK.FLOOR) and (not map[math.floor(ent.abs_x)] or map[math.floor(ent.abs_x)][math.floor(ent.abs_y)] == "empty") then
                uids[ent.uid] = colors[64]
            end
        end
    end
end, SPAWN_TYPE.ANY, MASK.ANY)

set_callback(function(ctx, d)
    mask = 0
    for k, v in pairs(MASK) do
        if options[k] then mask = set_mask(mask, v) end
    end
    if d == 5 then
        for _, uid in pairs(get_entities_by(0, mask, state.camera_layer)) do
            if uids[uid] then
                ctx:draw_world_rect(get_hitbox(uid, -0.2), 20, uids[uid])
            end
        end

        for _, c in pairs(perc) do
            local sx, sy = screen_position(c.x, c.y)
            ctx:draw_text(F "{c.c}%", sx, sy, 0.0004, 0.0004, Color:white(), VANILLA_TEXT_ALIGNMENT.CENTER,
                VANILLA_FONT_STYLE.NORMAL)
        end
    end
end, ON.RENDER_POST_DRAW_DEPTH)

set_callback(function(rx, ry, t, ctx)
    for y = 0, 7 do
        for x = 0, 9 do
            local code = ctx:get_short_tile_code(x, y, 0)
            local def = get_short_tile_code_definition(code)
            local name = string.lower(enum_get_name(TILE_CODE, def.tile_code))
            local lx, ly = get_room_pos(rx, ry)
            lx = math.floor(lx + x + 0.5)
            ly = math.floor(ly - y - 0.5)
            if def.chance ~= 100 then
                perc[#perc + 1] = { x = lx, y = ly, c = def.chance }
            end
            if not map[lx] then map[lx] = {} end
            map[lx][ly] = name
        end
    end
end, ON.PRE_HANDLE_ROOM_TILES)
