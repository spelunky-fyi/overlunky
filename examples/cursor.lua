meta.name = "HD Cursor"
meta.description = [[Replace the cursor with the HD cursor!

Note: The feature is enabled by default natively in Overlunky
and doesn't need these files, this is just an example how to do it with lua
]]
meta.version = "1.0"
meta.author = "Dregu"

iio = get_io()

-- load the image
cursor, width, height = create_image("cursor.png")

last_ms, last_mx, last_my = 0, 0, 0

if cursor >= 0 then
    set_callback(function(ctx)
        local mx, my = mouse_position()

        -- update cursor hiding timer
        if mx ~= last_mx or my ~= last_my then last_ms = get_ms() end

        -- draw on top of UI windows, including all platform windows outside the game
        ctx:draw_foreground(true)

        -- check for recent movement
        if get_ms() - last_ms < 2000 then
            -- draw the cursor
            ctx:draw_image(cursor, mx, my, mx + 0.05, my - 0.05 / 9 * 16, 0, 0, 1, 1, 0xffffffff)
        end

        -- hide normal cursor
        iio.showcursor = false

        last_mx, last_my = mx, my
    end, ON.GUIFRAME)
end
