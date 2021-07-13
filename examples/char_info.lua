meta.name = "Character Info"
meta.version = "WIP"
meta.description = "Draws character names and royal title in their respective color"
meta.author = "Malacath"

set_callback(function(draw_ctx)
    local chars = get_entities_by_mask(MASK.PLAYER)
    for _, char_uid in pairs(chars) do
        local char = get_entity(char_uid):as_player()
        local royal_title = nil
        if char:is_female() then
            royal_title = 'Queen'
        else
            royal_title = 'King'
        end
        local name = F'{char:get_name()} aka {royal_title} {char:get_short_name()}'
        local color = char:get_heart_color()
        local u_color = rgba(
            math.floor(color.r * 255),
            math.floor(color.g * 255),
            math.floor(color.b * 255),
            math.floor(color.a * 255)
        )

        local x, y, l = get_render_position(char_uid)
        local sx, sy = screen_position(x, y + char.hitboxy + char.offsety)
        local tx, ty = draw_text_size(35, name)
        draw_ctx:draw_text(sx - tx / 2, sy - ty * 2, 35, name, u_color)
    end
end, ON.GUIFRAME)
