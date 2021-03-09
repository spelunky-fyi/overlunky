meta.name = 'Reverse game'
meta.version = 'WIP'
meta.description = 'A really weird and maybe impossible game where you start on top of Hundun and have to get back to 1-1 to become one with the Cosmos. Hey, that rhymes!'
meta.author = 'Dregu'

register_option_bool('goodies', 'Get some goodies at start, climbing is hard.', false)

dest = {}
dest[74] = { 7, 3, THEME.SUNKEN_CITY }
dest[73] = { 7, 2, THEME.SUNKEN_CITY }
dest[72] = { 7, 1, THEME.SUNKEN_CITY }
dest[71] = { 6, 4, THEME.TIAMAT }
dest[64] = { 6, 3, THEME.NEO_BABYLON }
dest[63] = { 6, 2, THEME.NEO_BABYLON }
dest[62] = { 6, 1, THEME.NEO_BABYLON }
dest[61] = { 5, 1, THEME.ICE_CAVES }
dest[51] = { 4, 4, THEME.TIDE_POOL }
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
dest[11] = { 7, 98, THEME.COSMIC_OCEAN }

nextworld = 7
nextlevel = 4
nexttheme = THEME.HUNDUN
volcana = false

function init()
    nextworld = 7
    nextlevel = 4
    nexttheme = THEME.HUNDUN
    volcana = false
end

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

function give_udjat()
    sockets = get_entities_by_type(ENT_TYPE.ITEM_UDJAT_SOCKET)
    if #sockets > 0 then
        x, y, l = get_position(sockets[1])
        spawn(ENT_TYPE.ITEM_PICKUP_UDJATEYE, x, y, l, 0, 0)
    end
end

function teleport_mount(ent, x, y)
    if ent.overlay ~= nil then
        move_entity(ent.overlay.uid, x, y, 0, 0)
    else
        move_entity(ent.uid, x, y, 0, 0)
    end
end

set_callback(function()
    if state.level == 98 then return end

    timeout = 1
    if state.theme == THEME.ICE_CAVES then -- stupid ice caves crashes sometimes when you try fiddling with stuff immediately
        timeout = 15
    end

    set_timeout(function()
        exits = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
        entrances = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_ENTRANCE)
        layerdoors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_LAYER)
        x, y, l = get_position(exits[1])
        if state.theme == THEME.TIAMAT then
            x, y, l = get_position(layerdoors[1])
        end
        for i,player in ipairs(players) do
            teleport_mount(player, x, y)
        end
        for i,v in ipairs(exits) do
            x, y, l = get_position(v)
            move_entity(v, x+100, y, 0, 0)
            lock_door_at(x, y)
        end
        to = 10*state.world+state.level
        nextworld = dest[to][1]
        nextlevel = dest[to][2]
        nexttheme = dest[to][3]
        if volcana == true and nexttheme == THEME.JUNGLE then
            nexttheme = THEME.VOLCANA
        end
        for i,v in ipairs(entrances) do
            x, y, l = get_position(v)
            door(x, y, l, nextworld, nextlevel, nexttheme)
            unlock_door_at(x, y)
        end
    end, timeout)

    if state.theme == THEME.HUNDUN then
        elev = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_CRUSHING_ELEVATOR)
        for i,v in ipairs(elev) do
            kill_entity(v)
        end
    end

    if state.theme == THEME.OLMEC then
        set_interval(olmec_exit, 15)
    end

    if state.theme == THEME.VOLCANA then
        set_timeout(give_udjat, 15)
    end

    players[1].flags = set_flag(players[1].flags, 6)
    set_timeout(function()
        players[1].flags = clr_flag(players[1].flags, 6)
    end, 45)

end, ON.LEVEL)

set_callback(function()
    if state.level < 98 then
        state.theme_next = nexttheme
        state.world_next = nextworld
        state.level_next = nextlevel
    end
end, ON.LOADING)

set_callback(function()
    if options.goodies then
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, 0, 0, LAYER.PLAYER1, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, 0, 0, LAYER.PLAYER1, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, 0, 0, LAYER.PLAYER1, 0, 0)
    end
end, ON.START)

set_callback(init, ON.CAMP)
set_callback(init, ON.RESET)
init()
