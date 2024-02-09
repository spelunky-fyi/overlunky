meta = {
    name = "Spawn Type Analyzer",
    description =
    "Colors all entities based on their SPAWN_TYPE, shows floor style spreading (magenta), corner filling/traps spawning in empty space (red), traps replacing floor (green), 50% tiles and original tilecodes for all the peculiar spots.",
    version = "1.1",
    author = "Dregu"
}

for k, v in pairs(MASK) do
    if v > 0 then
        register_option_bool(k, k, test_mask(MASK.FLOOR | MASK.ACTIVEFLOOR, v))
    end
end

a = 0.8

colors = {
    [SPAWN_TYPE.LEVEL_GEN_TILE_CODE] = Color:new(0, 0.3, 0.8, 0),
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
            if test_mask(ent.type.search_flags, MASK.FLOOR) and (not map[ent.layer] or not map[ent.layer][math.floor(ent.abs_x)] or map[ent.layer][math.floor(ent.abs_x)][math.floor(ent.abs_y)] == "empty") then
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
        local ax, ay, bx, by = get_bounds()
        for _, uid in pairs(get_entities_by(0, mask, state.camera_layer)) do
            local x, y, l = get_position(uid)
            if uids[uid] and uids[uid].a ~= 0 and x > ax and x < bx and y < ay and y > by then
                ctx:draw_world_rect(get_hitbox(uid, -0.15), 20, uids[uid])
                if map[l] and map[l][x] and map[l][x][y] then
                    local sx, sy = screen_position(x, y - 0.2)
                    ctx:draw_text(F "{map[l][x][y]}", sx, sy, 0.0003, 0.0003, Color:white(),
                        VANILLA_TEXT_ALIGNMENT.CENTER,
                        VANILLA_FONT_STYLE.NORMAL)
                end
            end
        end

        for _, c in pairs(perc) do
            if c.l == state.camera_layer then
                local sx, sy = screen_position(c.x, c.y + 0.2)
                ctx:draw_text(F "{c.c}%", sx, sy, 0.0004, 0.0004, Color:white(), VANILLA_TEXT_ALIGNMENT.CENTER,
                    VANILLA_FONT_STYLE.NORMAL)
                if map[c.l] and map[c.l][c.x] and map[c.l][c.x][c.y] then
                    sx, sy = screen_position(c.x, c.y - 0.2)
                    ctx:draw_text(F "{map[c.l][c.x][c.y]}", sx, sy, 0.0003, 0.0003, Color:white(),
                        VANILLA_TEXT_ALIGNMENT.CENTER,
                        VANILLA_FONT_STYLE.NORMAL)
                end
            end
        end
    end
end, ON.RENDER_POST_DRAW_DEPTH)

set_callback(function(rx, ry, t, ctx)
    for l = 0, 1 do
        for y = 0, 7 do
            for x = 0, 9 do
                local code = ctx:get_short_tile_code(x, y, l)
                if code then
                    local def = get_short_tile_code_definition(code)
                    local name = string.lower(enum_get_name(TILE_CODE, def.tile_code))
                    local lx, ly = get_room_pos(rx, ry)
                    lx = math.floor(lx + x + 0.5)
                    ly = math.floor(ly - y - 0.5)
                    if def.chance ~= 100 then
                        perc[#perc + 1] = { l = l, x = lx, y = ly, c = def.chance }
                    end
                    if not map[l] then map[l] = {} end
                    if not map[l][lx] then map[l][lx] = {} end
                    map[l][lx][ly] = name
                end
            end
        end
    end
end, ON.PRE_HANDLE_ROOM_TILES)
