meta.name = "Room visualizer"
meta.version = "WIP"
meta.description = "Shows shop zones, shop floors and other special room things."
meta.author = "Dregu"

set_callback(function(ctx)
    local l = state.camera_layer
    for rx=0,state.width-1 do
        for ry=0,state.height-1 do
            local rt = get_room_template(rx, ry, l)
            local rn = get_room_template_name(rt)
            if rt == 86 or rt == 87 or rt == 90 or rt == 45 or string.match(rn, "shop") or string.match(rn, "challenge_entrance") or string.match(rn, "vault") then
                local x = rx * 10 + 7.5
                local y = 122.5 - ry * 8 - 4
                local left, right = x, x
                local top, bottom = y+0.01, y-0.01
                while is_inside_shop_zone(left, y, l) do left = left - 0.1 end
                while is_inside_shop_zone(x, top, l) do top = top + 0.1 end
                while is_inside_shop_zone(right, y, l) do right = right + 0.1 end
                while is_inside_shop_zone(x, bottom, l) do bottom = bottom - 0.1 end
                local box = AABB:new(left, top, right, bottom)
                local sbox = screen_aabb(box)
                ctx:draw_rect_filled(sbox, 0, 0x7000ff00)

                for x=rx*10+3,rx*10+12,1 do
                    for y=122-ry*8,122-ry*8-7,-1 do
                        local color = 0x60000000
                        if is_inside_active_shop_room(x, y, l) then color = color | 0x00ff0000 end
                        local floor = get_grid_entity_at(x, y, l)
                        box = AABB:new(x-0.5, y+0.5, x+0.5, y-0.5)
                        sbox = screen_aabb(box)
                        if floor ~= -1 then
                            local ent = get_entity(floor)
                            if test_flag(ent.flags, ENT_FLAG.SHOP_FLOOR) then
                                color = color | 0x400000ff
                            end
                        end
                        ctx:draw_rect_filled(sbox, 0, color)
                    end
                end
            end
        end
    end
end, ON.GUIFRAME)
