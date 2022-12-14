-- Draw the level boundaries
set_callback(function(draw_ctx)
    local xmin, ymax, xmax, ymin = get_bounds()
    local sx, sy = screen_position(xmin, ymax) -- top left
    local sx2, sy2 = screen_position(xmax, ymin) -- bottom right
    draw_ctx:draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
end, ON.GUIFRAME)
