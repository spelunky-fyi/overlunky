meta.name = "Position Type Test"
meta.author = "Dregu"
meta.version = "WIP"
meta.description = "Test all position_is_valid flags and draw some circles."

for k,v in pairs(POS_TYPE) do
    register_option_bool(k, k, false)
end

set_callback(function(ctx)
    flags = 0
    for k,v in pairs(POS_TYPE) do
        if options[k] then
            flags = flags | v
        end
    end
    local ax, ay, bx, by = get_bounds()
    for y = ay-0.5, by+0.5, -1 do
        for x = ax+0.5, bx-0.5 do
            if position_is_valid(x, y, state.camera_layer, flags) then
                local sx, sy = screen_position(x, y)
                local d = screen_distance(0.3)
                ctx:draw_circle(sx, sy, d, 2, 0x9900ff00)
            end
        end
    end
end, ON.GUIFRAME)
