local types = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT}
set_callback(function()
    local uids = get_entities_by_type(ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_BAT)
    -- is not the same thing as this, but also works
    local uids2 = get_entities_by_type(types)
    print(tostring(#uids).." == "..tostring(#uids2))
end, ON.LEVEL)
