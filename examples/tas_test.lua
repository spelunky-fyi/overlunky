meta.name = "TAS test"
meta.version = "WIP"
meta.description = "Simple test for TASing a seeded run. Works from start to finish, although you might have to wait a frame or two at the start of boss levels because of cutscene skip weirdness. The rerecording stuff is very WIP."
meta.author = "Dregu"

local seed = 0
local frames = {}
local levels = {}
local stopped = true
local stolen = false
local cutcb = -1
local rerecord_level = -1

register_option_combo('mode', 'Mode', 'Record\0Playback\0\0')
register_option_bool('pause', 'Start levels paused (when recording)', true)
register_option_bool('pskip', 'Skip level transitions automatically', true)
register_option_string('seed', 'Seed (empty=random)', '')
register_option_int('delay', "Send delay", 1, -2, 2)
register_option_bool('turbo', 'Turbo mode', true)
register_option_button('zrlevel', 'Rerecord current level', function()
    rerecord_level = state.level_count
    if options.seed ~= '' then
        seed = tonumber(options.seed, 16)
    else
        seed = state.seed
    end
    set_seed(seed)
end)
register_option_button('zrestart', 'Restart run', function()
    rerecord_level = -1
    if options.seed ~= '' then
        seed = tonumber(options.seed, 16)
    else
        seed = math.random(0, 0xffffffff)
    end
    set_seed(seed)
end)

set_callback(function()
    if rerecord_level ~= -1 and state.level_count ~= rerecord_level then
        prinspect("Skipping level")
        state.level_count = state.level_count + 1
        local next = levels[state.level_count]
        state.screen_next = SCREEN.LEVEL
        state.world_next = next[1]
        state.level_next = next[2]
        state.theme_next = next[3]
        state.time_total = next[9]
        state.shoppie_aggro = next[10]+1
        state.shoppie_aggro_next = next[11]+1
        state.merchant_aggro = next[12]+1
        state.loading = 1
    else
        if rerecord_level == -1 then
            prinspect("Saving stats")
            local holding = players[1].holding_uid
            if holding ~= -1 then
                holding = get_entity(holding).type.id
            end
            levels[state.level_count] = { state.world, state.level, state.theme, players[1].health, players[1].inventory.bombs, players[1].inventory.ropes, players[1]:get_powerups(), holding, state.time_total, state.shoppie_aggro, state.shoppie_aggro_next, state.merchant_aggro }
        elseif state.level_count >= rerecord_level then
            prinspect("Rerecord level reached")
            rerecord_level = -1
            local next = levels[state.level_count]
            state.time_total = next[9]
            players[1].health = next[4]
            players[1].inventory.bombs = next[5]
            players[1].inventory.ropes = next[6]
            players[1].health = next[4]
            players[1].inventory.bombs = next[5]
            players[1].inventory.ropes = next[6]
            for i,v in ipairs(next[7]) do
                players[1]:give_powerup(v)
            end
            if levels[state.level_count][8] ~= -1 then
                pick_up(players[1].uid, spawn(next[8], 0, 0, LAYER.PLAYER, 0, 0))
            end
        end
        if options.mode == 1 and options.pause then -- record
            if state.pause == 0 then
                state.pause = 0x20
            else
                cutcb = set_callback(function()
                    if state.pause == 0 then
                        clear_callback(cutcb)
                        state.pause = 0x20
                    end
                end, ON.GUIFRAME)
            end
        elseif options.mode == 2 then -- playback
            steal_input(players[1].uid)
            stopped = false
            stolen = true
        end
    end
end, ON.LEVEL)

set_callback(function()
    if #players < 1 then return end
    if frames[state.level_count] == nil then
        frames[state.level_count] = {}
    end
    if options.mode == 1 then -- record
        frames[state.level_count][state.time_level] = read_input(players[1].uid)
        message('Recording '..string.format('%04x', frames[state.level_count][state.time_level])..' '..#frames[state.level_count])
    elseif options.mode == 2 and not stopped then -- playback
        local input = frames[state.level_count][state.time_level+options.delay]
        if input and stolen then
            message('Sending '..string.format('%04x', input)..' '..state.time_level..'/'..#frames[state.level_count])
            send_input(players[1].uid, input)
        elseif stolen and state.time_level > #frames[state.level_count] then
            message('Stopped')
            return_input(players[1].uid)
            stolen = false
            stopped = true
        end
    end
end, ON.FRAME)

set_callback(function()
    if options.pskip then -- auto skip transitions
        warp(state.world_next, state.level_next, state.theme_next)
    end
end, ON.TRANSITION)

--[[set_callback(function()
    if options.mode == 1 then
        frames = {}
    end
end, ON.RESET)]]

set_global_interval(function()
    if state.logic.olmec_cutscene ~= nil then
        state.logic.olmec_cutscene.timer = 809
    end
    if state.logic.tiamat_cutscene ~= nil then
        state.logic.tiamat_cutscene.timer = 379
    end
end, 1)

-- 43D3D0DD

set_callback(function()
    if options.turbo then
        state.fadeout = 0
        state.fadein = 0
    end
end, ON.LOADING)
