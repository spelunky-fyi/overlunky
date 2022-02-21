-- Draw the level boundaries
set_callback(function(draw_ctx)
    local xmin, ymin, xmax, ymax = get_bounds()
    local sx, sy = screen_position(xmin, ymin) -- top left
    local sx2, sy2 = screen_position(xmax, ymax) -- bottom right
    draw_ctx:draw_rect(sx, sy, sx2, sy2, 4, 0, rgba(255, 255, 255, 255))
end, ON.GUIFRAME)
