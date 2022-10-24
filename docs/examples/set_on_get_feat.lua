-- Keep custom feats in a table, unlock some by default for some reason
feats = {
    [FEAT.THE_FULL_SPELUNKY] = false,
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
    if id == hash_to_stringid(0x9c821452) then perform(2) end -- MELTED
end, ON.DEATH_MESSAGE)

-- Return feats from our table when asked, default to false
set_on_get_feat(function(feat)
    return feats[feat] or false
end)

-- Block vanilla feats, although they wouldn't show up on the Feats page,
-- they would still trigger Steam achievements if not blocked otherwise
set_on_set_feat(function(feat)
    prinspect("Tried to unlock a vanilla feat", enum_get_name(FEAT, feat))
end)
