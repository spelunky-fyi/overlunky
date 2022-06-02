meta.name = 'Adventure Seeded'
meta.version = '1.0'
meta.description = [[Reset the adventure, using the same seed that was used to start it, replicating it perfectly.

The example seed should have a teleporter in 1-1 right in front of you in the crust.]]
meta.author = 'Dregu'

-- The seed pair is initialized on game start, and the first value is added to the second value on every level or transtion
-- In adventure mode it is initialized using rand(), in seeded mode obviously the 32bit seed entered

function init()
    set_adventure_seed(0x271B63BDC8C39DB9, 0x3C253BCCD6072A0F)
    a, b = get_adventure_seed()
    ainput = string.format("%X", a)
    binput = string.format("%X", b)
end
set_callback(init, ON.CAMP)
init()

set_callback(function(ctx)
    ctx:window("Adventure Seed", -0.2, 1, 0, 0, false, function()
        ainput = ctx:win_input_text("First", ainput)
        a = tonumber(ainput, 16)
        binput = ctx:win_input_text("Second", binput)
        b = tonumber(binput, 16)
        ctx:win_inline()
        if (ctx:win_button("Restart with seed")) then
            set_adventure_seed(a, b)
            state.quest_flags = 1
            state.world_next = state.world_start
            state.level_next = state.level_start
            state.theme_next = state.theme_start
            state.screen_next = SCREEN.LEVEL
            state.loading = 1
        end
    end)
end, ON.GUIFRAME)

-- Save the initial seed pair on reset so we can recall it later to restart the same adventure
set_callback(function()
    if state.loading == 2 and (test_flag(state.quest_flags, 1) or state.screen < SCREEN.LEVEL) then
        a, b = get_adventure_seed()
        ainput = string.format("%X", a)
        binput = string.format("%X", b)
    end
end, ON.LOADING)
