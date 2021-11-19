meta.name = "TAS Tool"
meta.version = "WIP"
meta.description = "STILL BUGGY AND RERECORD DOESN'T WORK PERFECTLY! This is The Tool for Tool Assisted Speedrunning Spelunky 2, to be used in seeded runs with Overlunky frame advance features. In a TAS, you record your inputs frame by frame, with the ability to rerecord your steps if you screw up. The resulting inputs are then played back in real time to recreate the perfect inhuman run."
meta.author = "Dregu"

local seed_text = ""
local seed = 0
local frames = {}
local pos = {}
local levels = {}
local stopped = true
local stolen = false
local cutcb = -1
local rerecord_level = -1
local rerecord_frame = 0
local rerecording = false
local window_open = true
local mode = 1
local pause = true
local skip = true
local turbo = true
local draw = true
local delay = 1

register_option_button("open", "Show TAS window", function()
    window_open = true
end)

local names = {}
for i,v in pairs(ENT_TYPE) do
  names[v] = i
end

local function clear_run()
    frames = {}
    pos = {}
    levels = {}
    rerecord_level = -1
    rerecord_frame = 0
    rerecording = false
end

set_callback(function(ctx)
    if window_open then
        window_open = ctx:window("TAS", 0, 0, 0, 0, true, function()

            ctx:win_text("Seed")
            seed_text = ctx:win_input_text("##Seed", seed_text)
            ctx:win_inline()
            if ctx:win_button("Restart") then
                rerecord_level = -1
                rerecord_frame = 0
                rerecording = false
                if seed_text ~= "" then
                    local new_seed = tonumber(seed_text, 16)
                    if new_seed ~= seed then
                        clear_run()
                    end
                    seed = new_seed
                else
                    seed = math.random(0, 0xffffffff)
                    seed_text = string.format("%X", seed)
                    clear_run()
                end
                set_seed(seed)
            end
            ctx:win_inline()
            if ctx:win_button("Reset") then
                clear_run()
                set_seed(seed)
            end

            ctx:win_separator()
            ctx:win_text("Recording")
            rerecord_frame = ctx:win_slider_int("##Frame", rerecord_frame, 0, state.time_level)
            ctx:win_inline()
            if ctx:win_button("Rerecord level from frame") then
                mode = 1
                rerecord_level = state.level_count
                rerecording = true
                set_seed(seed)
            end

            ctx:win_separator()
            ctx:win_text("Playback")
            if ctx:win_button("Playback level") then
                mode = 2
                rerecord_level = state.level_count
                rerecording = false
                set_seed(seed)
            end
            ctx:win_inline()
            if ctx:win_button("Playback run") then
                mode = 2
                rerecord_level = -1
                rerecord_frame = 0
                rerecording = false
                set_seed(seed)
            end

            ctx:win_separator()
            ctx:win_text("Options")
            pause = ctx:win_check("Start levels paused", pause)
            skip = ctx:win_check("Skip level transitions", skip)
            turbo = ctx:win_check("Skip fades", turbo)
            draw = ctx:win_check("Draw history", draw)
        end)
    end
    if draw and pos[state.level_count] and #pos[state.level_count] > 3 then
        local cpos = pos[state.level_count]
        for i=2,#cpos do
            if (rerecord_frame > 0 and i >= rerecord_frame) or not cpos[i] or not cpos[i-1] then break end
            local x1, y1 = screen_position(cpos[i-1].x, cpos[i-1].y)
            local x2, y2 = screen_position(cpos[i].x, cpos[i].y)
            local col = cpos[i].l == state.camera_layer and 0xff0000aa or 0xff000066
            ctx:draw_line(x1, y1, x2, y2, 2, col)
        end
    end
end, ON.GUIFRAME)

set_callback(function()
    if not rerecording then
        rerecord_frame = 0
    end
end, ON.PRE_LEVEL_GENERATION)

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
            local backitem = worn_backitem(players[1].uid)
            if backitem ~= -1 then
                backitem = get_entity(backitem).type.id
            end
            levels[state.level_count] = { state.world, state.level, state.theme, players[1].health, players[1].inventory.bombs, players[1].inventory.ropes, players[1]:get_powerups(), holding, state.time_total, state.shoppie_aggro, state.shoppie_aggro_next, state.merchant_aggro, backitem }
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
                local m = string.find(names[v], "PACK")
                if not m then
                    players[1]:give_powerup(v)
                end
            end
            if next[8] ~= -1 then
                pick_up(players[1].uid, spawn(next[8], 0, 0, LAYER.PLAYER, 0, 0))
            end
            if next[13] ~= -1 then
                pick_up(players[1].uid, spawn(next[13], 0, 0, LAYER.PLAYER, 0, 0))
            end
        end
        if mode == 1 and pause and state.time_level >= rerecord_frame then -- record
            if state.pause == 0 then
                state.pause = 0x20
            end
        elseif mode == 2 or state.time_level < rerecord_frame and state.time_level < #frames[state.level_count] then -- playback
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
        pos[state.level_count] = {}
    end
    if (mode == 2 and not stopped) or (mode == 1 and state.time_level < rerecord_frame) then -- playback
        local input = frames[state.level_count][state.time_level+delay]
        if input and stolen then
            message('Sending '..string.format('%04x', input)..' '..state.time_level..'/'..#frames[state.level_count])
            send_input(players[1].uid, input)
        elseif stolen and state.time_level >= #frames[state.level_count] and players[1].state ~= CHAR_STATE.ENTERING and state.loading == 0 then
            message('Playback ended, recording')
            return_input(players[1].uid)
            stolen = false
            stopped = true
            if pause then
                state.pause = 0x20
            end
            mode = 1
        end
    end
    if mode == 1 and state.time_level == rerecord_frame then
        if pause then
            state.pause = 0x20
        end
        if stolen then
            message('Rerecording from frame')
            return_input(players[1].uid)
            stolen = false
            stopped = true
        end
        rerecording = false
    end

    if mode == 1 and state.time_level >= rerecord_frame then -- record
        frames[state.level_count][state.time_level] = read_input(players[1].uid)
        message('Recording '..string.format('%04x', frames[state.level_count][state.time_level])..' '..state.time_level..'/'..#frames[state.level_count])
        local x, y, l = get_position(players[1].uid)
        pos[state.level_count][state.time_level] = {x=x, y=y, l=l}
    end
end, ON.FRAME)

set_callback(function()
    if skip then
        warp(state.world_next, state.level_next, state.theme_next)
    end
end, ON.TRANSITION)

set_global_interval(function()
    if state.logic.olmec_cutscene ~= nil then
        state.logic.olmec_cutscene.timer = 809
        if pause and mode == 1 then
            cutcb = set_callback(function()
                if state.pause == 0 then
                    clear_callback(cutcb)
                    state.pause = 0x20
                end
            end, ON.GUIFRAME)
        end
    end
    if state.logic.tiamat_cutscene ~= nil then
        state.logic.tiamat_cutscene.timer = 379
        if pause and mode == 1 then
            cutcb = set_callback(function()
                if state.pause == 0 then
                    clear_callback(cutcb)
                    state.pause = 0x20
                end
            end, ON.GUIFRAME)
        end
    end
end, 1)

set_callback(function()
    if turbo then
        state.fadeout = 0
        state.fadein = 0
    end
end, ON.LOADING)

-- 43D3D0DD
