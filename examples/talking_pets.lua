set_callback(function()
    pet_interval = set_interval(function()
        if #players == 0 then return end
        x, y, l = get_position(players[1].uid)
        cats = get_entities_at(ENT_TYPE.MONS_PET_CAT, 0, x, y, l, 2)
        if #cats > 0 then
            say(cats[1], "I hate Mondays.", 0, true)
            clear_callback(pet_interval)
        end
        dogs = get_entities_at(ENT_TYPE.MONS_PET_DOG, 0, x, y, l, 2)
        if #dogs > 0 then
            say(dogs[1], "Living in the caves is ruff.", 0, true)
            clear_callback(pet_interval)
        end
        hamsters = get_entities_at(ENT_TYPE.MONS_PET_HAMSTER, 0, x, y, l, 2)
        if #hamsters > 0 then
            say(hamsters[1], "Do you ever feel like you're just living the same day over and over again?", 0, true)
            clear_callback(pet_interval)
        end
    end, 60)
end, ON.LEVEL)
