meta.name = "Vanilla rendering"
meta.version = "WIP"
meta.description = "Demonstrates drawing of in-game text"
meta.author = "Zappatic"

white = Color:new()
white.r  = 1.0
white.g  = 1.0
white.b  = 1.0
white.a  = 1.0

red = Color:new()
red.r  = 1.0
red.g  = 0.0
red.b  = 0.0
red.a  = 1.0

green = Color:new()
green.r  = 0.0
green.g  = 1.0
green.b  = 0.0
green.a  = 1.0

blue = Color:new()
blue.r  = 0.0
blue.g  = 0.0
blue.b  = 1.0
blue.a  = 1.0

black = Color:new()
black.r  = 0.0
black.g  = 0.0
black.b  = 0.0
black.a  = 1.0

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
    y = y - 0.115
    
    local x = -0.12
    for i = 1, 7 do
        render_ctx:draw_texture(TEXTURE.DATA_TEXTURES_HUD_0, 4, i-1, x, y, 0.08, 0.08, white)
        x = x + 0.04
    end
    y = y - 0.035

    render_ctx:draw_text("Left aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    render_ctx:draw_text("Center aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    render_ctx:draw_text("Right aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11

    render_ctx:draw_text("Red", 0.0, y, scale, scale, red, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    render_ctx:draw_text("Green", 0.0, y, scale, scale, green, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    render_ctx:draw_text("Blue", 0.0, y, scale, scale, blue, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11

    render_ctx:draw_text("Bold text", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
    y = y - 0.13

    local text = "Bordered, shadowed text"
    scale = scale * 0.75
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
