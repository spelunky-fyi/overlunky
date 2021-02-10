-- r/uselessredcircle
set_callback(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    sx, sy = screen_position(x, y)
    draw_circle(sx, sy, 80, 3, color(255, 0, 0, 255))
end, ON.GUIFRAME)

-- no players allowed
set_callback(function()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    sx, sy = screen_position(x-0.5, y-0.5)
    sx2, sy2 = screen_position(x+0.5, y+0.5)
    draw_line(sx, sy, sx2, sy2, 3, color(255, 0, 0, 255))
end, ON.GUIFRAME)

-- drawing needs a bit more work to normalize things on different resolutions...
