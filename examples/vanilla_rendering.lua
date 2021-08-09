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

font_scale = 0.0006

set_callback(function()
    local y = 0.9
    local scale = font_scale 
    for i = 1, 7 do
        vanilla_draw_text("Text scale "..tostring(scale), 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
        y = y - 0.1
        scale = scale + 0.0001
    end
    y = y - 0.115
    
    local x = -0.12
    for i = 1, 7 do
        vanilla_draw_texture(TEXTURE.DATA_TEXTURES_HUD_0, 4, i-1, x, y, 0.08, 0.08, white)
        x = x + 0.04
    end
    y = y - 0.035

    vanilla_draw_text("Left aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.LEFT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    vanilla_draw_text("Center aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    vanilla_draw_text("Right aligned", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11

    vanilla_draw_text("Red", 0.0, y, scale, scale, red, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    vanilla_draw_text("Green", 0.0, y, scale, scale, green, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.11
    vanilla_draw_text("Blue", 0.0, y, scale, scale, blue, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
    y = y - 0.14

    vanilla_draw_text("Bold text", 0.0, y, scale, scale, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
end, ON.VANILLA_RENDER)
