--Make it so there is 50% chance that the Ankh will be destroyed

set_callback(function ()
    -- more or less 50% chance
    if prng:random(2) == 1 then
        -- get all Ankh's in a level
        ankhs = get_entities_by(ENT_TYPE.ITEM_PICKUP_ANKH, MASK.ITEM, LAYER.BOTH)
        for _, uid in pairs(ankhs) do
            get_entity(uid):destroy()
        end
    end
end, ON.LEVEL)
