meta.name = "Liquids by grid"
meta.version = "1.0"
meta.description = "Shows the amounts of liquids by grid, using the `get_liquids_at` function"

register_option_bool("only_draw_at_mouse", "Testing: Draw only liquid num at mouse position", "", false)
register_option_bool("draw_grid_lines", "Draw grid lines", "", true)
register_option_bool("draw_text_shadow", "Draw text shadow", "", true)

local function get_camera_bounds_grid()
	local left, top = game_position(-1, 1)
	local right, bottom = game_position(1, -1)
	left, top, right, bottom = math.floor(left - 0.5), math.ceil(top + 0.5), math.ceil(right + 0.5), math.floor(bottom - 0.5)
	return left, top, right, bottom
end

local water_color = Color:new(0, 0.1, 1.0, 1.0)
local lava_color = Color:new(1.0, 0.1, 0.0, 1.0)
local black = Color:black()

---@type table<integer, TextRenderingInfo>
local num_text_renders = {}
local last_zoom = get_zoom_level()

-- Generate a text for every possible number, to optimize when there's many numbers to render
local function reload_num_text_renders(zoom)
	for i = 0, 12 do
		num_text_renders[i] = TextRenderingInfo:new(tostring(i), 0.0081 / zoom, 0.0081 / zoom, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
	end
end
reload_num_text_renders(last_zoom)

---@param ctx VanillaRenderContext
---@param text_render TextRenderingInfo
local function draw_text_shadow(ctx, text_render)
	local offset <const> = 0.027 / last_zoom
	local saved_x, saved_y = text_render.x, text_render.y
	text_render.x, text_render.y = text_render.x + offset, text_render.y - offset
	ctx:draw_text(text_render, black)
	text_render.x, text_render.y = saved_x, saved_y
end

---@param ctx VanillaRenderContext
---@param x number
---@param y number
local function draw_liquids_at(ctx, x, y)
	local water, lava = get_liquids_at(x, y, LAYER.PLAYER)
	if water == 0 and lava == 0 then return end

	local spacing <const> = 0.0135 / last_zoom
	local text_render = num_text_renders[water]
	text_render.x, text_render.y = screen_position(x, y)
	text_render.x = text_render.x - (text_render.width / 2.) - spacing
	if options.draw_text_shadow then
		draw_text_shadow(ctx, text_render)
	end
	ctx:draw_text(text_render, water_color)

	text_render = num_text_renders[lava]
	text_render.x, text_render.y = screen_position(x, y)
	text_render.x = text_render.x + (text_render.width / 2.) + spacing
	if options.draw_text_shadow then
		draw_text_shadow(ctx, text_render)
	end
	ctx:draw_text(text_render, lava_color)
end

---@param ctx VanillaRenderContext
---@param x number
---@param y number
local function draw_grid_liquids(ctx, x, y)
	local off <const> = 1./3.
	for iy = -1, 1 do
		for ix = -1, 1 do
			local cx, cy = x + (ix * off), y + (iy * off)
			draw_liquids_at(ctx, cx, cy)
		end
	end
end

local trans_white = Color:white()
trans_white.a = 0.25

---@param ctx VanillaRenderContext
set_callback(function(ctx)
	local left, top, right, bottom = get_camera_bounds_grid()
	local zoom = get_zoom_level()
	if last_zoom ~= zoom then
		reload_num_text_renders(zoom)
		last_zoom = zoom
	end

	if not options.only_draw_at_mouse then
		for y = bottom, top do
			for x = left, right do
				draw_grid_liquids(ctx, x, y)
			end
		end
	else
		local spacing <const> = 0.0135 / last_zoom
		local x, y = game_position(mouse_position())
		x = x + (1/6)
		y = y + (1/6)
		x = x - (x % (1/3))
		y = y - (y % (1/3))
		draw_liquids_at(ctx, x, y)
	end

	if not options.draw_grid_lines then return end
	local off <const> = 1./3.
	local line_width <const> = 3.5
	for y = bottom, top do
		for i = 0, 2 do
			local line_y = y + (off * i) + 0.5
			ctx:draw_screen_line(Vec2:new(screen_position(left, line_y)), Vec2:new(screen_position(right, line_y)), line_width, trans_white)
		end
	end
	for x = left, right do
		for i = 0, 2 do
			local line_x = x + (off * i) + 0.5
			ctx:draw_screen_line(Vec2:new(screen_position(line_x, top)), Vec2:new(screen_position(line_x, bottom)), line_width, trans_white)
		end
	end
end, ON.RENDER_PRE_HUD)
