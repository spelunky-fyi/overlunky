meta.name = "Seed Finder"
meta.version = "1.0"
meta.description = "Script your own success condition and find some seeds, in adventure or seeded mode!\n\nREAD THE SOURCE FOR MORE INFORMATION OR ENJOY FINDING CAVEMAN SHOPS FOREVER!"
meta.author = "Dregu"


--[[
By default, this script just finds caveman shops.
To script it, you need to import it to your own script,
or the Overlunky console and change the success condition:

-- Find shop jetpack in the front layer:
finder = import("dregu/seedfinder")
finder.success = function()
    return get_entities_by(ENT_TYPE.ITEM_PURCHASABLE_JETPACK, MASK.ITEM, LAYER.FRONT)[1] ~= nil
end

-- You can also set the position or uid of the found thing, and you will be pointed to it
-- Find crust jetpack:
finder = import("dregu/seedfinder")
finder.success = function()
    for i,v in ipairs(get_entities_by(ENT_TYPE.ITEM_JETPACK, MASK.ITEM, LAYER.FRONT)) do
        --finder.x, finder.y, _ = get_position(v)
        finder.uid = v
        return true
    end
end

-- Find crust teleporter close to entrance
finder = import("dregu/seedfinder")
finder.success = function()
    for i,v in ipairs(get_entities_by(ENT_TYPE.ITEM_TELEPORTER, MASK.ITEM, LAYER.FRONT)) do
        if distance(players[1].uid, v) < 10 then
            finder.uid = v
            return true
        end
    end
end

]]

exports = {
    deepest_world = 8,
    deepest_level = 98,
    x = 0,
    y = 0,
    uid = -1,
    world = -1,
    level = -1,
    theme = -1,

    success = function()
        for i,v in ipairs(get_entities_by_type(ENT_TYPE.MONS_CAVEMAN_SHOPKEEPER)) do
            exports.x, exports.y, _ = get_position(v)
            return true
        end
    end,

    reset = function()
        return state.world >= exports.deepest_world and state.level >= exports.deepest_level
    end,

    volcana = false,
    temple = false,
    cog = false,
    abzuduat = false,
    sunken = false,
    eggplant = false,
    cosmic = false,

    seeded = true,
    find = false,
    msg = "",
    a = 0,
    b = 0,
    start = 0,
    seeds = 1,
    levels = 1,

    next = function(w, l, t)
        local nw, nl, nt = 0, 0, 0
        if w == 1 then
            if l < 4 then
                nw = w
                nl = l + 1
                nt = t
            else
                nw = 2
                nl = 1
                nt = THEME.JUNGLE
                if exports.volcana then nt = THEME.VOLCANA end
            end
        end

        if w == 2 then
            if l < 4 then
                nw = w
                nl = l + 1
                nt = t
            else
                nw = 3
                nl = 1
                nt = THEME.OLMEC
            end
        end

        if w == 3 then
            nw = 4
            nl = 1
            nt = THEME.TIDE_POOL
            if exports.temple then nt = THEME.TEMPLE end
        end

        if w == 4 then
            if l == 1 then
                nw = w
                nl = l + 1
                nt = t
            elseif l == 2 then
                if t == THEME.TIDE_POOL then
                    nw = w
                    nl = l + 1
                    nt = t
                else
                    nw = w
                    nl = l + 1
                    nt = t
                    if exports.cog then nt = THEME.CITY_OF_GOLD end
                end
            elseif l == 3 then
                if t == THEME.TIDE_POOL then
                    nw = w
                    nl = l + 1
                    nt = t
                    if exports.abzuduat then nt = THEME.ABZU end
                elseif t == THEME.TEMPLE then
                    nw = w
                    nl = l + 1
                    nt = t
                elseif t == THEME.CITY_OF_GOLD then
                    nw = w
                    nl = l + 1
                    nt = THEME.TEMPLE
                    if exports.abzuduat then nt = THEME.DUAT end
                end
            elseif l == 4 then
                nw = 5
                nl = 1
                nt = THEME.ICE_CAVES
            end
        end

        if w == 5 then
            nw = 6
            nl = 1
            nt = THEME.NEO_BABYLON
        end

        if w == 6 then
            if l < 3 then
                nw = w
                nl = l + 1
                nt = t
            elseif l == 3 then
                nw = w
                nl = l + 1
                nt = THEME.TIAMAT
            else
                if exports.sunken then
                    nw = 7
                    nl = 1
                    nt = THEME.SUNKEN_CITY
                else
                    return false
                end
            end
        end

        if w == 7 then
            if l < 3 then
                nw = w
                nl = l + 1
                nt = THEME.SUNKEN_CITY
                if l == 1 and exports.eggplant then nt = THEME.EGGPLANT_WORLD end
            elseif l == 3 then
                nw = w
                nl = l + 1
                nt = THEME.HUNDUN
            elseif l == 4 then
                if exports.cosmic then
                    nw = 8
                    nl = l + 1
                    nt = THEME.COSMIC_OCEAN
                else
                    return false
                end
            end
        end

        if w == 8 then
            if l < 98 then
                nw = w
                nl = l + 1
                nt = t
            else
                return false
            end
        end

        if nw == 0 or nl == 0 or nt == 0 then
            return false
        end

        return {world=nw, level=nl, theme=nt}
    end
}

function stats()
    return F"\n{(get_ms()-exports.start)/1000} seconds\n{exports.seeds} seeds ({string.format('%.2f',exports.seeds/(get_ms()-exports.start)*1000)} sps)\n{exports.levels} levels ({string.format('%.2f',exports.levels/(get_ms()-exports.start)*1000)} lps)"
end

set_callback(function()
    state.fadeout = 0
    state.fadein = 0
    state.fadevalue = 0
    state.loading_black_screen_timer = 0

    if not exports.find then return end

    local next = exports.next(state.world, state.level, state.theme)

    if exports.success() then -- found the thing, lets celebrate
        exports.msg = F"Found thing in {state.world}-{state.level}" .. stats()
        exports.world = state.world
        exports.level = state.level
        exports.theme = state.theme
        exports.find = false
    elseif exports.reset() or not next then -- reached deepest level or can't figure out next level, lets reset
        if test_flag(state.quest_flags, 7) then
            state.seed = math.random(0, 0xFFFFFFFF)
        end
        state.quest_flags = state.quest_flags | 1
        state.world_next = state.world_start
        state.level_next = state.level_start
        state.theme_next = state.theme_start
        exports.seeds = exports.seeds + 1
        exports.levels = exports.levels + 1
        exports.msg = F"Finding your thing" .. stats()
        state.loading = 1
    else -- load next level
        state.world_next = next.world
        state.level_next = next.level
        state.theme_next = next.theme
        if not test_flag(state.quest_flags, 7) then
            state.screen_next = SCREEN.TRANSITION
        else
            state.screen_next = SCREEN.LEVEL
        end
        exports.levels = exports.levels + 1
        exports.msg = F"Finding your thing" .. stats()
        state.loading = 1
    end
end, ON.LEVEL)

set_callback(function()
    state.screen_next = SCREEN.LEVEL
    state.loading = 1
end, ON.TRANSITION)

set_callback(function()
    if test_flag(state.quest_flags, 1) and state.screen_next == SCREEN.LEVEL then
        exports.a, exports.b = get_adventure_seed()
    end
    state.fadeout = 0
    state.fadein = 0
    state.fadevalue = 0
    state.loading_black_screen_timer = 0
end, ON.PRE_LOAD_SCREEN)

set_callback(function(ctx)
    state.fadeout = 0
    state.fadein = 0
    state.fadevalue = 0
    state.loading_black_screen_timer = 0
    ctx:window("Scriptable Seed Finder", 0, 0, 0, 0, true, function()
        exports.seeded = ctx:win_check("Seeded mode", exports.seeded)
        if exports.seeded then
            state.seed = tonumber(ctx:win_input_text("Seed", string.format("%08X", state.seed)), 16)
            state.quest_flags = set_flag(state.quest_flags, 7)
        else
            exports.a = tonumber(ctx:win_input_text("First", string.format("%016X", exports.a)), 16)
            exports.b = tonumber(ctx:win_input_text("Second", string.format("%016X", exports.b)), 16)
            state.quest_flags = clr_flag(state.quest_flags, 7)
        end

        if not exports.find and ctx:win_button("Find") then
            exports.find = true
            exports.start = get_ms()
            exports.seeds = 1
            exports.levels = 1
            exports.x, exports.y = 0, 0
            exports.uid = -1
            exports.world = -1
            exports.level = -1
            exports.theme = -1
            state.quest_flags = 0x1
            if exports.seeded then
                state.quest_flags = set_flag(state.quest_flags, 7)
            end
            state.seed = math.random(0, 0xFFFFFFFF)
            state.world_next = state.world_start
            state.level_next = state.level_start
            state.theme_next = state.theme_start
            state.loading = 1
            exports.msg = "Finding your thing" .. stats()
        end
        if exports.find and ctx:win_button("Stop") then
            exports.find = false
            exports.msg = F"Stopped" .. stats()
        end

        ctx:win_inline()
        if ctx:win_button("Play this seed") then
            exports.find = false
            state.quest_flags = 0x1
            if exports.seeded then
                state.quest_flags = set_flag(state.quest_flags, 7)
            end
            set_adventure_seed(exports.a, exports.b)
            state.world_next = state.world_start
            state.level_next = state.level_start
            state.theme_next = state.theme_start
            state.loading = 1
        end

        ctx:win_separator()
        ctx:win_text("Route (defaults to first exit/ending)")
        exports.volcana = ctx:win_check("Volcana", exports.volcana)
        exports.temple = ctx:win_check("Temple", exports.temple)
        exports.cog = ctx:win_check("City of Gold", exports.cog)
        exports.abzuduat = ctx:win_check("Abzu / Duat", exports.abzuduat)
        exports.sunken = ctx:win_check("Sunken City", exports.sunken)
        exports.eggplant = ctx:win_check("Eggplant World", exports.eggplant)
        exports.cosmic = ctx:win_check("Cosmic Ocean", exports.cosmic)

        ctx:win_separator()
        ctx:win_text("Deepest level (resets if thing is not found here)")
        exports.deepest_world = ctx:win_input_int("World", exports.deepest_world)
        exports.deepest_level = ctx:win_input_int("Level", exports.deepest_level)

        ctx:win_separator()
        ctx:win_text(exports.msg)
        ctx:win_text("")
    end)

    if exports.uid > -1 then
        local ent = get_entity(exports.uid)
        if ent then
            exports.x, exports.y, _ = get_render_position(ent.uid)
        end
    end

    if not exports.find and exports.x ~= 0 and exports.y ~= 0 and exports.world == state.world and exports.level == state.level and exports.theme == state.theme then
        local sx, sy = screen_position(exports.x, exports.y)
        local rad = screen_distance(0.66)
        ctx:draw_circle(sx, sy, rad, 4, 0xFFFFFFFF)
        if #players > 0 then
            local px, py = get_render_position(players[1].uid)
            local spx, spy = screen_position(px, py)
            ctx:draw_line(spx, spy, sx, sy, 4, 0xFFFFFFFF)
        end
    end
end, ON.GUIFRAME)
