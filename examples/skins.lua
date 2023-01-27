meta = {
	name = "Too Many Skins",
	version = "WIP",
	description = "Load extra character mods from all available packs or the skins subfolder.",
	author = "Dregu"
}

DIR = "skins/"
CUSTOM_SKIN_TEXT = "\u{84} Too Many Skins"

texture_def = get_texture_definition(TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0)
skins = {}
images = {}
draw_select = false
selected_skin = 0

-- get vanilla or enabled mods' skins
for i = ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_CLASSIC_GUY do
    local tex = get_type(i).texture
    local def = get_texture_definition(tex)
    images[#images+1] = { path=def.texture_path, name=get_character_name(i), texture=tex }
end

-- I guess you can test these too if you want to
images[#images+1] = { path="/Data/Textures/char_hired.DDS", name="Hired Hand", texture=TEXTURE.DATA_TEXTURES_CHAR_HIRED_0 }
images[#images+1] = { path="/Data/Textures/char_eggchild.DDS", name="Eggplant Child", texture=TEXTURE.DATA_TEXTURES_CHAR_EGGCHILD_0 }

function exists(path)
    for i,img in pairs(images) do
        if img.path == path then return true end
    end
    return false
end

function clean_name(name)
    name = name:gsub("/Mods/Packs/", "")
    name = name:gsub("Data/Textures/", "")
    name = name:gsub("Entities/", "")
    name = name:gsub("fyi.", "")
    name = name:gsub("skins/", "")
    name = name:gsub("_full.png", "")
    name = name:gsub("char_", "")
    name = name:gsub(".png", "")
    return name
end

-- create a custom skin texture that supports _full sheets too
function create_skin(path, w, h)
    texture_def.texture_path = path
    texture_def.width = w
    texture_def.height = h
    texture_def.sub_image_width = w
    texture_def.sub_image_height = w
    texture_def.tile_width = math.floor(w/16)
    texture_def.tile_height = math.floor(w/16)
    local skin_id = define_texture(texture_def)
    images[#images+1] = {path=path, name=clean_name(path), texture=skin_id, w=w, h=h}
    return skin_id
end

-- get all installed char mods or png files in the skins folder
function get_skins()
    local added = {}

    -- all installed char mods
    for i,path in pairs(list_char_mods() or {}) do
        if not exists(path) then
            local w, h = get_image_size(path)
            create_skin(path, w, h)
        end
        added[path] = true
    end

    -- png files from skins subfolder
    for i,path in pairs(list_dir(DIR) or {}) do
        if string.match(path, ".png") and not exists(path) then
            local w, h = get_image_size(path)
            create_skin(path, w, h)
        end
        added[path] = true
    end

    -- remove skins that were deleted since they were loaded
    for i=#images,1,-1 do
        if not added[images[i].path] and images[i].texture > TEXTURE.DATA_TEXTURES_CHAR_EGGCHILD_0 then
            table.remove(images, i)
        end
    end

    -- calculate icon sizes that hopefully fit the dialog with any number of skins
    N = 7
    F = 1.98
    AX = -0.7
    AY = 0.675
    repeat
        F = F - 0.002
        W = -AX/(N/F)
        H = W/9*16
        XP = W / 20
        YP = H / 20
        N = math.floor(-2*AX / W)
    until math.ceil(#images/N)*H < 2*AY
end

-- load all skins on load
get_skins()

-- check for global inputs
buttons_prev = 0
function pressed(key)
    if test_mask(game_manager.game_props.buttons, key) and not test_mask(buttons_prev, key) then
        return true
    end
    return false
end
function released(key)
    if not test_mask(game_manager.game_props.buttons, key) and test_mask(buttons_prev, key) then
        return true
    end
    return false
end

-- refresh textures of existing entities
function set_textures()
    for i=1,4 do
        local ent = get_player(i)
        if ent then ent:set_texture(ent:get_texture()) end
        local ghost = get_playerghost(i)
        if ghost then ghost:set_texture(ghost:get_texture()) end
    end
    for i,v in pairs(get_entities_by({ENT_TYPE.ITEM_PICKUP_PLAYERBAG, ENT_TYPE.ITEM_CLIMBABLE_ROPE, ENT_TYPE.ITEM_UNROLLED_ROPE, ENT_TYPE.BG_SHOPWANTEDPORTRAIT, ENT_TYPE.ITEM_WHIP}, MASK.ANY, LAYER.BOTH)) do
        local ent = get_entity(v)
        ent:set_texture(ent:get_texture())
    end
end

-- replace or reset character textures
function replace_skin()
    local replaced = {}
    for i=1,4 do
        if skins[i] then
            replace_texture(state.items.player_select[i].texture, skins[i])
            replaced[state.items.player_select[i].texture] = true
        elseif state.items.player_select[i].activated and not replaced[state.items.player_select[i].texture] then
            reset_texture(state.items.player_select[i].texture)
        end
    end
    set_textures()
end

-- show custom message on bottom right of character select wood panel
change_string(hash_to_stringid(0xcd07f25b), CUSTOM_SKIN_TEXT)
set_pre_render_screen(SCREEN.CHARACTER_SELECT, function(self, ctx)
    if state.items.player_select[1].activated then
        if state.screen_character_select.player_y[1] == 0 then
            state.screen_character_select.right_button_text_id = hash_to_stringid(0xcd07f25b)
        end
    end
    if state.screen_character_select.player_quickselect_shown[1] then
        if skins[1] then
            skins[1] = nil
            replace_skin()
        end
    end
end)

-- draw the skin previews and cursor
function draw_skins(ctx)
    local sx = selected_skin % N
    local sy = math.floor(selected_skin / N)
    local sdest = AABB:new(
        AX + sx * W + XP,
        AY - sy * H - YP,
        AX + sx * W + W - XP,
        AY - sy * H - H + YP
    )
    ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_STICKERS_0, 2, 7, sdest, Color:white())
    local x = AX
    local y = AY
    for i, img in pairs(images) do
        local dest = AABB:new()
        local af = 0
        if i == selected_skin + 1 then
            af = math.floor (get_frame() % 24 / 3) + 1
        end
        ctx:draw_screen_texture(img.texture, 0, af, x + XP, y - YP, x + W - XP, y - H + YP, Color:white())
        x = x + W
        if x > -AX-W then
            x = AX
            y = y - H
        end
    end
end

-- draw the TooManySkins prompt or skin name and buttons inside the selector
function draw_name(ctx)
    if draw_select then
        local color = Color:new(0.25, 0.2, 0.2, 1)
        local reset = ""
        if images[selected_skin + 1].texture == state.items.player_select[1].texture and skins[1] then
            reset = "(Reset) "
        end
        ctx:draw_text("\u{83} "..reset..images[selected_skin + 1].name.."   \u{86} Random   \u{85} Reset   \u{88} Refresh", 0.68, -0.68, 0.001, 0.001, color,
            VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.NORMAL)
    elseif game_manager.pause_ui.visibility > PAUSEUI_VISIBILITY.INVISIBLE then
        local color = Color:white()
        color.a = game_manager.pause_ui.menu_slidein_progress
        ctx:draw_text(CUSTOM_SKIN_TEXT, 0, -0.8, 0.001, 0.001, color, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
    end
end

-- draw the selector background
function draw_selector(ctx)
    if selected_skin > #images-1 then
        selected_skin = #images-1
    end
    if draw_select then
        if state.pause == PAUSE.MENU or game_manager.pause_ui.visibility == PAUSEUI_VISIBILITY.VISIBLE then
            draw_select = false
            return
        end
        local src = Quad:new(AABB:new(0.525, 0.21, 0.86, 0.93))
        local dest = Quad:new(AABB:new(-1, 0.84, 1, -0.84))
        dest:rotate(math.pi/2, 0, 0)
        ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_TOP_MAIN_0, src, dest, Color:white())
        draw_skins(ctx)
    end
end

function draw_everything(ctx)
    draw_selector(ctx)
    draw_name(ctx)
end

-- draw on top of character select screen
set_post_render_screen(SCREEN.CHARACTER_SELECT, function(self, ctx)
    draw_everything(ctx)
end)

-- draw on top of the pause menu
set_callback(function(ctx)
    draw_everything(ctx)
end, ON.RENDER_POST_PAUSE_MENU)

-- disable inputs to underlaying char select screen when skin select is open and handle input
set_callback(function()
    if state.screen ~= SCREEN.CHARACTER_SELECT and #players == 0 then return end
    local ret = false
    if state.items.player_select[1].activated and
        ((state.screen == SCREEN.CHARACTER_SELECT and state.screen_character_select.player_y[1] == 0) or game_manager.pause_ui.visibility > PAUSEUI_VISIBILITY.INVISIBLE or draw_select) and pressed(0x4) then
        game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.INVISIBLE
        draw_select = not draw_select
        if #players > 0 and not draw_select then
            game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.VISIBLE
            ret = true
        end
    end
    local old_skin = skins[1]
    if draw_select then
        if state.screen == SCREEN.CHARACTER_SELECT then
            state.screen_character_select.player_quickselect_shown[1] = false
            state.screen_character_select.player_quickselect_fadein_timer[1] = 0
        end
        if pressed(0x10000) and selected_skin > 0 then --left
            selected_skin = selected_skin - 1
        elseif pressed(0x20000) and selected_skin < #images-1 then --right
            selected_skin = selected_skin + 1
        elseif pressed(0x40000) and selected_skin >= N then --up
            selected_skin = selected_skin - N
        elseif pressed(0x80000) and selected_skin + N < #images then --down
            selected_skin = selected_skin + N
        elseif pressed(0x1) then --select
            skins[1] = images[selected_skin + 1].texture
            replace_skin()
            draw_select = false
            if #players > 0 then
                game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
                ret = true
            end
        elseif released(0x2) then --cancel
            skins[1] = nil
            replace_skin()
            draw_select = false
            if #players > 0 then
                game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
            end
            ret = true
        elseif pressed(0x8) then --random
            selected_skin = prng:random_index(#images, 0) - 1
        elseif pressed(0x20) then --refresh
            get_skins()
        elseif pressed(0x800) and #players > 0 then --start
            draw_select = false
            game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.VISIBLE
            ret = true
        end
    end
    buttons_prev = game_manager.game_props.buttons
    return ret or draw_select
end, ON.PRE_UPDATE)
