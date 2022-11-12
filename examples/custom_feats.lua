meta.name = 'Custom Feats'
meta.version = 'WIP'
meta.description = 'Replace a few Feats with our own and create hooks for them.'
meta.author = 'Dregu'

-- Keep custom feats in a table, unlock some by default for some reason
-- We could just leave this empty though, cause we default to false later
feats = {
    [FEAT.THE_FULL_SPELUNKY] = false,
    [2] = false,
    [32] = true
}

-- Unlock a feat in our table and toast for it
function perform(feat)
    done, hidden, name, desc = get_feat(feat)
    if feats[feat] == false then
        toast(F"{name}\n{desc}")
    end
    feats[feat] = true
    prinspect("Unlocked custom feat", feat, name)
end

-- Change "Parenthood" and make it visible
change_feat(FEAT.PARENTHOOD, false, "I am the eggman", "This is impossible to get, but you already have it")

-- Change the first feat and make it hidden
change_feat(1, true, "I am the walrus", "Play as Guy Spelunky from the hit game Spelunky")
set_callback(function()
    if players[1].type.id == ENT_TYPE.CHAR_GUY_SPELUNKY then perform(1) end
end, ON.LEVEL)

-- One more
change_feat(2, false, "Borscht", "Die to lava")
set_callback(function(id)
    if id == hash_to_stringid(0x9c821452) then
        perform(2)
        -- Reveal the first feat only after getting this one
        set_feat_hidden(1, false)
    end -- MELTED
end, ON.DEATH_MESSAGE)

-- Return feat status from our table when asked, defaulting to false if not found
set_callback(function(feat)
    return feats[feat] or false

    -- this would default to vanilla behaviour for feats missing from the table,
    -- if you only want to override some of them
    --return feats[feat]
end, ON.PRE_GET_FEAT)

-- Block vanilla feats, although they wouldn't show up on the Feats page,
-- they would still trigger Steam achievements if not blocked otherwise
set_callback(function(feat)
    prinspect("Tried to unlock a vanilla feat", enum_get_name(FEAT, feat))
    return true

    -- this would default to vanilla behaviour for feats missing from the table,
    -- if you only want to override some of them
    --if feats[feat] ~= nil then return true end
end, ON.PRE_SET_FEAT)

-- Clear the feats our mod doesn't use.
for i=3,31 do
    change_feat(i, false, "", "")
end
