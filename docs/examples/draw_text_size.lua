-- draw text
set_callback(function(draw_ctx)
    -- get a random color
    local color = math.random(0, 0xffffffff)
    -- zoom the font size based on frame
    local size = (get_frame() % 199)+1
    local text = 'Awesome!'
    -- calculate size of text
    local w, h = draw_text_size(size, text)
    -- draw to the center of screen
    draw_ctx:draw_text(0-w/2, 0-h/2, size, text, color)
end, ON.GUIFRAME)
