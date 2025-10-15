function remove_from_shop(ent_uid)
    local ent = get_entity(ent_uid)
    if ent then
        -- technically this function is all you need
        ent:liberate_from_shop(true)
    end

    -- Be aware that removing all the items with the method below will grant you the "Big Spender" quest
    -- removing items from the owned_items is not required
    -- game only does it when you buy from the store and not when items are destroyed or shop disabled by aggro
    state.room_owners.owned_items:erase(ent_uid)
end
