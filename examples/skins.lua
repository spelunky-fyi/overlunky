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
    -- vanilla or enabled mods' skins
    for i = ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_CLASSIC_GUY do
        local tex = get_type(i).texture
        local def = get_texture_definition(tex)
        if not exists(def.texture_path) then
            images[#images+1] = { path=def.texture_path, name=get_character_name(i), texture=tex }
        end
    end

    -- all installed char mods
    for i,path in pairs(list_char_mods() or {}) do
        if not exists(path) then
            local w, h = get_image_size(path)
            create_skin(path, w, h)
        end
    end

    -- png files from skins subfolder
    for i,path in pairs(list_dir(DIR) or {}) do
        if string.match(path, ".png") and not exists(path) then
            local w, h = get_image_size(path)
            create_skin(path, w, h)
        end
    end
end

get_skins()

function hook_skin(ent)
    if not ent then return end
    ent:set_post_update_state_machine(function(ent)
        if not ent.inventory then return end
        if skins and skins[ent.inventory.player_slot] then
            ent:set_texture(skins[ent.inventory.player_slot])
        elseif ent.inventory.player_slot > 0 then
            ent:set_texture(state.items.player_select[ent.inventory.player_slot].texture)
        end
    end)
end

-- hook the current and future players
set_post_entity_spawn(function(ent) hook_skin(ent) end, SPAWN_TYPE.ANY, MASK.PLAYER)
set_post_entity_spawn(function(ent) hook_skin(ent) end, SPAWN_TYPE.ANY, MASK.ITEM, ENT_TYPE.ITEM_PLAYERGHOST)
hook_skin(get_player(1, true))

-- apparently playerbags and ghosts with custom texture crash the game, maybe this will fix it
set_pre_entity_spawn(function(type, x, y, l)
    for i,p in pairs(players) do
        p:set_texture(p.type.texture)
    end
    local uid = spawn_critical(type, x, y, l, 0, 0)
    if type == ENT_TYPE.ITEM_PICKUP_PLAYERBAG then
        get_entity(uid):set_post_update_state_machine(function(ent)
            if skins[1] then
                ent:set_texture(skins[1])
            end
            clear_callback()
        end)
    end
    return uid
end, SPAWN_TYPE.SYSTEMIC | SPAWN_TYPE.LEVEL_GEN, MASK.ITEM, {ENT_TYPE.ITEM_PICKUP_PLAYERBAG, ENT_TYPE.ITEM_PLAYERGHOST})

-- vanilla ui stuff
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

-- show custom message on bottom right
change_string(hash_to_stringid(0xcd07f25b), CUSTOM_SKIN_TEXT)
set_pre_render_screen(SCREEN.CHARACTER_SELECT, function(self, ctx)
    if state.items.player_select[1].activated then
        state.screen_character_select.right_button_text_id = hash_to_stringid(0xcd07f25b)
        if skins[1] and state.screen_character_select.start_pressed then
            state.screen_character_select.player_y[1] = -2 --hide default char, TODO: something else
        end
    end
    if state.screen_character_select.player_quickselect_shown[1] then
        skins[1] = nil
    end
end)

N = 8
F = 1.96
AX = -0.7
AY = 0.675
ANG = 0

repeat
    F = F - 0.005
    W = -AX/(N/F)
    H = W/9*16
    XP = W / 20
    YP = H / 20
    N = math.floor(-2*AX / W)
until math.ceil(#images/N)*H < 2*AY

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

function draw_name(ctx)
    if draw_select then
        local color = Color:new(0.25, 0.2, 0.2, 1)
        ctx:draw_text("\u{83} "..images[selected_skin + 1].name.."   \u{86} Random   \u{85} Reset   \u{88} Refresh", 0.68, -0.68, 0.001, 0.001, color,
            VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.NORMAL)
    elseif game_manager.pause_ui.visibility > PAUSEUI_VISIBILITY.INVISIBLE then
        local color = Color:white()
        color.a = game_manager.pause_ui.menu_slidein_progress
        ctx:draw_text(CUSTOM_SKIN_TEXT, 0, -0.8, 0.001, 0.001, color, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
    end
end

function draw_sticker(ctx)
    if state.screen ~= SCREEN.CHARACTER_SELECT then return end
    local color = Color:white()
    color.r = 1 - state.screen_character_select.opacity
    color.g = 1 - state.screen_character_select.opacity
    color.b = 1 - state.screen_character_select.opacity
    local src = Quad:new(AABB:new(0.13, 0.03125, 0.2183, 0.2168))
    local x = -0.49
    local y = 0.27
    local w = 0.16
    local h = w/9*16

    local bg_aabb = AABB:new(x - 0.0225, y + 0.06, x + w + 0.0225, y - h - 0.02)
    local bg_dest = Quad:new(bg_aabb)
    local bg_x, bg_y = bg_dest:get_AABB():center()
    bg_dest:rotate(ANG, bg_x, bg_y)

    local dest_aabb = AABB:new(x, y, x + w, y - h)
    --local dest = Quad:new(dest_aabb)
    --local d_x, d_y = dest:get_AABB():center()
    --dest:rotate(ANG, d_x, d_y)
    ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_TOP_PROFILE_0, src, bg_dest, color)
    ctx:draw_screen_texture(skins[1], 0, 0, dest_aabb, color, ANG, 0, 0)
end

function draw_selector(ctx)
    if draw_select then
        if state.pause == PAUSE.MENU then
            draw_select = false
            return
        end
        local src = Quad:new(AABB:new(0.525, 0.21, 0.86, 0.93))
        local dest = Quad:new(AABB:new(-1, 0.84, 1, -0.84))
        dest:rotate(math.pi/2, 0, 0)
        ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_TOP_MAIN_0, src, dest, Color:white())
        draw_skins(ctx)
    elseif state.items.player_select[1].activated and skins[1] and state.screen == SCREEN.CHARACTER_SELECT then
        draw_sticker(ctx)
    end
end

-- draw huge quick select and skins
set_post_render_screen(SCREEN.CHARACTER_SELECT, function(self, ctx)
    draw_selector(ctx)
    draw_name(ctx)
end)

set_callback(function(ctx)
    draw_selector(ctx)
    draw_name(ctx)
end, ON.RENDER_POST_PAUSE_MENU)

-- clear custom skin selection
set_callback(function()
    if state.screen_next == SCREEN.CHARACTER_SELECT then
        skins = {}
    end
end, ON.PRE_LOAD_SCREEN)

-- disable inputs to underlaying char select screen when skin select is open
set_callback(function()
    if state.screen ~= SCREEN.CHARACTER_SELECT and #players == 0 then return end
    local ret = false
    if state.items.player_select[1].activated and
        (state.screen == SCREEN.CHARACTER_SELECT or game_manager.pause_ui.visibility > PAUSEUI_VISIBILITY.INVISIBLE or draw_select) and pressed(0x4) then
        game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.INVISIBLE
        draw_select = not draw_select
        if #players > 0 and not draw_select then
            game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.VISIBLE
            ret = true
        end
    end
    if draw_select then
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
            draw_select = false
            ANG = (prng:random() - 0.5) / 20 * math.pi
            if #players > 0 then
                game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
                ret = true
            end
        elseif pressed(0x2) then --cancel
            skins[1] = nil
            draw_select = false
            if #players > 0 then
                game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
            end
            ret = true
        elseif released(0x8) then --random
            selected_skin = prng:random_index(#images, 0) - 1
            skins[1] = images[selected_skin + 1].texture
            draw_select = false
            ANG = (prng:random() - 0.5) / 20 * math.pi
            if #players > 0 then
                game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
            end
            ret = true
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
