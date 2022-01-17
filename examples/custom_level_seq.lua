meta.name = "Custom level test"
meta.version = "WIP"
meta.author = "Dregu"
meta.description = "Testing custom levels..."

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
local stats = {1, 0, 0}

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
    {file="level1.lvl", theme=THEME.DWELLING, bg=true, deco={1,2,4,5,7,8}, border_floor=ENT_TYPE.FLOOR_BORDERTILE_OCTOPUS},
    {file="level1.lvl", theme=THEME.JUNGLE, bg=true, deco={1,2,4,5,7,8}},
    {file="level1.lvl", theme=THEME.VOLCANA, camera=THEME.VOLCANA, bg=true, deco={1,2,4,5,7}, floor=TEXTURE.DATA_TEXTURES_FLOOR_EGGPLANT_0, border_floor=ENT_TYPE.FLOOR_BORDERTILE_METAL},
    {file="level1.lvl", theme=THEME.TIDE_POOL, bg=true, deco={1,2,4,5,7,8}},
    {file="level1.lvl", theme=THEME.TEMPLE, bg=true, deco={1,2,4,5,7,8}},
    {file="level1.lvl", theme=THEME.CITY_OF_GOLD, bg=true, deco={1,2,4,5,7,8}},
    {file="level1.lvl", theme=THEME.ICE_CAVES, base=THEME.ICE_CAVES, camera=THEME.ICE_CAVES, bg=true, deco={1,2,4,5,7,8}, on_level=function() state.camera.bounds_bottom = 120.5-8*state.height end },
    {file="level1.lvl", theme=THEME.NEO_BABYLON, bg=true, deco={1,2,4,5,7}},
    {file="level1.lvl", theme=THEME.EGGPLANT_WORLD, bg=true, deco={1,2,4,5,7}},
    {file="level1.lvl", theme=THEME.SUNKEN_CITY, bg=true, deco={1,2,4,5,7}},
    {file="level1.lvl", theme=THEME.DUAT, bg=true, base=THEME.DUAT, camera=THEME.DUAT, init=true, post=true, border_type=1}
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

        if current.theme == THEME.TIDE_POOL then
            current.theme = THEME.ABZU
        elseif current.theme == THEME.SUNKEN_CITY then
            current.theme = THEME.HUNDUN
        end

        if not current.custom then
            current.custom = CustomTheme:new()
        end

        state.theme = current.theme
        current.custom.init = current.init or false
        current.custom.bg = current.bg or false
        current.custom.theme = current.theme-1
        current.custom.base_theme = current.theme-1
        current.custom.custom_base_theme = current.base and current.base-1 or 0xFF
        current.custom.texture_theme = current.theme-1
        current.custom.camera_theme = current.camera and current.camera-1 or 0xFF
        current.custom.post_process = current.post or false
        current.custom.post_process_exit = current.post or false
        current.custom.textures[CUSTOM_TEXTURE.FLOOR] = current.floor or nil
        current.custom.border_floor = current.border_floor or 0xFFFFFFFF
        current.custom.border_type = current.border_type or 0

        current.custom.populate = true
        current.custom.unknown1 = false
        current.custom.unknown2 = false
        --[[current.custom.unknownv4 = true
        current.custom.unknownv5 = true
        current.custom.unknownv7 = true
        current.custom.unknownv8 = true
        current.custom.unknownv12 = true
        current.custom.unknownv30 = true
        current.custom.unknownv32 = true
        current.custom.unknownv37 = true
        current.custom.unknownv47 = true]]

        if next then
            current.custom.next_theme = next.theme
        end
        --state.level_flags = set_flag(state.level_flags, 4) -- disable pet
        --state.level_flags = set_flag(state.level_flags, 7) -- disable pot

        if all_checkpoint or current.checkpoint then
            checkpoint = state.level_count
            prinspect("save checkpoint", checkpoint)
        end

        force_custom_theme(current.custom)
    end
end, ON.PRE_LOAD_LEVEL_FILES)

set_callback(function()
    if not in_level() then return end
    if state.time_total > total_time then
        total_time = state.time_total
    end

    -- death counter
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

-- destroy starting pots and rocks
set_post_entity_spawn(function(ent, flags)
    local prev = get_entity(ent.uid-1)
    if prev and prev.type.id == ENT_TYPE.BG_DOOR then
        ent:destroy()
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_ROCK)

-- destroy starting skull and bones
set_post_entity_spawn(function(ent, flags)
    local prev = get_entity(ent.uid-1)
    local prev2 = get_entity(ent.uid-2)
    if prev and prev.type.id == ENT_TYPE.ITEM_BONES and prev2 and prev2.type.id == ENT_TYPE.BG_DOOR then
        ent:destroy()
        prev:destroy()
    end
end, SPAWN_TYPE.LEVEL_GEN, 0, ENT_TYPE.ITEM_SKULL)

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

set_callback(function()
    if not in_level() then return end
    if current.music then
        state.theme = current.music
    end

    if current.deco then uniform_styleddecoration(current.deco) end

    -- for hud
    --state.world = 42
    --state.level = state.level_count+1

    for i,p in ipairs(players) do
        p.health = stats[1]
        p.inventory.bombs = stats[2]
        p.inventory.ropes = stats[3]
    end
end, ON.POST_LEVEL_GENERATION)

set_callback(function()
    if not in_level() then return end
    if current and current.on_level then
        current.on_level()
    end
    if load_time and state.time_total < total_time then
        state.time_total = total_time
        load_time = false
    end
end, ON.LEVEL)

--[[TILECODES]]

--[[
-- oneway dustwalls that push you in whatever direction. doesn't affect movement if already moving in the right way
define_tile_code("dustwall_right")
set_pre_tile_code_callback(function(x, y, layer)
    local uid = spawn(ENT_TYPE.FLOOR_DUSTWALL, x, y, layer, 0, 0)
    local ent = get_entity(uid)
    ent.flags = clr_flag(ent.flags, ENT_FLAG.SOLID)
    ent.hitboxx = 0.45
    ent.hitboxy = 0.45
    set_pre_collision2(uid, function(self, collidee)
        if collidee.velocityx < 0.1 then collidee.velocityx = 0.1 end
        return true
    end)
end, "dustwall_right")

define_tile_code("dustwall_left")
set_pre_tile_code_callback(function(x, y, layer)
    local uid = spawn(ENT_TYPE.FLOOR_DUSTWALL, x, y, layer, 0, 0)
    local ent = get_entity(uid)
    ent.flags = clr_flag(ent.flags, ENT_FLAG.SOLID)
    ent.hitboxx = 0.45
    ent.hitboxy = 0.45
    set_pre_collision2(uid, function(self, collidee)
        if collidee.velocityx > -0.1 then collidee.velocityx = -0.1 end
        return true
    end)
end, "dustwall_left")

define_tile_code("dustwall_down")
set_pre_tile_code_callback(function(x, y, layer)
    local uid = spawn(ENT_TYPE.FLOOR_DUSTWALL, x, y, layer, 0, 0)
    local ent = get_entity(uid)
    ent.flags = clr_flag(ent.flags, ENT_FLAG.SOLID)
    ent.hitboxx = 0.45
    ent.hitboxy = 0.45
    set_pre_collision2(uid, function(self, collidee)
        if collidee.velocityy > -0.1 then collidee.velocityy = -0.1 end
        return true
    end)
end, "dustwall_down")

define_tile_code("dustwall_up")
set_pre_tile_code_callback(function(x, y, layer)
    local uid = spawn(ENT_TYPE.FLOOR_DUSTWALL, x, y, layer, 0, 0)
    local ent = get_entity(uid)
    ent.flags = clr_flag(ent.flags, ENT_FLAG.SOLID)
    ent.hitboxx = 0.45
    ent.hitboxy = 0.45
    set_pre_collision2(uid, function(self, collidee)
        if collidee.velocityy < 0.1 then collidee.velocityy = 0.1 end
        return true
    end)
end, "dustwall_up")

-- the one and only kaizo block
define_tile_code("kaizo_block")
set_pre_tile_code_callback(function(x, y, layer)
    local ent = get_entity(spawn(ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK, x, y, layer, 0, 0))
    ent.flags = set_flag(ent.flags, ENT_FLAG.NO_GRAVITY)
    ent.more_flags = set_flag(ent.more_flags, ENT_MORE_FLAG.DISABLE_INPUT)
    ent.flags = set_flag(ent.flags, ENT_FLAG.INVISIBLE)
    ent.flags = clr_flag(ent.flags, ENT_FLAG.SOLID)
    local trigger = get_entity(spawn_over(ENT_TYPE.LOGICAL_TENTACLE_TRIGGER, ent.uid, 0, 0))
    trigger.x = 0
    trigger.y = -0.5
    trigger.hitboxx = 0.22
    trigger.hitboxy = 0.05
    trigger.offsetx = 0
    trigger.offsety = 0
    set_pre_collision2(trigger.uid, function(self, collidee)
        local bx, by, bl = get_position(ent.uid)
        local cx, cy, cl = get_position(collidee.uid)
        if test_flag(ent.flags, ENT_FLAG.INVISIBLE) and collidee.velocityy > 0 and cy < y-0.3 then
            ent.flags = clr_flag(ent.flags, ENT_FLAG.INVISIBLE)
            ent.flags = set_flag(ent.flags, ENT_FLAG.SOLID)
            collidee.velocityy = -0.1
            spawn(ENT_TYPE.ITEM_GOLDCOIN, bx, by+0.6, bl, 0, 0.2)
            self:destroy()
        end
        return true
    end)
end, "kaizo_block")

-- horizontal elevator that activates when you step on it
define_tile_code("elevator_horizontal")
set_pre_tile_code_callback(function(x, y, layer)
    local ent = get_entity(spawn(ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP, x, y, layer, 0, 0))
    ent.angle = math.pi/2
    ent.animation_frame = 26
    for _,t in ipairs(entity_get_items_by(ent.uid, 0, MASK.LOGICAL)) do
        get_entity(t):destroy()
    end
    set_post_statemachine(ent.uid, function(ent)
        for _,p in ipairs(entity_get_items_by(ent.uid, 0, MASK.PLAYER)) do
            local player = get_entity(p)
            if player.idle_counter >= 30 and ent.move_state == 0 then
                local ex, ey, l = get_position(ent.uid)
                local right = get_grid_entity_at(ex+1, ey, l)
                if right == -1 then
                    ent.dirx = 1
                else
                    ent.dirx = -1
                end
                ent.move_state = 6
            end
        end
        if ent.dirx < 0 and ent.move_state == 6 then
            ent.animation_frame = 27
        elseif ent.dirx > 0 and ent.move_state == 6 then
            ent.animation_frame = 28
        else
            ent.animation_frame = 26
        end
    end)
end, "elevator_horizontal")

-- quillback who rolls forever after seen the player
define_tile_code("infinite_quillback")
set_pre_tile_code_callback(function(x, y, layer)
  local quill = get_entity(spawn(ENT_TYPE.MONS_CAVEMAN_BOSS, x, y, layer, 0, 0))
  set_post_statemachine(quill.uid, function(ent)
    if ent.seen_player then
      ent.move_state = 10
    end
  end)
end, "infinite_quillback")

define_tile_code("mosquito_aggro")
set_pre_tile_code_callback(function(x, y, layer)
  local fly = get_entity(spawn(ENT_TYPE.MONS_MOSQUITO, x, y, layer, 0, 0))
  fly.move_state = 6
end, "mosquito_aggro")

-- queen bee who breaks tiles
define_tile_code("brute_bee")
set_pre_tile_code_callback(function(x, y, layer)
  local uid = spawn_critical(ENT_TYPE.MONS_QUEENBEE, x, y, layer, 0, 0)
  set_post_statemachine(uid, function(ent)
    local x, y, l = get_position(ent.uid)
    local dx = math.random(-1, 1)
    local dy = math.random(-1, 1)
    local floor = get_grid_entity_at(x+dx, y+dy, l)
    if floor ~= -1 then
      local ent = get_entity(floor)
      if test_flag(ent.flags, ENT_FLAG.SOLID) and not test_flag(ent.flags, ENT_FLAG.INDESTRUCTIBLE_OR_SPECIAL_FLOOR) then
        kill_entity(floor)
      end
    end
  end)
end, "brute_bee")

set_post_entity_spawn(function(ent)
    ent.health = 9
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_HORNEDLIZARD)
]]
