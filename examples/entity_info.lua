meta.name = "Entity info"
meta.version = "WIP"
meta.description = "A bunch of information about entities around you."
meta.author = "Dregu"

names = {}
for i,v in pairs(ENT_TYPE) do
  names[v] = i
end

-- entity uid, health, distance
set_callback(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    ents = get_entities_at(0, 255, x, y, l, 30)
    for i,v in ipairs(ents) do
        ex, ey, el = get_position(v)
        e = get_entity(v)
        if e ~= nil then
            e = e:as_container()
            sx, sy = screen_position(ex-e.hitboxx, ey-e.hitboxy+e.offsety)
            dist = distance(players[1].uid, v)
            c = ""
            if names[e.inside] and (e.type.id == ENT_TYPE.ITEM_CRATE or e.type.id == ENT_TYPE.ITEM_DMCRATE or e.type.id == ENT_TYPE.ITEM_COFFIN or e.type.id == ENT_TYPE.ITEM_PRESENT or e.type.id == ENT_TYPE.ITEM_GHIST_PRESENT or e.type.id == ENT_TYPE.ITEM_POT) then
                c = " ("..names[e.inside]..")"
            end
            if not string.match(names[e.type.id], "FX") then
                draw_text(sx, sy, 0,
                    tostring(v).."\n"..
                    names[e.type.id]..c.."\n"..
                    tostring(e.health).." HP\n"..
                    "D: "..string.format("%.3f", dist), rgba(255, 255, 255, 255))
            end
        end
    end

    ents = get_entities_at(0, 0x100, x, y, l, 30)
    for i,v in ipairs(ents) do
        e = get_entity(v)
        if e ~= nil and string.match(names[e.type.id], "TRAP") then
            e = e:as_movable()
            ex, ey, el = get_position(v)
            sx, sy = screen_position(ex-e.hitboxx, ey-e.hitboxy+e.offsety)
            dist = distance(players[1].uid, v)
            draw_text(sx, sy, 0,
                tostring(v).."\n"..
                names[e.type.id].."\n"..
                "D: "..string.format("%.3f", dist), rgba(255, 255, 255, 255))
        end
    end
end, ON.GUIFRAME)

-- door finder
set_callback(function()
    if #players < 1 then return end
    px, py, pl = get_position(players[1].uid)
    ents = get_entities_by_type(ENT_TYPE.LOGICAL_DOOR)
    for i,v in ipairs(ents) do
        x, y, l = get_position(v)
        e = get_entity(v):as_movable()
        sx, sy = screen_position(x-e.hitboxx, y-e.hitboxy+e.offsety)
        if l == pl then
            draw_text(sx, sy, 0, tostring(v), rgba(255, 255, 255, 255))
        end
    sx, sy = screen_position(x-e.hitboxx+e.offsetx, y+e.hitboxy+e.offsety) -- top left
    sx2, sy2 = screen_position(x+e.hitboxx+e.offsetx, y-e.hitboxy+e.offsety) -- bottom right
    draw_rect(sx, sy, sx2, sy2, 2, 10, rgba(255, 0, 255, 255))
    end
end, ON.GUIFRAME)
