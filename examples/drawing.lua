meta.name = "Drawing example"
meta.version = "WIP"
meta.description = "Draws some shapes and text."
meta.author = "Dregu"

-- r/uselessredcircle
set_callback(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    sx, sy = screen_position(x, y)
    radius = screen_distance(1) -- one tile
    draw_circle(sx, sy, radius, 3, rgba(255, 0, 0, 255))
end, ON.GUIFRAME)

-- no players allowed
set_callback(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    sx, sy = screen_position(x-0.5, y-0.5)
    sx2, sy2 = screen_position(x+0.5, y+0.5)
    draw_line(sx, sy, sx2, sy2, 3, rgba(255, 0, 0, 255))
end, ON.GUIFRAME)

-- custom yellow rounded hitbox
set_callback(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    sx, sy = screen_position(x-players[1].hitboxx+players[1].offsetx, y+players[1].hitboxy+players[1].offsety) -- top left
    sx2, sy2 = screen_position(x+players[1].hitboxx+players[1].offsetx, y-players[1].hitboxy+players[1].offsety) -- bottom right
    draw_rect(sx, sy, sx2, sy2, 2, 10, rgba(255, 255, 0, 255))
end, ON.GUIFRAME)

-- entity uid and health
set_callback(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    ents = get_entities_at(0, 255, x, y, l, 30)
    for i,v in ipairs(ents) do
        x, y, l = get_position(v)
        e = get_entity(v):as_movable()
        sx, sy = screen_position(x-e.hitboxx, y-e.hitboxy+e.offsety)
        if e.health > 0 then
            draw_text(sx, sy, 0, tostring(v).."\n"..tostring(e.health).." HP", rgba(255, 255, 255, 255))
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

-- hilight shop walls
set_callback(function()
    if #players < 1 then return end
    px, py, pl = get_position(players[1].uid)
    ents = get_entities_by_mask(0x100)
    for i,v in ipairs(ents) do
        x, y, l = get_position(v)
        e = get_entity(v):as_movable()
        if test_flag(e.flags, 24) and l == pl then
            sx, sy = screen_position(x-e.hitboxx+e.offsetx, y+e.hitboxy+e.offsety) -- top left
            sx2, sy2 = screen_position(x+e.hitboxx+e.offsetx, y-e.hitboxy+e.offsety) -- bottom right
            draw_rect(sx, sy, sx2, sy2, 2, 0, rgba(255, 255, 0, 200))
        end
    end
end, ON.GUIFRAME)

-- level boundaries
set_callback(function()
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(xmin, ymin) -- top left
    sx2, sy2 = screen_position(xmax, ymax) -- bottom right
    draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
end, ON.GUIFRAME)

-- random shapes
set_callback(function()
    draw_rect_filled(-0.6, -0.6, -0.8, -0.8, 0, rgba(255, 0, 0, 80))
    draw_rect(-0.8, -0.8, -0.6, -0.6, 2, 0, rgba(255, 255, 255, 255))
    draw_circle_filled(-0.7, -0.7, 0.1, rgba(255, 0, 0, 128))
end, ON.GUIFRAME)
