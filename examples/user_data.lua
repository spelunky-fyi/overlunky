meta.name = "User data"
meta.description = "Extra lives user_data example"
meta.version = "1.0"
meta.author = "Dregu"

set_callback(function()
    players[1].user_data = {lives = 2}
    print(F"You have {players[1].user_data.lives} extra lives!")
end, ON.START)

set_callback(function()
    if state.screen_next == SCREEN.DEATH and players[1] and players[1].user_data.lives and players[1].user_data.lives > 0 then
        players[1].flags = clr_flag(players[1].flags, ENT_FLAG.DEAD)
        players[1].health = 4
        players[1]:set_behavior(1)
        players[1].user_data.lives = players[1].user_data.lives - 1
        print(F"You have {players[1].user_data.lives} lives left!")
        return true
    end
end, ON.PRE_LOAD_SCREEN)


-- another example that applies user data on first update after level gen or in post_load_screen for transitions
function apply_userdata(ent)
    if ent.user_data and ent.user_data.color then
        -- apply the color to the entity
        ent.color = ent.user_data.color
        prinspect(ent.uid, "applied user_data")
    end
end
set_post_entity_spawn(function(ent)
    -- saved user_data from the previous level is not available yet in post_entity_spawn, it is loaded right before post_level_gen
    -- we might be technically writing user_data to an entity that will get saved user data, but who cares, that will happen later
    ent.user_data = { color=Color:new():set_ucolor(math.random(0, 0xffffffff)) }
    ent.user_data.color.a = 1
    prinspect(ent.uid, "set userdata")

    -- apply saved userdata on next update
    -- for new entities this will be what we just wrote
    -- for carried entities saved data will be loaded before the first update
    ent:set_post_update_state_machine(function(ent)
        prinspect(ent.uid, "first update")
        apply_userdata(ent)
        -- no need to keep doing it again
        clear_callback()
    end)
end, SPAWN_TYPE.LEVEL_GEN, MASK.ITEM, {ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_SKULL})

-- apply color to items with user_data after screen load to transition
-- you could use this for levels too, but the thing above might be better if you don't need cosmetic changes in the transition
set_callback(function()
    if state.screen == SCREEN.TRANSITION then
        for i,v in pairs(get_entities_by({ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_SKULL}, MASK.ITEM, LAYER.FRONT)) do
            apply_userdata(get_entity(v))
        end
    end
end, ON.POST_LOAD_SCREEN)
