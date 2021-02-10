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
        e = get_entity(v)
        sx, sy = screen_position(x-e.hitboxx, y-e.hitboxy-0.2)
        if e.health > 0 then
            draw_text(sx, sy, tostring(v).."\n"..tostring(e.health).." HP", rgba(255, 255, 255, 255))
        end
    end
end, ON.GUIFRAME)
