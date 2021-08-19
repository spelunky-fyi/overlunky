local module = {}
door_cbs = {}

theme = {1,2,3,5,6,7,8,9,10,11}
bosses = {THEME.OLMEC, THEME.ABZU, THEME.DUAT, THEME.TIAMAT, THEME.HUNDUN}
bosses_left = {}
world = {1,2,2,3,4,4,5,6,7,8,4,4,4,6,7,7,1}
normal_levels = 0
nextworld = 1
nextlevel = 1
nexttheme = 1
reallevel = 1
realtheme = 1
boss_level = false
dead = false
critters_spawned = true

critters = {}
critters[THEME.DWELLING] = ENT_TYPE.MONS_CRITTERDUNGBEETLE
critters[THEME.JUNGLE] = ENT_TYPE.MONS_CRITTERBUTTERFLY
critters[THEME.VOLCANA] = ENT_TYPE.MONS_CRITTERSNAIL
critters[THEME.TIDE_POOL] = ENT_TYPE.MONS_CRITTERCRAB
critters[THEME.TEMPLE] = ENT_TYPE.MONS_CRITTERLOCUST
critters[THEME.ICE_CAVES] = ENT_TYPE.MONS_CRITTERPENGUIN
critters[THEME.NEO_BABYLON] = ENT_TYPE.MONS_CRITTERDRONE
critters[THEME.SUNKEN_CITY] = ENT_TYPE.MONS_CRITTERSLIME
critters[THEME.CITY_OF_GOLD] = ENT_TYPE.MONS_SCARAB
for i,v in ipairs(bosses) do
    critters[v] = ENT_TYPE.ITEM_LITWALLTORCH
end

function set_doors()
    if #players > 0 then
        px, py, pl = get_position(players[1].uid)
        doors = get_entities_at(ENT_TYPE.FLOOR_DOOR_EXIT, 0, px, py, pl, 15)
        for i,v in ipairs(doors) do
            x, y, layer = get_position(v)
            if state.theme == THEME.TIAMAT or state.theme == THEME.HUNDUN then
                --set_door_target(v, nextworld, nextlevel, nexttheme) --crashes?
                unlock_door_at(x, y)
            end
            if not critters_spawned and critters[realtheme] ~= nil then
                spawn(critters[realtheme], x-0.7, y+0.5, layer, 0, 0)
                spawn(critters[realtheme], x+0.7, y+0.5, layer, 0, 0)
                critters_spawned = true
            end
        end
    end
end

function random_level()
    -- rare eggplant world
    if math.random(120) == 1 then
        nexttheme = THEME.EGGPLANT_WORLD
        nextlevel = 2

    -- pick a boss level from the remaining
    elseif #bosses_left > #bosses-options.door_bosses and normal_levels >= options.door_min_levels and math.random(options.door_max_levels) <= normal_levels then
        boss_level = true
        normal_levels = 0
        nexttheme = bosses_left[math.random(#bosses_left)]
        nextlevel = 4
        if nexttheme == THEME.OLMEC then
            nextlevel = 1
        end

    -- all bosses killed, go to 7-98
    elseif #bosses_left <= #bosses-options.door_bosses and normal_levels >= options.door_min_levels and math.random(options.door_max_levels) <= normal_levels then
        boss_level = false
        nexttheme = THEME.COSMIC_OCEAN
        nextlevel = 98
        toast("Lets get out of here!")

    -- pick a regular level
    else
        boss_level = false
        normal_levels = normal_levels+1
        nexttheme = theme[math.random(#theme)]
        if nexttheme == THEME.NEO_BABYLON then
            nextlevel = math.random(3)
        elseif nexttheme == THEME.CITY_OF_GOLD then
            nextlevel = 3
        elseif nexttheme == THEME.COSMIC_OCEAN then
            nextlevel = math.random(5, 97)
        elseif nexttheme == THEME.ICE_CAVES then
            nextlevel = 1
        else
            nextlevel = math.random(4)
        end
    end
    nextworld = state.world
    reallevel = nextlevel
    nextlevel = 1
    if nexttheme == THEME.COSMIC_OCEAN then
        nextlevel = 5
    end
    realtheme = nexttheme
    if state.theme ~= THEME.OLMEC then
        nexttheme = state.theme
    else
        nexttheme = 1
    end
    --message("Going to "..tostring(nextworld).."-"..tostring(reallevel)..", theme "..tostring(realtheme))
end

function init_run()
    --message("Started new run")
    bosses_left = {table.unpack(bosses)}
    normal_levels = 0
    boss_level = false
    random_level()
end

function remove_boss(boss)
    for k,v in ipairs(bosses_left) do
        if v == boss then
            table.remove(bosses_left, k)
            toast("Boss defeated!\nBosses remaining: "..tostring(#bosses_left-(#bosses-options.door_bosses)))
            return
        end
    end
end

function dead_olmec()
    if state.theme == THEME.OLMEC then
        olmecs = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_OLMEC)
        if #olmecs > 0 then
            x, y, l = get_position(olmecs[1])
            if y < 71 then -- this olmec is low enough
                remove_boss(THEME.OLMEC)
            end
        end
    end
end

function dead_tiamat()
    if state.theme == THEME.TIAMAT then
        tiamats = get_entities_by_type(ENT_TYPE.MONS_TIAMAT)
        if #tiamats > 0 then
            tiamat = get_entity(tiamats[1])
            if testflag(tiamat.flags, 29) then -- this tiamat is dead
                remove_boss(THEME.TIAMAT)
            end
        end
    end
end

function dead_hundun()
    if state.theme == THEME.HUNDUN then
        hunduns = get_entities_by_type(ENT_TYPE.MONS_HUNDUN)
        if #hunduns > 0 then
            if get_entity(hunduns[1]).move_state == 4 then -- this hundun is just chillin on the floor
                remove_boss(THEME.HUNDUN)
            end
        end
    end
end

function dead_kingu()
    if testflag(state.journal_flags, 16) then
        remove_boss(THEME.ABZU)
    end
end

function dead_osiris()
    if testflag(state.journal_flags, 17) then
        remove_boss(THEME.DUAT)
    end
end

function duat_door()
    -- spawn duat skip door
    spawn_door(17, 106, 0, nextworld, nextlevel, nexttheme)
    spawn_entity(ENT_TYPE.BG_DOOR_BACK_LAYER, 17, 106, 0, 0, 0)
end

function module.start()
    register_option_int("door_min_levels", "Min normal levels before boss", 3, 1, 100)
    register_option_int("door_max_levels", "Max normal levels before boss", 6, 1, 100)
    register_option_int("door_bosses", "Bosses to defeat to trigger ending", 5, 1, 5)
    register_option_bool("door_xiit", "Spawn exit on entrance (for debugging)", false)

    door_cbs[#door_cbs+1] = set_callback(function()
        --message("Level")
        critters_spawned = false
        dead = false
        if state.level_count == 0 then
            init_run()
        end

        if state.theme == THEME.DUAT then
            set_timeout(duat_door, 60)
        end

        random_level()

        if options.door_xiit and #players > 0 then
            x, y, l = get_position(players[1].uid)
            spawn_door(x, y, l, nextworld, nextlevel, nexttheme)
        end

        set_interval(function()
            set_doors()
            dead_olmec()
            dead_tiamat()
            dead_hundun()
            dead_kingu()
            dead_osiris()
        end, 15)

        if state.theme == THEME.TIAMAT or state.theme == THEME.HUNDUN then
            doors = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
            for i,v in ipairs(doors) do
                x, y, l = get_position(v)
                move_entity(v, 70, y, 0, 0)
                spawn_door(x, y, l, state.world, state.level, state.theme)
            end
        end
    end, ON.LEVEL)

    door_cbs[#door_cbs+1] = set_callback(function()
        --message("Transition")
        toast("Level "..tostring(state.level_count).." completed!\nBosses remaining: "..tostring(#bosses_left-(#bosses-options.door_bosses)))
        if state.level < 98 then
            --message("Transition - Setting next level")
            state.theme_next = realtheme
            state.world_next = world[state.theme_next]
            state.level_next = reallevel
            state.quest_flags = 0
        end
    end, ON.TRANSITION)

    door_cbs[#door_cbs+1] = set_callback(function()
        --message("Loading")
        if dead == true and state.level < 98 then
            --message("Loading - Setting next level")
            state.theme_next = realtheme
            state.world_next = world[state.theme_next]
            state.level_next = reallevel
        end
    end, ON.LOADING)

    door_cbs[#door_cbs+1] = set_callback(function()
        --message("Reset - Init, state.reset == "..tostring(state.reset))
        init_run()
        dead = true
    end, ON.RESET)

    door_cbs[#door_cbs+1] = set_callback(function()
        --message("Camp - Init")
        init_run()
        dead = true
    end, ON.CAMP)

    door_cbs[#door_cbs+1] = set_callback(function()
        --message("Death - Init")
        toast("Died after "..tostring(state.level_count).." levels!\nBosses remaining: "..tostring(#bosses_left-(#bosses-options.door_bosses)))
        init_run()
        dead = true
    end, ON.DEATH)
end

function module.stop()
    for i,v in ipairs(door_cbs) do
        clear_callback(v)
    end
    door_cbs = {}
end

return module
