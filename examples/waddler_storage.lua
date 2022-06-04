meta.name = 'Waddler Storage'
meta.description = 'Save waddler items on death and load in the next run.'
meta.author = 'Dregu'
meta.version = '1.0'

function table.clone(org)
    return {table.unpack(org)}
end

-- save in the death screen
set_callback(function()
    storage = table.clone(state.waddler_storage)
    storage_meta = table.clone(state.waddler_metadata)
    prinspect("saved", storage)
end, ON.DEATH) -- or RESET if you want to save on quick restart too

-- load when loading the first level
set_callback(function()
    if test_flag(state.quest_flags, 1) and storage and state.screen == SCREEN.LEVEL then
        state.waddler_storage = table.clone(storage)
        state.waddler_metadata = table.clone(storage_meta)
        storage = nil
        storage_meta = nil
        prinspect("loaded", table.clone(state.waddler_storage))
    end
end, ON.PRE_LEVEL_GENERATION)
