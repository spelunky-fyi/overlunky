meta.name = "HD Cursor"
meta.description = "Replace the cursor with the HD cursor!"
meta.version = "1.0"
meta.author = "Dregu"

-- this script is enabled by default in overlunky user interface options
-- if it's installed in the default script directory

iio = get_io()

-- load the image
cursor, width, height = create_image("cursor.png")

last_ms, last_mx, last_my = 0, 0, 0

if cursor >= 0 then
    set_callback(function(ctx)
        local mx, my = mouse_position()

        if mx ~= last_mx or my ~= last_my then last_ms = get_ms() end
        -- draw on top of UI windows
        ctx:draw_foreground(true)

        -- check for recent movement
        if get_ms() - last_ms < 2000 then
            -- draw the cursor
            ctx:draw_image(cursor, mx, my, mx + 0.05, my - 0.05 / 9 * 16, 0, 0, 1, 1, 0xffffffff)
        end

        -- hide normal cursor if inside the game window
        if iio.showcursor and mx > -1 and mx < 1 and my > -1 and my < 1 then
            iio.showcursor = false
        end

        last_mx, last_my = mx, my
    end, ON.GUIFRAME)
end
