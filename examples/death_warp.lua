meta.name = 'Death warp'
meta.version = 'WIP'
meta.description = 'Automatic instant restart on death.'
meta.author = 'Dregu'

-- actually instant restart on death
set_callback(function()
    if state.screen ~= 12 then
        return
    end
    local hp = 0
    for i, player in ipairs(players) do
        hp = hp + player.health
    end
    if hp == 0 then
        state.quest_flags = set_flag(state.quest_flags, 1)
        warp(state.world_start, state.level_start, state.theme_start)
    end
end, ON.FRAME)
