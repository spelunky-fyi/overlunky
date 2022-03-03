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
      if rt == 86 or rt == 87 or string.match(rn, "shop") or string.match(rn, "challenge") or string.match(rn, "vault") then
        for x=rx*10+3.25,rx*10+12.25,0.5 do
          for y=122.25-ry*8,122.25-ry*8-7,-0.5 do
            local color = 0x000000ff
            if is_inside_shop_zone(x, y, l) then color = 0xaa00ff00 end
            local box = AABB:new(x-0.25, y+0.25, x+0.25, y-0.25)
            local sbox = screen_aabb(box)
            ctx:draw_rect_filled(sbox, 0, color)
          end
        end

        for x=rx*10+3,rx*10+12,1 do
          for y=122-ry*8,122-ry*8-7,-1 do
            local color = 0x60000000
            if is_inside_active_shop_room(x, y, l) then color = color | 0x00ff0000 end
            local floor = get_grid_entity_at(x, y, l)
            local box = AABB:new(x-0.5, y+0.5, x+0.5, y-0.5)
            local sbox = screen_aabb(box)
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
