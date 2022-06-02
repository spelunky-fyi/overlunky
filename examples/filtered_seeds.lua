meta.name = "Filtered seeds"
meta.version = "0.1"
meta.description = "Resets until a seed is found that has the selected items in a 1-2 shop."
meta.author = "Freshjive_"

function item_type_option(name, desc, type, default)
    return {name = name, desc = desc, type = type, default = default or false}
end

local item_type_options = {
    item_type_option("fs01_jetpack", "Jetpack", ENT_TYPE.ITEM_PURCHASABLE_JETPACK, true),
    item_type_option("fs02_teleporter", "Teleporter", ENT_TYPE.ITEM_TELEPORTER),
    item_type_option("fs03_bombbox", "Bomb Box", ENT_TYPE.ITEM_PICKUP_BOMBBOX),
    item_type_option("fs04_telepack", "Telepack", ENT_TYPE.ITEM_PURCHASABLE_TELEPORTER_BACKPACK),
    item_type_option("fs05_hoverpack", "Hoverpack", ENT_TYPE.ITEM_PURCHASABLE_HOVERPACK),
    item_type_option("fs06_cape", "Cape", ENT_TYPE.ITEM_PURCHASABLE_CAPE),
    item_type_option("fs07_plasmacannon", "Plasma Cannon", ENT_TYPE.ITEM_PLASMACANNON),
    item_type_option("fs08_paste", "Paste", ENT_TYPE.ITEM_PICKUP_PASTE),
    item_type_option("fs09_spikeshoes", "Spike Shoes", ENT_TYPE.ITEM_PICKUP_SPIKESHOES),
    item_type_option("fs10_springshoes", "Spring Shoes", ENT_TYPE.ITEM_PICKUP_SPRINGSHOES),
    item_type_option("fs11_compass", "Compass", ENT_TYPE.ITEM_PICKUP_COMPASS),
    item_type_option("fs12_pitchersmitt", "Pitchers Mitt", ENT_TYPE.ITEM_PICKUP_PITCHERSMITT),
    item_type_option("fs13_climbinggloves", "Climbing Gloves", ENT_TYPE.ITEM_PICKUP_CLIMBINGGLOVES),
    item_type_option("fs14_spectacles", "Spectacles", ENT_TYPE.ITEM_PICKUP_SPECTACLES),
    item_type_option("fs15_skeletonkey", "Skeleton Key", ENT_TYPE.ITEM_PICKUP_SKELETON_KEY),
    item_type_option("fs16_bombbag", "Bomb Bag", ENT_TYPE.ITEM_PICKUP_BOMBBAG),
    item_type_option("fs17_present", "Present", ENT_TYPE.ITEM_PRESENT),
}
for i, option in ipairs(item_type_options) do
    register_option_bool(option.name, option.desc, option.default)
end
register_option_bool("fs_altar", "Require a 1-2 Kali Altar", false)
register_option_bool("fs_backlayer", "Also search in back layer shop", false)

local skip_reset = false
local in_run = true  -- Don"t start searching until after a manual reset.
local black_screen_cb = nil
local reset_counter = 0

function newseed()
    reset_counter = reset_counter + 1
    if reset_counter % 25 == 0 then
        print("Reset counter: " .. reset_counter)
    end
    seed = math.random(0xFFFFFFFF)
    -- seed = 0x41C485EB -- to test individual seeds
    set_seed(seed)
end

function black_screen(draw_ctx)
    if not in_run then
        state.fadein = 0
        state.fadeout = 0
        state.fadevalue = 0
    end
    local cam = state.camera
    sx, sy = screen_position(state.camera.bounds_left, state.camera.bounds_top) -- top left
    sx2, sy2 = screen_position(state.camera.bounds_right, state.camera.bounds_bottom) -- bottom right
    draw_ctx:draw_rect_filled(sx-0.1, sy+0.1, sx2+0.1, sy2-0.1, 0, rgba(0, 0, 0, 255))
end

set_callback(function()
    if in_run then
        if black_screen_cb ~= nil then
            clear_callback(black_screen_cb)
            black_screen_cb = nil
        end
        return
    end
    if (state.world == 1 and state.level == 1) then
        warp(1, 2, THEME.DWELLING)
        return
    end

    if options.fs_altar then
        local tiles = get_entities_by(ENT_TYPE.FLOOR_ALTAR, MASK.FLOOR, LAYER.FRONT)
        if #tiles == 0 then
            newseed()
            return
        end
    end

    local search_items = {}
    for i, option in pairs(item_type_options) do
        if options[option.name] then table.insert(search_items, option.type) end
    end

    local layer = LAYER.FRONT
    if options.fs_backlayer or options.fs07_plasmacannon then
      layer = LAYER.BOTH
    end

    local uids = get_entities_by(search_items, MASK.ITEM, layer)
    local found = {}
    for i, uid in ipairs(uids) do
        local ent = get_entity(uid)
        if found[ent.type.id] == nil and is_inside_shop_zone(ent.x, ent.y, ent.layer) then
            found[ent.type.id] = true
        end
    end
    for _, ent_type in ipairs(search_items) do
        if found[ent_type] == nil then
            newseed()
            return
        end
    end

    -- Found all items, reset to 1-1 and set flags so we stop searching.
    print("Found seed " .. string.format("%X", state.seed) .. " after " .. reset_counter .. " resets")
    skip_reset = true
    in_run = true
    set_seed(state.seed)
end, ON.LEVEL)

set_callback(function()
    if skip_reset then
        skip_reset = false
        reset_counter = 0
        return
    end
    if in_run then
        print("Searching for a seed...")
        in_run = false
        black_screen_cb = set_callback(black_screen, ON.GUIFRAME)
        newseed()
    end
end, ON.RESET)
