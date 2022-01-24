meta.name = "Custom theme"
meta.version = "WIP"
meta.author = "Dregu"
meta.description = "Testing custom themes."

local borders = {ENT_TYPE.FLOOR_BORDERTILE, ENT_TYPE.FLOOR_BORDERTILE_METAL, ENT_TYPE.FLOOR_BORDERTILE_OCTOPUS}
local crust_items = {ENT_TYPE.EMBED_GOLD, ENT_TYPE.EMBED_GOLD_BIG, ENT_TYPE.ITEM_DIAMOND, ENT_TYPE.ITEM_EMERALD, ENT_TYPE.ITEM_SAPPHIRE, ENT_TYPE.ITEM_RUBY}

names = {}
for i,v in pairs(ENT_TYPE) do
  names[v] = i
end

theme_names = {}
for i,v in pairs(THEME) do
  theme_names[v] = i
end

local function pick(from)
    return from[prng:random(#from)]
end

local function has(arr, item)
    for i, v in pairs(arr) do
        if v == item then
            return true
        end
    end
    return false
end

local current
local next
local num = 1
local won = false
local all_checkpoint = true
local checkpoint = 0
local total_time = 0
local deaths = 0
local load_time = false
local stats = {99, 99, 99}

local theme_index = 100

local room_templates = {}
for x = 0, 7 do
    local room_templates_x = {}
    for y = 0, 14 do
        local room_template = define_room_template("setroom" .. y .. "_" .. x, ROOM_TEMPLATE_TYPE.NONE)
        room_templates_x[y] = room_template
    end
    room_templates[x] = room_templates_x
end

local levels = {
    {file="level1.lvl", theme=THEME.DWELLING, floor=TEXTURE.DATA_TEXTURES_FLOOR_EGGPLANT_0, bg=TEXTURE.DATA_TEXTURES_BG_EGGPLANT_0},
    {file="level2.lvl", theme=THEME.JUNGLE},
}

local function in_level()
    return state.screen == SCREEN.LEVEL and #levels > state.level_count
end

local function win()
    prinspect("congratulation")
    won = true
    state.items.player_inventory[1].collected_money_total = deaths
    state.screen_next = SCREEN.CONSTELLATION
    state.win_state = 3
    state:force_current_theme(THEME.COSMIC_OCEAN)
end

set_callback(function()
    won = false
    checkpoint = 0
    total_time = 0
    deaths = 0
end, ON.CAMP)

set_callback(function(ctx)
    if state.screen ~= SCREEN.LEVEL then return end
    if checkpoint > 0 and #levels >= checkpoint+1 and test_flag(state.quest_flags, 1) and not won then
        prinspect("load checkpoint", checkpoint)
        state.level_count = checkpoint
        deaths = deaths + 1
        load_time = true
    end
    if #levels >= state.level_count+1 and not won then
        num = state.level_count+1
        current = levels[num]
        next = levels[num+1]
        ctx:override_level_files({current.file})
        local random_border = pick(borders)

        if not current.custom then
            current.custom = CustomTheme:new()
            --current.custom.level_file = current.file -- this may do something if we loaded multiple files
            current.custom.theme = theme_index -- unique index that doesn't collide with the game themes
            current.custom.base_theme = current.theme -- set base theme for enabled overrides that don't have a specific theme or function
            current.custom.textures[DYNAMIC_TEXTURE.FLOOR] = current.floor or nil -- set texture used by FLOOR_GENERIC
            current.custom.textures[DYNAMIC_TEXTURE.BACKGROUND] = current.bg or nil
            -- override(themeinfo_virtual_to_override, bool / theme / function)
            current.custom:override(THEME_OVERRIDE.PLAYER_DAMAGE, false) -- disable player damage and resource use (like in camp)
            current.custom:override(THEME_OVERRIDE.TEXTURE_DYNAMIC, THEME.NEO_BABYLON) -- get the rest of the textures from neobab
            current.custom:override(THEME_OVERRIDE.EFFECTS, THEME.VOLCANA) -- get those hot effects from volcana
            current.custom:post(THEME_OVERRIDE.EFFECTS, function() -- remove the camera bounds just set by the theme though
                prinspect("Fix camera for the loop here, so it won't yank to position on level start")
                state.camera.adjusted_focus_x = state.level_gen.spawn_x
                state.camera.adjusted_focus_y = state.level_gen.spawn_y + 0.05
                state.camera.bounds_left = -math.huge
                state.camera.bounds_top = math.huge
                state.camera.bounds_right = math.huge
                state.camera.bounds_bottom = -math.huge
            end)
            -- make level loop like co
            current.custom:override(THEME_OVERRIDE.BORDER, THEME.COSMIC_OCEAN)
            current.custom:override(THEME_OVERRIDE.LOOP, THEME.COSMIC_OCEAN)

            theme_index = theme_index + 1
        end
        state.theme = current.theme

        if next then
            current.custom:override(THEME_OVERRIDE.PRE_TRANSITION, function()
                prinspect("Set theme for the right transition floor based on next level in our sequence")
                state.theme_next = next.theme
            end)
        end

        if all_checkpoint or current.checkpoint then
            checkpoint = state.level_count
            prinspect("save checkpoint", checkpoint)
        end

        force_custom_theme(current.custom)
    end
end, ON.PRE_LOAD_LEVEL_FILES)

set_callback(function()
    if not in_level() then return end
    -- save total time across all attempts
    if state.time_total > total_time then
        total_time = state.time_total
    end

    -- use money as death counter
    for i,p in ipairs(players) do
        if i == 1 then
            p.inventory.money = 0
            p.inventory.collected_money_total = deaths
        else
            p.inventory.money = 0
            p.inventory.collected_money_total = 0
        end
    end
    state.money_shop_total = 0

end, ON.GAMEFRAME)

-- win if out of levels
set_callback(function()
    if #levels < state.level_count+1 and state.loading == 2 and state.screen_next == SCREEN.TRANSITION then
        win()
    end
end, ON.LOADING)

-- destroy treasure, random pots
set_post_entity_spawn(function(ent, flags)
    ent:destroy()
end, SPAWN_TYPE.LEVEL_GEN_GENERAL, MASK.ITEM)

-- destroy embed treasure
set_post_entity_spawn(function(ent, flags)
    ent.flags = set_flag(ent.flags, ENT_FLAG.DEAD)
    ent:destroy()
end, SPAWN_TYPE.LEVEL_GEN_TILE_CODE, 0, crust_items)

-- set LevelSequence room templates and force dark levels
set_callback(function(ctx)
    if not in_level() then return end
    for x = 0, state.width - 1 do
        for y = 0, state.height - 1 do
            ctx:set_room_template(x, y, LAYER.FRONT, room_templates[x][y])
        end
    end
    if current.dark then
        state.level_flags = set_flag(state.level_flags, 18)
    else
        state.level_flags = clr_flag(state.level_flags, 18)
    end
end, ON.POST_ROOM_GENERATION)

-- spawn random midbg decorations around the level
local function spawn_deco(x, y, l, deco)
    local uid = spawn_critical(ENT_TYPE.MIDBG_STYLEDDECORATION, x, y, LAYER.FRONT, 0, 0)
    local ent = get_entity(uid)
    ent.animation_frame = pick(deco)
end
local function uniform_styleddecoration(deco)
    local num = 1
    local ax, ay, bx, by = get_bounds()
    for x=ax+math.random()*4+num,bx,math.random()*5+num*2 do
        for y=by+math.random()*4+num,ay,math.random()*5+num*2 do
            spawn_deco(x+math.random(-num*2, num*2), y+math.random(-num, num), LAYER.FRONT, deco)
        end
    end
end

-- set music, hud level and resources
set_callback(function()
    if not in_level() then return end
    if current.music then
        state.theme = current.music
    end

    if current.deco then uniform_styleddecoration(current.deco) end

    -- for hud
    state.world = 1
    state.level = state.level_count+1

    for i,p in ipairs(players) do
        p.health = stats[1]
        p.inventory.bombs = stats[2]
        p.inventory.ropes = stats[3]
    end
end, ON.POST_LEVEL_GENERATION)

-- add accumulated total time and run level hooks
set_callback(function()
    if not in_level() then return end
    if current and current.on_level then
        current.on_level()
    end
    if load_time and state.time_total < total_time then
        state.time_total = total_time
        load_time = false
    end
    if current.name then
        cancel_toast()
        toast(current.name)
    end
end, ON.LEVEL)
