meta.name = "User data"
meta.description = "Extra lives and colorful junk user_data examples"
meta.version = "1.1"
meta.author = "Dregu"

-- there are three separate examples in this script, just pick one approach that works for your case
-- all the results seem pretty random, but the point is all the pots and skulls keep their random color
-- when carried to the next level


-- 1: using ON.USER_DATA, it will trigger on every write to ent.user_data, including when loaded after transitions
-- this is probably the simplest way
set_callback(function(ent)
    -- it's good to make sure it's valid, especially with the other examples in the same script
    if ent.user_data and ent.user_data.color then
        ent.color = ent.user_data.color
    end
end, ON.USER_DATA)
set_post_entity_spawn(function(ent)
    local random_color = Color:new():set_ucolor(math.random(0, 0xffffffff))
    random_color.a = 1
    -- this will trigger the callback above
    ent.user_data = { color=random_color }
end, SPAWN_TYPE.LEVEL_GEN | SPAWN_TYPE.SYSTEMIC, MASK.ITEM, ENT_TYPE.ITEM_SKULL)


-- 2: here we just check user_data before death and don't care what it is before that
set_callback(function()
    players[1].user_data = {lives = 2}
    print(F"You have {players[1].user_data.lives} extra lives!")
end, ON.START)
set_callback(function()
    if state.screen_next == SCREEN.DEATH and players[1] and players[1].user_data and players[1].user_data.lives and players[1].user_data.lives > 0 then
        players[1].flags = clr_flag(players[1].flags, ENT_FLAG.DEAD)
        players[1].health = 4
        players[1]:set_behavior(1)
        players[1].user_data.lives = players[1].user_data.lives - 1
        print(F"You have {players[1].user_data.lives} lives left!")
        return true
    end
end, ON.PRE_LOAD_SCREEN)


-- 3: apply user data on every update. this might be a smart thing with cosmetic changes,
-- cause the game likes to mess with the colors too when entering doors for example
function apply_userdata(ent)
    if ent.user_data and ent.user_data.color3 then
        ent.color = ent.user_data.color3
    end
end
set_post_entity_spawn(function(ent)
    -- saved user_data from the previous level is not available yet in post_entity_spawn, it is loaded right before post_level_gen
    -- we might be technically writing user_data to an entity that will get saved user data, but who cares, that will happen later
    local random_color = Color:new():set_ucolor(math.random(0, 0xffffffff))
    random_color.a = 1
    ent.user_data = { color3=random_color }
    -- for new entities this will be what we just wrote
    -- for carried entities saved data will be loaded before the first update
    ent:set_post_update_state_machine(apply_userdata)
    -- SPAWN_TYPE.LEVEL_GEN won't trigger in transitions
end, SPAWN_TYPE.ANY, MASK.ITEM, ENT_TYPE.ITEM_POT)
