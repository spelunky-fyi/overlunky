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

local customtheme
local current
local next
local num = 1
local won = false

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
    {file="level1.lvl", theme=THEME.DWELLING, border_floor=ENT_TYPE.FLOOR_BORDERTILE_OCTOPUS},
    {file="level1.lvl", theme=THEME.JUNGLE, music=THEME.ABZU},
    {file="level1.lvl", theme=THEME.VOLCANA, floor=TEXTURE.DATA_TEXTURES_FLOOR_EGGPLANT_0, border_floor=ENT_TYPE.FLOOR_BORDERTILE_METAL},
    {file="level1.lvl", theme=THEME.ABZU, border_type=1, camera=THEME.ABZU, music=THEME.TIDE_POOL, post=true},
    {file="level1.lvl", theme=THEME.TEMPLE},
    {file="level1.lvl", theme=THEME.CITY_OF_GOLD},
    {file="level1.lvl", theme=THEME.ICE_CAVES, on_level=function() state.camera.bounds_bottom = 120.5-8*state.height end },
    {file="level1.lvl", theme=THEME.NEO_BABYLON},
    {file="level1.lvl", theme=THEME.EGGPLANT_WORLD},
    {file="level1.lvl", theme=THEME.HUNDUN, music=THEME.SUNKEN_CITY},
    {file="level1.lvl", theme=THEME.DUAT, init=true, border_type=1}
}

local function in_level()
    return state.screen == SCREEN.LEVEL and #levels > state.level_count
end

local function win()
    prinspect("congratulation")
    won = true
    state.screen_next = SCREEN.CONSTELLATION
    state.win_state = 3
    state:force_current_theme(THEME.COSMIC_OCEAN)
end

set_callback(function()
    won = false
end, ON.CAMP)

set_callback(function(ctx)
    if state.screen ~= SCREEN.LEVEL then return end
    if not customtheme then
        customtheme = CustomTheme:new()
    end
    if #levels >= state.level_count+1 and not won then
        num = state.level_count+1
        current = levels[num]
        next = levels[num+1]
        ctx:override_level_files({current.file})

        state.theme = current.theme
        state.level_flags = set_flag(state.level_flags, 4) -- disable pet
        state.level_flags = set_flag(state.level_flags, 7) -- disable pot

        customtheme.theme = state.theme-1
        customtheme.base_theme = state.theme-1
        customtheme.border_type = 1

        customtheme.init = current.init or false
        customtheme.post_process = current.post or false
        customtheme.post_process_exit = current.post or false
        --customtheme.special = true

        --customtheme.post_process = true
        --customtheme.post_process_decoration = true
        --customtheme.post_process_exit = true
        --customtheme.post_process_entity = true


        --customtheme.unknownv4 = true
        --customtheme.unknownv5 = true
        --customtheme.unknownv7 = true
        --customtheme.unknownv8 = true
        --customtheme.unknownv12 = true
        --customtheme.unknownv30 = true
        --customtheme.unknownv32 = true
        --customtheme.unknownv37 = true
        --customtheme.unknownv47 = true

        --customtheme.procedural_spawn = true
        --customtheme.procedural_level_gen = true

        if next then
            customtheme.next_theme = next.theme
        end

        if current.floor then
            customtheme.textures[CUSTOM_TEXTURE.FLOOR] = current.floor
        else
            customtheme.textures[CUSTOM_TEXTURE.FLOOR] = nil
        end
        if current.border_floor then
            customtheme.border_floor = current.border_floor
        else
            customtheme.border_floor = 0xFFFFFFFF
        end
        if current.border_type then
            customtheme.border_type = current.border_type
        else
            customtheme.border_type = 0
        end
        if current.camera then
            customtheme.camera_theme = current.camera
        else
            customtheme.camera_theme = 0
        end
        force_custom_theme(customtheme)
    end
end, ON.PRE_LOAD_LEVEL_FILES)

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

set_callback(function()
    if not in_level() then return end
    if current.dark then
        state.level_flags = set_flag(state.level_flags, 18)
    else
        state.level_flags = clr_flag(state.level_flags, 18)
    end
end, ON.PRE_LEVEL_GENERATION)

set_callback(function()
    if not in_level() then return end
    if current.music then
        state.theme = current.music
    end
end, ON.POST_LEVEL_GENERATION)

set_callback(function()
    if not in_level() then return end
    if current and current.on_level then
        current.on_level()
    end
end, ON.LEVEL)

--[[TILECODES]]

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

set_post_entity_spawn(function(ent)
    ent.health = 9
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_HORNEDLIZARD)
