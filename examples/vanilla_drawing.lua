meta.name = "Vanilla Drawing"
meta.version = "WIP"
meta.description = "Demonstrates drawing text, shapes and lines with the game engine"
meta.author = "Mr Auto"

purple = Color:purple()
yellow = Color:yellow()
white = Color:white()
red = Color:red()
blue = Color:blue()

ratio = 16.0/9.0
n_ratio = 9.0/16.0

text_scale = 0.001

--[[ Blinking text: ]]

BuyNow = TextRenderingInfo:new("FOR BEST OFFERS, CLICK HERE", -0.6, 0.4, text_scale, text_scale, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
BuyNowTimer = 0
interval = 10

set_callback(function(render_ctx)
    if state.time_level - BuyNowTimer > interval then
        render_ctx:draw_text(BuyNow, purple)
        if state.time_level - BuyNowTimer > interval*2 then
            BuyNowTimer = state.time_level
        end
    else
        render_ctx:draw_text(BuyNow, yellow)
    end
    -- you can also just draw the same TextRendering multiple times, with different color 
    -- and even different position, just edit x/y position after each draw_text call
end, ON.RENDER_POST_HUD)

--[[ Draw Circle (not really) ]]
-- draws polyline around the center point, points that make up the polyline are drawn in the same distance to each other
-- clicking the text from the example above adds more points (increases precision), at some point blending into circle like object

-- get_circle_points(Vec2 center, float radius, optional: bool screen, optional: int precision)
function get_circle_points(center, radius, screen, precision)
    if precision == nil then
        precision = 50
    end
    angle_step = math.pi * 2 / precision
    circle_points = {}
    for idx = 1, precision do
        a = radius * math.cos(angle_step * idx) 
        b = radius * math.sin(angle_step * idx)
        if screen then
            a = a * n_ratio -- need to correct for the screen ratio
        end
        -- for drawing ellipse: you can just multiply a and b values by some number, similar as we fix the screen ratio
        table.insert(circle_points, Vec2:new(a + center.x, b + center.y))
    end
    return circle_points
end

nof_points = 4 -- just the initial
circle_radius = 0.56
circle_thickness = 30.0

-- we borrow some stuff from example above
p_center = Vec2:new(BuyNow.x, BuyNow.y)
click_area_w = BuyNow.width / 2
click_area_h = BuyNow.height / 2

set_callback(function(render_ctx)
    points_to_draw = get_circle_points(p_center, circle_radius, true, nof_points)
    -- if you need static circle, consider grabbing the points_to_draw once, instead of generating them every refresh rate in this callback
    render_ctx:draw_screen_poly(points_to_draw, circle_thickness, yellow, true)
    -- debug text just to see the number of points/precision
    render_ctx:draw_text(F"precision: {nof_points}", p_center.x, p_center.y - 0.3, 0.0005, 0.0005, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
end, ON.RENDER_POST_HUD)

-- need to use imgui for caching the mouse inputs (GUIFRAME)
iio = get_io()
click_area = AABB:new(p_center.x + click_area_w, p_center.y + click_area_h, p_center.x - click_area_w, p_center.y - click_area_h)

set_callback(function(render_ctx)
    if iio.mouseclicked[1] then
        if iio.wantmouse then
            mx, my = mouse_position()
            if click_area:is_point_inside(mx, my) then
                nof_points = nof_points + 1
            end
        end
    end
end, ON.GUIFRAME)

--[[ Two Color Text (top/down) ]]

HalfTextTop = TextRenderingInfo:new("Poland", 0.9, 0.4, text_scale, text_scale, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.NORMAL)
HalfTextBottom = TextRenderingInfo:new("Poland", 0.9, 0.4, text_scale, text_scale, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.NORMAL)

bottom_letters_src = HalfTextBottom:get_source()
bottom_letters_dest = HalfTextBottom:get_dest()

function cut_letter_in_half(letter, bottom, y)
    quad = letter:get_quad()
    if bottom then
        quad.bottom_left_y = y
        quad.bottom_right_y = y
    else
        quad.top_left_y = y
        quad.top_right_y = y
    end
    letter:set_quad(quad)
end

-- we only need one loop as they have the same number of letters
for idx, _ in pairs(bottom_letters_src) do
    center_src = bottom_letters_src[idx]:center()
    center_dest = bottom_letters_dest[idx]:center()
    
    -- we only need to modify the bottom as we can just drawn it on top  
    cut_letter_in_half(bottom_letters_src[idx], false, center_src.y) -- modify the source, to only get bottom half of the letters from the texture
    cut_letter_in_half(bottom_letters_dest[idx], false, center_dest.y) -- modify the destination, so the bottom half of the letters are not drawn in the whole height of the text
end

set_callback(function(render_ctx)
    -- notice that this is the only thing in the callback, the rest was done just once, on the script load
    render_ctx:draw_text(HalfTextTop, white)
    render_ctx:draw_text(HalfTextBottom, red)
end, ON.RENDER_POST_HUD)

--[[ Two Color Text (stripes) with Underline ]]

example_text = "Some sample text with underline"
TwoColorText = TextRenderingInfo:new(example_text, 0.9, 0.3, text_scale, text_scale, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.NORMAL)
TwoColorText2 = TextRenderingInfo:new(example_text, 0.9, 0.3, text_scale, text_scale, VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.NORMAL)

underline_offset = 0.036
line_A = Vec2:new(TwoColorText.x - TwoColorText.width, TwoColorText.y + TwoColorText.height + underline_offset)
line_B = Vec2:new(TwoColorText.x, TwoColorText.y + TwoColorText.height + underline_offset)

do -- just to keep the variables local to this part of the code
    local every_other = false
    local zero_vector = Vec2:new()
    for _, letter in pairs(TwoColorText2:get_dest()) do
        if every_other then
            letter.bottom.A = zero_vector
            letter.bottom.B = zero_vector
            letter.bottom.C = zero_vector
            every_other = false
        else
            letter.top.A = zero_vector
            letter.top.B = zero_vector
            letter.top.C = zero_vector
            every_other = true
        end
    end
end

set_callback(function(render_ctx)
    render_ctx:draw_text(TwoColorText, white)
    render_ctx:draw_text(TwoColorText2, purple)
    render_ctx:draw_screen_line(line_A, line_B, 10, white)
end, ON.RENDER_POST_HUD)

--[[ Rotated Text ]]

-- no position will create it at 0,0
UpsideDownText = TextRenderingInfo:new("Upside down text", text_scale, text_scale, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
UpsideDownText:rotate(3.14) -- good enogh ;)
-- notice: the `:rotate` function, rotates the text by the given angle, not set the given angle

set_callback(function(render_ctx)
    render_ctx:draw_text(UpsideDownText, blue)
    
    --UpsideDownText:rotate(0.01) -- uncomment to make it spin
end, ON.RENDER_POST_HUD)

--[[ Text Drawn in the World Coordinates (at specific draw depth) ]]

set_callback(function(render_ctx, draw_depth)
    
    if #players > 0 and players[1].draw_depth == draw_depth then
    
        x, y = get_render_position(players[1].uid)
        sx, sy = screen_position(x, y + 0.6)
        -- fix scale for the zoom
        scale = text_scale  / (get_zoom_level() * 0.0737)
        render_ctx:draw_text("Look At Me", sx, sy, scale, scale, yellow, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.BOLD)
    end
end, ON.RENDER_POST_DRAW_DEPTH)

--[[ Draw Rotating Triangle with Marked Center]]

rotating_triangle = Triangle:new(Vec2:new(-0.2,0.5), Vec2:new(0.0, 0.6), Vec2:new(-0.1, 0.1))
rt_center = rotating_triangle:center()
dot_size = 0.01 -- it's actually half the size
-- to draw square we need to account for screen ratio
dot_AABB = AABB:new(rt_center.x - dot_size * n_ratio, rt_center.y - dot_size, rt_center.x + dot_size * n_ratio, rt_center.y + dot_size)

set_callback(function(render_ctx)

    render_ctx:draw_screen_rect_filled(dot_AABB, Color:red())
    -- convert from 16/9 ratio to 1/1 to properly rotate
    rotating_triangle.A.x = rotating_triangle.A.x * ratio
    rotating_triangle.B.x = rotating_triangle.B.x * ratio
    rotating_triangle.C.x = rotating_triangle.C.x * ratio
    -- get center in 1/1 ratio
    ctr = rotating_triangle:center()
    --rotate around the center
    rotating_triangle:rotate(0.01, ctr.x, ctr.y)
    -- convert back to 16/9
    rotating_triangle.A.x = rotating_triangle.A.x * n_ratio
    rotating_triangle.B.x = rotating_triangle.B.x * n_ratio
    rotating_triangle.C.x = rotating_triangle.C.x * n_ratio
    render_ctx:draw_screen_triangle(rotating_triangle, 20, Color:white())

end, ON.RENDER_POST_HUD)

--[[ Examples of Corner finish ]]

set_callback(function(render_ctx)
    render_ctx:set_corner_finish(CORNER_FINISH.NONE)
    render_ctx:draw_screen_poly({Vec2:new(-0.8, -0.7), Vec2:new(-0.9, -0.7), Vec2:new(-0.8, -0.63)}, 40, Color:new(1.0, 1.0, 1.0, 0.5), false)
    render_ctx:draw_text("CORNER_FINISH.NONE", -0.84, -0.57, 0.0005, 0.0005, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
    render_ctx:set_corner_finish(CORNER_FINISH.REAL)
    render_ctx:draw_screen_poly({Vec2:new(-0.55, -0.7), Vec2:new(-0.65, -0.7), Vec2:new(-0.55, -0.63)}, 40, Color:new(1.0, 1.0, 1.0, 0.5), false)
    render_ctx:draw_text("CORNER_FINISH.REAL", -0.6, -0.57, 0.0005, 0.0005, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
    render_ctx:set_corner_finish(CORNER_FINISH.CUT)
    render_ctx:draw_screen_poly({Vec2:new(-0.3, -0.7), Vec2:new(-0.4, -0.7), Vec2:new(-0.3, -0.63)}, 40, Color:new(1.0, 1.0, 1.0, 0.5), false)
    render_ctx:draw_text("CORNER_FINISH.CUT", -0.37, -0.57, 0.0005, 0.0005, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
    render_ctx:set_corner_finish(CORNER_FINISH.ADAPTIVE)
    render_ctx:draw_screen_poly({Vec2:new(-0.1, -0.7), Vec2:new(-0.2, -0.7), Vec2:new(-0.1, -0.63)}, 40, Color:new(1.0, 1.0, 1.0, 0.5), false)
    render_ctx:draw_text("CORNER_FINISH.ADAPTIVE", -0.13, -0.57, 0.0005, 0.0005, white, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
end, ON.RENDER_POST_HUD)


--[[ Text Floating on Screen (bouncing of the screen edges) ]]

DVDLogo = TextRenderingInfo:new("DVD", 0.0055, 0.0018, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.ITALIC)
DVD_direction_x = 1.0
DVD_direction_y = -1.0
DVD_speed = 0.003
DVD_Colors = {
    Color:purple(), Color:yellow(), Color:white(), Color:silver(),
    Color:gray(), Color:red(), Color:maroon(), Color:olive(),
    Color:lime(), Color:green(), Color:aqua(), Color:teal(),
    Color:blue(), Color:navy(), Color:fuchsia()
}
DVD_current_color = Color:white()

function DVD_change_color()
    chosen_index = 0
    repeat
        chosen_index = math.random(#DVD_Colors)
    until( not (DVD_current_color == DVD_Colors[chosen_index]))
    DVD_current_color = DVD_Colors[chosen_index]
end

set_callback(function(render_ctx, draw_depth)
    
    DVDLogo.x = DVDLogo.x + DVD_speed * DVD_direction_x
    DVDLogo.y = DVDLogo.y + DVD_speed * DVD_direction_y
    if DVDLogo.x + DVDLogo.width / 2 > 1.0 then
        DVDLogo.x = DVDLogo.x - DVD_speed
        DVD_direction_x = -1.0
        DVD_change_color()
    elseif DVDLogo.x - DVDLogo.width / 2 < -1.0 then
        DVDLogo.x = DVDLogo.x + DVD_speed
        DVD_direction_x = 1.0
        DVD_change_color()
    end
    
    if DVDLogo.y - DVDLogo.height / 2 > 1.0 then
        DVDLogo.y = DVDLogo.y - DVD_speed
        DVD_direction_y = -1.0
        DVD_change_color()
    elseif DVDLogo.y + DVDLogo.height / 2 < -1.0 then
        DVDLogo.y = DVDLogo.y + DVD_speed
        DVD_direction_y = 1.0
        DVD_change_color()
    end
    
    render_ctx:draw_text(DVDLogo, DVD_current_color)
end, ON.RENDER_POST_HUD)
