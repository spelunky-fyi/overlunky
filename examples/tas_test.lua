meta.name = "TAS test"
meta.version = "WIP"
meta.description = "Simple test for TASing a seeded run. It does desync, I don't think the game is fully deterministic or there's something wrong."
meta.author = "Dregu"

register_option_combo('mode', 'Mode', 'Record\0Playback\0\0')
register_option_bool('pause', 'Start levels paused (when recording)', true)
register_option_bool('pskip', 'Skip level transitions automatically', true)
-- this probably needs a way to save and load the prng state to work
--[[register_option_button('rslevel', 'Restart level', function()
    warp(state.world, state.level, state.theme)
end)]]

local frames = {}
local stopped = true
local stolen = false

set_seed(math.random(0, 0xffffffff))

set_callback(function()
    if options.mode == 1 and options.pause then -- record
        state.pause = 0x20
    elseif options.mode == 2 then -- playback
        steal_input(players[1].uid)
        stopped = false
        stolen = true
    end
end, ON.LEVEL)

set_callback(function()
    if #players < 1 then return end
    if frames[state.level_count] == nil then
        frames[state.level_count] = {}
    end
    if options.mode == 1 then -- record
        frames[state.level_count][state.time_level-1] = read_input(players[1].uid)
        message('Recording '..string.format('%04x', frames[state.level_count][state.time_level-1])..' '..#frames[state.level_count])
    elseif options.mode == 2 and not stopped then -- playback
        local input = frames[state.level_count][state.time_level]
        if input and stolen then
            message('Sending '..string.format('%04x', input)..' '..state.time_level..'/'..#frames[state.level_count])
            send_input(players[1].uid, input)
        elseif state.time_level > #frames[state.level_count] then
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

set_callback(function()
    if options.mode == 1 then
        frames = {}
    end
end, ON.RESET)
