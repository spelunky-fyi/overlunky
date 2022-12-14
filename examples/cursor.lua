meta.name = "HD Cursor"
meta.description = "Replace the cursor with the HD cursor!"
meta.version = "1.0"
meta.author = "Dregu"

cursor, width, height = create_image("cursor.png")
iio = get_io()
set_callback(function(ctx)
    local mx, my = mouse_position()
    ctx:draw_foreground(true)
    ctx:draw_image(cursor, mx, my, mx + 0.05, my - 0.05 / 9 * 16, 0, 0, 1, 1, 0xffffffff)
    if iio.showcursor then iio.showcursor = false end
end, ON.GUIFRAME)
