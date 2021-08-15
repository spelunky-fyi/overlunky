meta.name = "Vanilla rendering"
meta.version = "WIP"
meta.description = "Demonstrates drawing of in-game text and textures"
meta.author = "Zappatic"

white = Color:white()
red = Color:red()
green = Color:green()
blue = Color:blue()
black = Color:black()

font_scale = 0.0006

background_rect_top_left_x = 0.0
background_rect_top_left_y = 0.0
background_rect_bottom_right_x = 0.0
background_rect_bottom_right_y = 0.0

set_callback(function(render_ctx)
    local y = 0.9
    local scale = font_scale 
    for i = 1, 7 do
        render_ctx:draw_text("Text scale "..tostring(scale), 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
        y = y - 0.1
        scale = scale + 0.0001
    end
    y = y + 0.05
    
    local x = -0.28
    for i = 1, 7 do
        render_ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_HUD_0, 4, i-1, x, y, x + 0.08, y - (0.08 * (16.0/9.0)), white)
        x = x + 0.08
    end
    y = y - 0.20

    scale = 0.0008
    render_ctx:draw_text("Left aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07
    render_ctx:draw_text("Center aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07
    render_ctx:draw_text("Right aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07

    red_width, _ = render_ctx:draw_text_size("Red ", scale, scale, VANILLA_FONT_STYLE.ITALIC)
    green_width, _ = render_ctx:draw_text_size("Green ", scale, scale, VANILLA_FONT_STYLE.ITALIC)
    blue_width, _ = render_ctx:draw_text_size("Blue ", scale, scale, VANILLA_FONT_STYLE.ITALIC)
    space_width = 0.01
    total_width = red_width + green_width + blue_width + (space_width * 2.0)

    render_ctx:draw_text("Red", (total_width / 2.0) * -1.0, y, scale, scale, red, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    render_ctx:draw_text("Green", ((total_width / 2.0) * -1.0) + red_width + space_width, y, scale, scale, green, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    render_ctx:draw_text("Blue", ((total_width / 2.0) * -1.0) + red_width + space_width + green_width + space_width, y, scale, scale, blue, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.07

    render_ctx:draw_text("Bold text", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
    y = y - 0.07

    local text = "Bordered, shadowed text"
    render_ctx:draw_text(text, 0.0025, y - 0.0025, scale, scale, black, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    render_ctx:draw_text(text, 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    width, height = render_ctx:draw_text_size(text, scale, scale, VANILLA_FONT_STYLE.ITALIC)

    -- because this text is rendered centered, we have to calc the top left relative to the centerpoint we provided to render_ctx:draw_text
    background_rect_top_left_x = 0.0 - (width / 2.0)
    background_rect_top_left_y = y - (height / 2.0)
    background_rect_bottom_right_x = background_rect_top_left_x + width
    background_rect_bottom_right_y = background_rect_top_left_y + height
    -- these background rect coords are then used below in the GUIFRAME callback to draw the border on the screen
    y = y - 0.12

    -- jump = \u{83}
    -- attack = \u{84}
    -- bomb = \u{85}
    -- rope = \u{86}
    -- pause = \u{89}
    -- journal = \u{8A}
    -- use door/buy = \u{88}
    -- walk/run = \u{87}
    -- left = \u{8B}
    -- right = \u{8C}
    -- up = \u{8D}
    -- down = \u{8E}
    -- enter/confirm = \u{8F}
    -- if you combine multiple keys in one string, it might mess up, e.g. "\u{83} and \u{89}"
    local special_characters = "Press \u{83} to jump"
    scale = 0.0013
    render_ctx:draw_text(special_characters, 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)

end, ON.RENDER_POST_HUD)

set_callback(function(render_ctx)
    render_ctx:draw_text("Rendered below the HUD", 0.95, 0.86,  0.0006, 0.0006, red, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.ITALIC)
end, ON.RENDER_PRE_HUD)

set_callback(function(draw_ctx)
    draw_ctx:draw_rect(background_rect_top_left_x, background_rect_top_left_y, background_rect_bottom_right_x, background_rect_bottom_right_y, 2, 2, rgba(255, 0, 255, 200))
end, ON.GUIFRAME)

-- draw a crown on the player's head
set_callback(function(render_ctx, draw_depth)
    if #players < 1 then return end
    -- the event is PRE draw depth, so if we want to draw it in front of the player, we have to
    -- draw it 'pre' the next draw_depth, which is one closer towards draw_depth 1 (so subtract 1 instead of add 1)
    if draw_depth == players[1].type.draw_depth - 1 then
        x, y, l = get_position(players[1].uid)
        -- reposition the crown on top of the head, starting from the center point of the player: bit to the left, bit upwards
        x = x - 0.5
        y = y + 1.15
        sx, sy = screen_position(x, y)
        sx2, sy2 = screen_position(x + 1.0, y - 1.0)
        render_ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_ITEMS_0, 1, 14, sx, sy, sx2, sy2, white)
    end
end, ON.RENDER_PRE_DRAW_DEPTH)
