meta.name = 'Reverse game'
meta.version = 'WIP'
meta.description = 'A really weird and maybe impossible game where you start on top of Hundun and have to get back to 1-1. Hey, that rhymes!'
meta.author = 'Dregu'

register_option_bool('goodies', 'Get some goodies at start, climbing is hard.', false)
waittime = 15
iframes = 90

dest = {}
dest[74] = { 7, 3, THEME.SUNKEN_CITY }
dest[73] = { 7, 2, THEME.SUNKEN_CITY }
dest[72] = { 7, 1, THEME.SUNKEN_CITY }
dest[71] = { 6, 4, THEME.TIAMAT }
dest[64] = { 6, 3, THEME.NEO_BABYLON }
dest[63] = { 6, 2, THEME.NEO_BABYLON }
dest[62] = { 6, 1, THEME.NEO_BABYLON }
dest[61] = { 5, 1, THEME.ICE_CAVES }
dest[51] = { 4, 4, THEME.ABZU }
dest[44] = { 4, 3, THEME.TIDE_POOL }
dest[43] = { 4, 2, THEME.TIDE_POOL }
dest[42] = { 4, 1, THEME.TIDE_POOL }
dest[41] = { 3, 1, THEME.OLMEC }
dest[31] = { 2, 4, THEME.JUNGLE }
dest[24] = { 2, 3, THEME.JUNGLE }
dest[23] = { 2, 2, THEME.JUNGLE }
dest[22] = { 2, 1, THEME.JUNGLE }
dest[21] = { 1, 4, THEME.DWELLING }
dest[14] = { 1, 3, THEME.DWELLING }
dest[13] = { 1, 2, THEME.DWELLING }
dest[12] = { 1, 1, THEME.DWELLING }
dest[11] = { 1, 1, THEME.BASE_CAMP }

function init()
    nextworld = 7
    nextlevel = 4
    nexttheme = THEME.HUNDUN
    volcana = false
    temple = false
    compass_spawned = false
    udjat_spawned = false
    sx, sy, ex, ey = 0, 0, 0, 0
end
init()

function olmec_exit()
    x, y, l = get_position(players[1].uid)
    if y > 100 then
        nexttheme = THEME.VOLCANA
        volcana = true
    else
        nexttheme = THEME.JUNGLE
        volcana = false
    end
end

function ice_caves_exit()
    x, y, l = get_position(players[1].uid)
    if l == LAYER.BACK then
        nexttheme = THEME.DUAT
        temple = true
        bosses = get_entities_at(ENT_TYPE.MONS_ALIENQUEEN, 0, x, y, l, 20)
        portals = get_entities_at(ENT_TYPE.LOGICAL_PORTAL, 0, x, y, l, 20)
        if #bosses > 0 and test_flag(get_entity(bosses[1]).flags, 29) and #portals == 0 then
            ex, ey, el = get_position(bosses[1])
            spawn(ENT_TYPE.LOGICAL_PORTAL, ex, ey, el, 0, 0)
        end
    else
        nexttheme = THEME.ABZU
        temple = false
    end
    if not compass_spawned then
        entrances = get_entities_at(ENT_TYPE.FLOOR_DOOR_ENTRANCE, 0, x, y, l, 20)
        if #entrances > 0 then
            cx, cy, cl = get_position(entrances[1])
            spawn(ENT_TYPE.ITEM_PICKUP_SPECIALCOMPASS, cx+1, cy, cl, 0, 0)
            compass_spawned = true
        end
    end
end

function unlock_entrance()
    if #players < 1 then return end
    x, y, l = get_position(players[1].uid)
    entrances = get_entities_at(ENT_TYPE.FLOOR_DOOR_ENTRANCE, 0, x, y, l, 20)
    for i,v in ipairs(entrances) do
        vx, vy, vl = get_position(v)
        bgs = get_entities_at(ENT_TYPE.BG_DOOR, 0, vx, vy, vl, 2)
        if #bgs > 0 then
            ent = get_entity(bgs[1])
            ent.animation_frame = set_flag(ent.animation_frame, 1)
        end
    end
    shipdoors = get_entities_at(ENT_TYPE.FX_EGGSHIP_DOOR, 0, x, y, l, 20)
    if #shipdoors > 0 then
        kill_entity(shipdoors[1])
    end
end

function give_udjat()
    x, y, l = get_position(players[1].uid)
    sockets = get_entities_at(ENT_TYPE.ITEM_UDJAT_SOCKET, 0, x, y, l, 20)
    entrances = get_entities_at(ENT_TYPE.FLOOR_DOOR_ENTRANCE, 0, x, y, l, 20)
    if not udjat_spawned then
        if #sockets > 0 then
            cx, cy, cl = get_position(sockets[1])
            spawn(ENT_TYPE.ITEM_PICKUP_UDJATEYE, cx, cy, cl, 0, 0)
            udjat_spawned = true
        end
    end
end

function teleport_mount(ent, x, y)
    if ent.overlay ~= nil then
        move_entity(ent.overlay.uid, x, y, 0, 0)
    else
        move_entity(ent.uid, x, y, 0, 0)
    end
    set_timeout(function()
        ent.more_flags = clr_flag(ent.more_flags, 16)
    end, 15)
    if state.theme == THEME.DUAT then -- we're spawning in the air, so
        set_timeout(function()
            x, y, l = get_position(players[1].uid)
            spawn(ENT_TYPE.ITEM_WEB, x, y, l, 0, 0)
        end, 1)
    elseif state.theme == THEME.TIAMAT then -- forbid going to backlayer here
        set_timeout(function()
            x, y, l = get_position(players[1].uid)
            layerdoors = get_entities_at(ENT_TYPE.FLOOR_DOOR_LAYER, 0, x, y, l, 20)
            if #layerdoors > 0 then
                kill_entity(layerdoors[1])
            end
        end, 1)
    elseif state.theme == THEME.TIDE_POOL and state.level == 3 then -- we're spawning in lava D:
        set_timeout(function()
            x, y, l = get_position(players[1].uid)
            spawn(ENT_TYPE.FX_POWEREDEXPLOSION, x, y-4, l, 0, 0)
        end, 1)
    end
end

set_callback(function()
    if state.level == 98 then return end

    iframes = 90
    if state.theme == THEME.TIDE_POOL and state.level == 3 then
        iframes = 240
    end

    players[1].flags = set_flag(players[1].flags, 6)
    players[1].more_flags = set_flag(players[1].more_flags, 16)
    set_timeout(function()
        players[1].flags = clr_flag(players[1].flags, 6)
    end, waittime+iframes)

    set_timeout(function()
        x, y, l = get_position(players[1].uid)

        exits = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
        if #exits > 0 then
            ex, ey, el = get_position(exits[1])
            if #exits > 1 and (temple == true and state.theme == THEME.OLMEC) or (volcana == true and state.theme == THEME.DWELLING and state.level == 4) or (state.theme == THEME.TIDE_POOL and state.level == 3) then
                ex, ey, el = get_position(exits[2])
            end
        elseif state.theme == THEME.DUAT then
            spawn(ENT_TYPE.FLOOR_DOOR_ENTRANCE, x, y, l, 0, 0)
            ex = 18
            ey = 120
        end

        if state.theme == THEME.TIAMAT then
            ldoors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_LAYER)
            if #ldoors > 0 then
                ex, ey, el = get_position(ldoors[1])
            end
        end

        set_timeout(function()
            to = 10*state.world+state.level
            nextworld = dest[to][1]
            nextlevel = dest[to][2]
            nexttheme = dest[to][3]
            if volcana == true and nexttheme == THEME.JUNGLE then
                nexttheme = THEME.VOLCANA
            end
            if temple == true and (nexttheme == THEME.ABZU or nexttheme == THEME.TIDE_POOL) then
                if nextlevel == 4 then
                    nexttheme = THEME.DUAT
                elseif nextlevel == 3 then
                    nexttheme = THEME.CITY_OF_GOLD
                else
                    nexttheme = THEME.TEMPLE
                end
            end
            entrances = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_ENTRANCE)
            for i,v in ipairs(entrances) do
                sx, sy, sl = get_position(v)
                door(sx, sy, sl, nextworld, nextlevel, nexttheme)
                if state.theme == THEME.DUAT then
                    bg = spawn(ENT_TYPE.BG_DOOR, sx, sy, sl, 0, 0)
                    ent = get_entity(bg)
                    bg.animation_frame = set_flag(bg.animation_frame, 1)
                end
            end

            if state.theme == THEME.HUNDUN then
                elev = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR)
                for i,v in ipairs(elev) do
                    kill_entity(v)
                end
            end
        end, 1)

        set_timeout(function()
            for i,player in ipairs(players) do
                teleport_mount(player, ex, ey)
            end
        end, 2)

        set_timeout(function()
            x, y, l = get_position(players[1].uid)
            exits = get_entities_at(ENT_TYPE.FLOOR_DOOR_EXIT, 0, x, y, l, 20)
            for i,v in ipairs(exits) do
                x, y, l = get_position(v)
                move_entity(v, x, y+1, 0, 0)
                lock_door_at(x, y)
            end
        end, 3)

        if state.theme == THEME.OLMEC then
            set_interval(olmec_exit, 30)
        elseif state.theme == THEME.ICE_CAVES then
            set_interval(ice_caves_exit, 30)
        elseif state.theme == THEME.VOLCANA then
            set_interval(give_udjat, 30)
        end
        set_interval(unlock_entrance, 30)

    end, waittime)

end, ON.LEVEL)

set_callback(function()
    if state.level < 98 then
        state.theme_next = nexttheme
        state.world_next = nextworld
        state.level_next = nextlevel
    end
    if state.theme_next == 17 then
        state.screen_next = 11
    end
end, ON.LOADING)

set_callback(function()
    if options.goodies then
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, 0, 0, LAYER.PLAYER1, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, 0, 0, LAYER.PLAYER1, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, 0, 0, LAYER.PLAYER1, 0, 0)
    end
end, ON.START)

set_callback(function()
    if state.screen ~= 12 then return end
    if state.level == 98 then return end
    if state.time_level < waittime+15 then
        draw_rect_filled(-1, 1, 1, -1, 0, rgba(0, 0, 0, 250))
    elseif state.time_level <= waittime+25 then
        alpha = 250 - math.floor((state.time_level - (waittime+15)) / 10 * 250)
        draw_rect_filled(-1, 1, 1, -1, 0, rgba(0, 0, 0, alpha))
    end
end, ON.GUIFRAME)

set_callback(init, ON.CAMP)
set_callback(init, ON.RESET)
