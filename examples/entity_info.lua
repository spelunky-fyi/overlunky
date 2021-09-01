meta.name = "Entity info"
meta.version = "WIP"
meta.description = "A bunch of information about entities around you."
meta.author = "Dregu"

names = {}
for i,v in pairs(ENT_TYPE) do
  names[v] = i
end

-- entity uid, health, distance
set_callback(function(draw_ctx)
    if #players < 1 then return end
    x, y, l = get_render_position(players[1].uid)
    mask = MASK.PLAYER | MASK.MOUNT | MASK.MONSTER | MASK.ITEM | MASK.EXPLOSION | MASK.ROPE | MASK.FX | MASK.ACTIVEFLOOR
    ents = get_entities_at(0, mask, x, y, l, 30)
    for i,v in ipairs(ents) do
        ex, ey, el = get_render_position(v)
        e = get_entity(v)
        if e ~= nil then
            hitbox = get_render_hitbox(v)
            sx, sy = screen_position(hitbox.left, hitbox.bottom)
            dist = distance(players[1].uid, v)
            c = ""
            if names[e.inside] and (e.type.id == ENT_TYPE.ITEM_CRATE or e.type.id == ENT_TYPE.ITEM_DMCRATE or e.type.id == ENT_TYPE.ITEM_COFFIN or e.type.id == ENT_TYPE.ITEM_PRESENT or e.type.id == ENT_TYPE.ITEM_GHIST_PRESENT or e.type.id == ENT_TYPE.ITEM_POT) then
                c = " ("..names[e.inside]..")"
            end
            if not string.match(names[e.type.id], "FX") then
                draw_ctx:draw_text(sx, sy, 0,
                    tostring(v).."\n"..
                    names[e.type.id]..c.."\n"..
                    tostring(e.health).." HP\n"..
                    "D: "..string.format("%.3f", dist), rgba(255, 255, 255, 255))
            end
        end
    end

    ents = get_entities_at(0, MASK.FLOOR, x, y, l, 30)
    for i,v in ipairs(ents) do
        e = get_entity(v)
        if e ~= nil and string.match(names[e.type.id], "TRAP") then
            hitbox = get_render_hitbox(v)
            sx, sy = screen_position(hitbox.left, hitbox.bottom)
            dist = distance(players[1].uid, v)
            draw_ctx:draw_text(sx, sy, 0,
                tostring(v).."\n"..
                names[e.type.id].."\n"..
                "D: "..string.format("%.3f", dist), rgba(255, 255, 255, 255))
        end
    end
end, ON.GUIFRAME)

-- door finder
set_callback(function(draw_ctx)
    if #players < 1 then return end
    px, py, pl = get_render_position(players[1].uid)
    ents = get_entities_by_type(ENT_TYPE.LOGICAL_DOOR)
    for i,v in ipairs(ents) do
        x, y, l = get_render_position(v)
        e = get_entity(v)
        hitbox = get_render_hitbox(v)
        sx, sy = screen_position(hitbox.left, hitbox.bottom)
        if l == pl then
            draw_ctx:draw_text(sx, sy, 0, tostring(v), rgba(255, 255, 255, 255))
        end
    hitbox = get_render_hitbox(v)
    hitbox = screen_aabb(hitbox)
    draw_ctx:draw_rect(hitbox, 2, 10, rgba(255, 0, 255, 255))
    end
end, ON.GUIFRAME)
