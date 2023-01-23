meta = {
	name = "Too Many Skins",
	version = "WIP",
	description = "Load extra character mods from all available packs or the skins subfolder.",
	author = "Dregu"
}

DIR = "skins/"
texture_def = get_texture_definition(TEXTURE.DATA_TEXTURES_CHAR_YELLOW_0)
skins = {}

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
    return skin_id
end

function create_thumbnail(path)
    local w, h = get_image_size(path)
    local ts = math.floor(w/16)
    local texture = create_image_crop(path, 0, 0, ts, ts)
    images[#images+1] = {name=path, texture=texture, w=w, h=h}
end

-- get all installed char mods or png files in the skins folder and create option buttons for them
function get_skins()
    images = {}
    for i,v in pairs(list_char_mods()) do
        create_thumbnail(v)
    end

    for i,v in pairs(list_dir(DIR)) do
        if string.match(v, ".png") then
            create_thumbnail(v)
        end
    end

    register_option_callback("skin", 0, function(ctx)
        for i,img in pairs(images) do
            local uvx = 1/16
            local uvy = uvx*img.w/img.h
            if ctx:win_imagebutton(img.name, img.texture, 48, 48, 0, 0, 1, 1) then
                skins[1] = create_skin(img.name, img.w, img.h)
                if players[1] then hook_skin(players[1]) end
            end
            if i % 5 ~= 0 and i < #images then
                ctx:win_sameline(0, 4)
            end
        end
    end)
end

register_option_button("_reload", "Reload skins", "", get_skins)
register_option_button("_reset", "Reset skin", "", function() skins = {} end)

get_skins()

function hook_skin(ent)
    if not ent then return end
    ent:set_post_update_state_machine(function(ent)
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



-- wip ui stuff here

buttons_prev = 0
function pressed_tab()
    if test_mask(game_manager.game_props.buttons, 4096) and not test_mask(buttons_prev, 4096) then
        buttons_prev = game_manager.game_props.buttons
        return true
    else
        buttons_prev = game_manager.game_props.buttons
        return false
    end
end

-- clear some journal pages for character select
set_callback(function(chapter, pages)
    if state.screen == SCREEN.CHARACTER_SELECT then
        table.remove(pages, 1)
        table.insert(pages, 1, 101)
        table.remove(pages, 2)
        table.insert(pages, 2, 102)
        return pages
    end
end, ON.POST_LOAD_JOURNAL_CHAPTER)

-- show custom message on bottom right
change_string(hash_to_stringid(0xcd07f25b), "\u{8A} Select custom skin")
set_pre_render_screen(SCREEN.CHARACTER_SELECT, function(self, ctx)
    if state.items.player_select[1].activated then
        state.screen_character_select.right_button_text_id = hash_to_stringid(0xcd07f25b)
    end
end)

-- disable inputs to underlaying char select screen when journal is open
set_callback(function()
    if state.screen ~= SCREEN.CHARACTER_SELECT then return end
    if state.items.player_select[1].activated and pressed_tab() then
        if game_manager.journal_ui.state == JOURNALUI_STATE.INVISIBLE then
            show_journal(0, 0)
            game_manager.journal_ui.max_page_count = 1
        else
            toggle_journal()
        end
    end
    if game_manager.journal_ui.state > 0 then
        if game_manager.journal_ui.flipping_to_page ~= 2 then
            --toggle_journal()
        end
        return true
    end
end, ON.PRE_UPDATE)
