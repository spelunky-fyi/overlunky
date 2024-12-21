meta = {
    name = "Too Many Skins",
    version = "1.0",
    description = "Load extra character mods from all available packs or the skins subfolder and change skins on the fly from the pause menu.",
    author = "Dregu",
    unsafe = true -- for the time being
}

DIR = "skins/"
CUSTOM_SKIN_TEXT = "\u{88} Too Many Skins"

texture_def = get_texture_definition(TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0)
skins = {}
images = {}
draw_select = false
selected_skin = { 0, 1, 2, 3 }
skin_def = {}
vanilla_strings = {}

stupid_story = {
    [TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0] = "She's come to the Moon to find her mom and dad, but was",
    [TEXTURE.DATA_TEXTURES_CHAR_MAGENTA_0] = "A fascination with the ocean led her to be",
    [TEXTURE.DATA_TEXTURES_CHAR_CYAN_0] = "An inquisitive artist and engineer who's been",
    [TEXTURE.DATA_TEXTURES_CHAR_BLACK_0] = "He had a lot of deep thoughts, but was"
}

-- get stringids of character names
function texture_to_stringid(tex)
    local i = tex - 285
    local full = hash_to_stringid(0x4cffb84e) + 2*i
    local short = hash_to_stringid(0x3ff7e9c4) + i
    return full, short
end

-- get vanilla or enabled mods' skins
for i = ENT_TYPE.CHAR_ANA_SPELUNKY, ENT_TYPE.CHAR_CLASSIC_GUY do
    local tex = get_type(i).texture
    local def = get_texture_definition(tex)
    skin_def[tex] = def
    images[#images+1] = { path=def.texture_path, name=get_character_name(i), texture=tex }
    local full_id, short_id = texture_to_stringid(tex)
    local full = get_string(full_id)
    local short = get_string(short_id)
    local bio = get_string(full_id+1)
    vanilla_strings[tex] = { full=full, short=short, bio=bio }
end

function exists(path)
    for i,img in pairs(images) do
        if img.path == path then return true end
    end
    return false
end

-- just to demo the results I want, playlunky could probably handle this with the linked files
-- this whole concept is kinda weird to add in overlunky
function get_char_json(path)
    path = path:gsub("/Data/Textures/Entities", "")
    path = path:gsub("/Data/Textures", "")
    path = path:gsub("_full.png$", "")
    path = path:gsub(".png$", "")
    path = path:gsub("^/", "")
    local data = {}
    local f = io.open(path..".json", "r")
    if f then
        data = json.decode(f:read("*all"))
        io.close(f)
    end
    if data.color then
        local color = Color:new()
        if type(data.color) == "string" then
            data.color = data.color:gsub("^#", "")
            data.color = data.color:gsub("^0x", "")
            data.color = data.color:sub(5,6)..data.color:sub(3,4)..data.color:sub(1,2)
            color:set_ucolor(tonumber("0x"..data.color))
        elseif type(data.color) == "table" then
            if data.color[1] then
                color.r, color.g, color.b = data.color[1], data.color[2], data.color[3]
            elseif data.color.red then
                color.r, color.g, color.b = data.color.red, data.color.green, data.color.blue
            end
        end
        data.color = color
    else
        local c = io.open(path..".color", "r")
        if c then
            local cdata = c:read("*all")
            io.close(c)
            local col = {}
            for tok in cdata:gmatch("[^%s]+") do
                col[#col+1] = tonumber(tok)
            end
            data.color = Color:new(col[1], col[2], col[3], 1)
        end
    end
    if not data.full_name then
        local n = io.open(path..".name", "r")
        if n then
            local ndata = n:read("*all")
            io.close(n)
            local col = {}
            for tok in ndata:gmatch("[^\r\n]+") do
                col[#col+1] = tok:gsub("^.-: (.-)$", "%1")
            end
            if col[1] then
                data.full_name = col[1]
            end
            if col[2] then
                data.short_name = col[2]
            end
        end
    end
    return data
end

function clean_name(name)
    local data = get_char_json(name)
    if data and data.full_name then
        return data.full_name
    end
    name = name:gsub("/Mods/Packs/", "")
    name = name:gsub("Data/Textures/", "")
    name = name:gsub("Entities/", "")
    name = name:gsub("fyi.", "")
    name = name:gsub("skins/", "")
    name = name:gsub("_full.png", "")
    name = name:gsub("char_", "")
    name = name:gsub(".png", "")
    name = name:gsub("/.*/", "/")
    return name
end

function clean_path(name)
    name = name:gsub("/Mods/Packs/", "")
    name = name:gsub("Data/Textures/", "")
    name = name:gsub("Entities/", "")
    if #name > 36 then
        name = name:gsub("/", "/\n")
    end
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
            for j=1,4 do
                if skins[j] == images[i].texture then
                    skins[j] = nil
                end
                replace_texture_and_heart_color(state.items.player_select[j].texture, state.items.player_select[j].texture)
            end
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
        XP = W / 11
        YP = H / 11
        N = math.floor(-2*AX / W)
    until math.ceil(#images/N)*H < 2*AY
end

-- load all skins on load
get_skins()

-- check for global inputs
buttons_prev = { 0, 0, 0, 0 }
function pressed(key)
    if test_mask(game_manager.game_props.buttons, key) and not test_mask(buttons_prev[1], key) then
        return true
    end
    return false
end
function released(key)
    if not test_mask(game_manager.game_props.buttons, key) and test_mask(buttons_prev[1], key) then
        return true
    end
    return false
end

-- check for player specific inputs
function pressed_alt(i, key)
    if test_mask(state.player_inputs.player_slots[i].buttons, key) and not test_mask(buttons_prev[i], key) then
        return true
    end
    return false
end
function released_alt(i, key)
    if not test_mask(state.player_inputs.player_slots[i].buttons, key) and test_mask(buttons_prev[i], key) then
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
    for i,v in pairs(get_entities_by({ENT_TYPE.ITEM_PICKUP_PLAYERBAG, ENT_TYPE.ITEM_CLIMBABLE_ROPE, ENT_TYPE.ITEM_UNROLLED_ROPE, ENT_TYPE.BG_SHOPWANTEDPORTRAIT, ENT_TYPE.ITEM_WHIP, ENT_TYPE.FX_PLAYERINDICATOR, ENT_TYPE.FX_PLAYERINDICATORPORTRAIT, ENT_TYPE.FX_BIRDIES}, MASK.ANY, LAYER.BOTH)) do
        local ent = get_entity(v)
        ent:set_texture(ent:get_texture())
    end
end

function get_name(tex)
    for _,img in pairs(images) do
        if img.texture == tex then
            return img.name, img.path
        end
    end
    return "Too Many Skins", "something"
end

-- replace the character names
function replace_strings()
    for tex,def in pairs(skin_def) do
        local full_id, short_id = texture_to_stringid(tex)
        change_string(full_id, vanilla_strings[tex].full)
        change_string(full_id+1, vanilla_strings[tex].bio)
        change_string(short_id, vanilla_strings[tex].short)
    end
    for i=1,active_players() do
        local tex = skins[i]
        if tex then
            local name, path = get_name(tex)
            local full_id, short_id = texture_to_stringid(state.items.player_select[i].texture)
            local data = get_char_json(path)
            if data then
                if data.full_name then
                    change_string(full_id, data.full_name)
                else
                    change_string(full_id, name)
                end
                if data.short_name then
                    change_string(short_id, data.short_name)
                else
                    change_string(short_id, name)
                end
                if data.color then
                    set_character_heart_color(state.items.player_select[i].character, data.color)
                end
                if stupid_story[state.items.player_select[i].texture] then
                    change_string(full_id+1, stupid_story[state.items.player_select[i].texture].." replaced with "..clean_path(path).."\nby Too Many Skins.")
                else
                    change_string(full_id+1, "Replaced with "..clean_path(path).."\nby Too Many Skins.")
                end
            elseif tex > 0x192 then
                change_string(full_id, name)
                if stupid_story[state.items.player_select[i].texture] then
                    change_string(full_id+1, stupid_story[state.items.player_select[i].texture].." replaced with "..clean_path(path).."\nby Too Many Skins.")
                else
                    change_string(full_id+1, "Replaced with "..clean_path(path).."\nby Too Many Skins.")
                end
                change_string(short_id, name)
            else
                change_string(full_id, "Fake "..vanilla_strings[tex].full)
                change_string(full_id+1, vanilla_strings[tex].bio)
                change_string(short_id, "Fake "..vanilla_strings[tex].short)
            end
        end
    end
end

-- replace or reset character textures
function replace_skin()
    local replaced = {}
    for i=1,4 do
        if skins[i] then
            replace_texture_and_heart_color(state.items.player_select[i].texture, skins[i])
            replaced[state.items.player_select[i].texture] = true
        elseif state.items.player_select[i].activated and not replaced[state.items.player_select[i].texture] then
            replace_texture_and_heart_color(state.items.player_select[i].texture, state.items.player_select[i].texture)
        end
        skin_def[state.items.player_select[i].texture] = get_texture_definition(state.items.player_select[i].texture)
    end
    replace_strings()
    set_textures()
end

-- show custom message on bottom right of character select wood panel
change_string(hash_to_stringid(0xcd07f25b), CUSTOM_SKIN_TEXT)
set_pre_render_screen(SCREEN.CHARACTER_SELECT, function(self, ctx)
    if state.items.player_select[1].activated then
        if state.screen_character_select.player_y[1] == 0 then
            state.screen_character_select.screen_panels.bottom_right_text = hash_to_stringid(0xcd07f25b)
        end
    end
    if state.screen_character_select.player_quickselect_shown[1] then
        if skins[1] then
            skins[1] = nil
            replace_skin()
        end
    end
end)

-- get number of active players
function active_players()
    local n = 0
    for i,p in pairs(state.items.player_select) do
        if p.activated then n = n + 1 end
    end
    return n
end

-- discover new files and reload active skins from disk
function refresh()
    get_skins()
    for i,skin in pairs(skins) do
        if skin and skin > 0x192 then
            local def = get_texture_definition(skin)
            clear_cache(def.texture_path)
        end
    end
    replace_skin()
end

-- draw the skin previews and cursor
function draw_skins(ctx)
    for i=1,active_players() do
        local sx = selected_skin[i] % N
        local sy = math.floor(selected_skin[i] / N)
        local sdest = AABB:new(
            AX + sx * W + XP,
            AY - sy * H - YP,
            AX + sx * W + W - XP,
            AY - sy * H - H + YP
        )
        local color = get_character_heart_color(state.items.player_select[i].character)
        color.a = 0.75
        ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_STICKERS_0, 2, 7, sdest, color)
    end
    local x = AX
    local y = AY
    for i, img in pairs(images) do
        local af = 0
        for j=1,active_players() do
            if i == selected_skin[j] + 1 then
                af = math.floor(get_frame() % 24 / 3) + 1
            end
        end
        ctx:draw_screen_texture(img.texture, 0, af, x + XP, y - YP, x + W - XP, y - H + YP, Color:white())
        for j=1,active_players() do
            if img.texture == get_type(state.items.player_select[j].character).texture then
                local color = get_character_heart_color(state.items.player_select[j].character)
                ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_MENU_DEATHMATCH2_0, 0, 7, x, y, x + W, y - H, color)
            end
        end
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
        local scale = 0.001
        local color = Color:new(0.25, 0.2, 0.2, 1)
        local reset = ""
        if images[selected_skin[1] + 1].texture == state.items.player_select[1].texture and skins[1] then
            reset = "(Reset) "
        end
        local name = images[selected_skin[1] + 1].name
        local line = "\u{83} "..reset..name.."   \u{86} Random   \u{85} Reset   \u{87} Refresh"
        while ctx:draw_text_size(line, scale, scale, VANILLA_FONT_STYLE.NORMAL) > -2*AX do
            scale = scale * 0.99
            line = "\u{83} "..reset..name.."   \u{86} Random   \u{85} Reset   \u{87} Refresh"
        end
        ctx:draw_text(line, 0.68, -0.68, scale, scale, color,
            VANILLA_TEXT_ALIGNMENT.RIGHT, VANILLA_FONT_STYLE.NORMAL)
    end
    if game_manager.pause_ui.visibility > PAUSEUI_VISIBILITY.INVISIBLE or (draw_select and state.screen ~= SCREEN.CHARACTER_SELECT) then
        local scale = 0.001
        local color = Color:white()
        color.a = game_manager.pause_ui.menu_slidein_progress
        ctx:draw_text(CUSTOM_SKIN_TEXT, 0, -0.85, scale, scale, color, VANILLA_TEXT_ALIGNMENT.CENTER, VANILLA_FONT_STYLE.NORMAL)
    end
end

-- draw the selector background
function draw_selector(ctx)
    for i=1,4 do
        if selected_skin[i] > #images-1 then
            selected_skin[i] = #images-1
        end
    end
    if draw_select then
        if state.pause == PAUSE.MENU or game_manager.pause_ui.visibility == PAUSEUI_VISIBILITY.VISIBLE then
            draw_select = false
            return
        end
        local src = Quad:new(AABB:new(0.525, 0.21, 0.86, 0.93))
        local dest = Quad:new(AABB:new(-1, 0.83, 1, -0.85))
        dest:rotate(math.pi/2, 0, 0)
        ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_TOP_MAIN_0, src, dest, Color:white())
        draw_skins(ctx)
    end
end

function draw_everything(ctx)
    if online.lobby.code ~= 0 then return end
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

-- don't allow same character
function resolve_characters()
    local used = {[state.items.player_select[1].character] = true}
    for i=2,active_players() do
        while used[state.items.player_select[i].character] do
            state.items.player_select[i].character = state.items.player_select[i].character + 1
            if state.items.player_select[i].character > ENT_TYPE.CHAR_CLASSIC_GUY then
                state.items.player_select[i].character = ENT_TYPE.CHAR_ANA_SPELUNKY
            end
        end
        state.items.player_select[i].texture = get_type(state.items.player_select[i].character).texture
        used[state.items.player_select[i].character] = true
    end
end

-- fix journal portrait coordinates from _full sheets
set_callback(function(ctx, type, page)
    if type ~= JOURNAL_PAGE_TYPE.PEOPLE then return end
    page = page:as_journal_page_people()
    local tex = page.page_number + TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0 - 2
    if tex > TEXTURE.DATA_TEXTURES_CHAR_ORANGE_0 then return end
    if skin_def[tex] then
        local tile = skin_def[tex].width/16
        local top = tile/skin_def[tex].height
        local bottom = tile*7/skin_def[tex].height
        page.character_drawing.source_top_left_y = top
        page.character_drawing.source_top_right_y = top
        page.character_drawing.source_bottom_left_y = bottom
        page.character_drawing.source_bottom_right_y = bottom
    else
        page.character_drawing.source_top_left_y = 0.0625
        page.character_drawing.source_top_right_y = 0.0625
        page.character_drawing.source_bottom_left_y = 0.437
        page.character_drawing.source_bottom_right_y = 0.437
    end
end, ON.RENDER_PRE_JOURNAL_PAGE)

set_callback(function(ctx, type, page)
    if type ~= JOURNAL_PAGE_TYPE.PEOPLE then return end
    page = page:as_journal_page_people()
    local tex = page.page_number + TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0 - 2
    if tex > TEXTURE.DATA_TEXTURES_CHAR_ORANGE_0 then return end
    if skin_def[tex] then
        local bg_source = page.character_background:source_get_quad()
        local bg_dest = page.character_background:dest_get_quad():offset(page.character_background.x, page.character_background.y)
        ctx:draw_screen_texture(TEXTURE.DATA_TEXTURES_JOURNAL_ENTRY_BG_0, bg_source, bg_dest, Color:white())
        local dest = page.character_icon:dest_get_quad():offset(page.character_icon.x, page.character_icon.y)
        ctx:draw_screen_texture(tex, 0, 0, dest, Color:white())
    end
end, ON.RENDER_POST_JOURNAL_PAGE)

-- disable inputs to underlaying char select screen when skin select is open and handle input
set_callback(function()
    if online.lobby.code ~= 0 then return end
    local ret = false
    if state.items.player_select[1].activated and
        ((state.screen == SCREEN.CHARACTER_SELECT and state.screen_character_select.player_y[1] == 0) or game_manager.pause_ui.visibility > PAUSEUI_VISIBILITY.INVISIBLE or draw_select) and pressed(0x20) then
        game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.INVISIBLE
        draw_select = not draw_select
        if (state.screen == SCREEN.LEVEL or state.screen == SCREEN.CAMP or state.screen == SCREEN.TRANSITION) and not draw_select then
            game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.VISIBLE
            ret = true
        end
        if active_players() > 1 and draw_select and state.screen == SCREEN.CHARACTER_SELECT then
            resolve_characters()
        end
    end
    if draw_select then
        if state.screen == SCREEN.CHARACTER_SELECT then
            state.screen_character_select.player_quickselect_shown[1] = false
            state.screen_character_select.player_quickselect_fadein_timer[1] = 0
        end
        if pressed(0x10000) and selected_skin[1] > 0 then --left
            selected_skin[1] = selected_skin[1] - 1
        elseif pressed(0x20000) and selected_skin[1] < #images-1 then --right
            selected_skin[1] = selected_skin[1] + 1
        elseif pressed(0x40000) and selected_skin[1] >= N then --up
            selected_skin[1] = selected_skin[1] - N
        elseif pressed(0x80000) and selected_skin[1] + N < #images then --down
            selected_skin[1] = selected_skin[1] + N
        elseif pressed(0x1) then --select
            skins[1] = images[selected_skin[1] + 1].texture
            replace_skin()
            draw_select = false
            if (state.screen == SCREEN.LEVEL or state.screen == SCREEN.CAMP or state.screen == SCREEN.TRANSITION) then
                game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
                ret = true
            end
        elseif released(0x2) then --cancel
            skins[1] = nil
            replace_skin()
            draw_select = false
            if (state.screen == SCREEN.LEVEL or state.screen == SCREEN.CAMP or state.screen == SCREEN.TRANSITION) then
                game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
            end
            ret = true
        elseif pressed(0x8) then --random
            selected_skin[1] = prng:random_index(#images, 0) - 1
        elseif pressed(0x80) then --refresh
            refresh()
        elseif pressed(0x800) and (state.screen == SCREEN.LEVEL or state.screen == SCREEN.CAMP or state.screen == SCREEN.TRANSITION) then --start
            draw_select = false
            game_manager.pause_ui.visibility = PAUSEUI_VISIBILITY.SLIDING_UP
            ret = true
        end
        for i=2,active_players() do
            if pressed_alt(i, INPUTS.LEFT) and selected_skin[i] > 0 then --left
                selected_skin[i] = selected_skin[i] - 1
            elseif pressed_alt(i, INPUTS.RIGHT) and selected_skin[i] < #images-1 then --right
                selected_skin[i] = selected_skin[i] + 1
            elseif pressed_alt(i, INPUTS.UP) and selected_skin[i] >= N then --up
                selected_skin[i] = selected_skin[i] - N
            elseif pressed_alt(i, INPUTS.DOWN) and selected_skin[i] + N < #images then --down
                selected_skin[i] = selected_skin[i] + N
            elseif pressed_alt(i, INPUTS.JUMP) then --select
                skins[i] = images[selected_skin[i] + 1].texture
                replace_skin()
            elseif released_alt(i, INPUTS.BOMB) then --cancel
                skins[i] = nil
                replace_skin()
            elseif pressed_alt(i, INPUTS.ROPE) then --random
                selected_skin[i] = prng:random_index(#images, 0) - 1
            end
        end
    end
    buttons_prev[1] = game_manager.game_props.buttons
    for i=2,4 do
        buttons_prev[i] = state.player_inputs.player_slots[i].buttons
    end
    return ret or draw_select
end, ON.PRE_UPDATE)
