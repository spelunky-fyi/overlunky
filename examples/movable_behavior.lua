meta = {
    name = "Movable Behavior Example",
    version = "WIP",
    description = "Makes all rocks soapy",
    author = "Mala🐈"
}

local custom_state_idle = {
    handle = nil,
    ---@param rock Movable
    get_next_state_id = function(rock, base_fun)
        if rock.overlay and rock.overlay:get_held_entity() == rock then
            return 2
        end
        return base_fun(rock)
    end
}
local custom_state_flee = {
    handle = nil,
    force_state = function(_, _)
        return false
    end,
    ---@param rock Movable
    on_enter = function(rock, _)
        local holding_ent = rock.overlay
        if holding_ent then
            drop(holding_ent.uid, rock.uid)

            spawn_entity_over(ENT_TYPE.LOGICAL_WET_EFFECT, rock.uid, 0, 0)
            rock.wet_effect_timer = 300;
        else
            rock.idle_counter = 300
        end
    end,
    ---@param rock Movable
    on_exit = function(rock, _)
        rock.wet_effect_timer = 0;
    end,
    ---@param rock Movable
    update_logic = function(rock, base_fun)
        return base_fun(rock)
    end,
    ---@param rock Movable
    update_world = function(rock, base_fun)
        if rock.velocityx == 0 then
            local x, y, l = get_position(rock.uid)
            local close_players = get_entities_at(0, MASK.PLAYER, x, y, l, 0.6);
            if close_players:size() > 0 then
                local first_player_uid = close_players[1]
                local px, _, _ = get_position(first_player_uid)
                local facing_left = px > x

                rock.velocityx = 0.11
                rock.velocityy = 0.1

                if facing_left then
                    rock.velocityx = -rock.velocityx
                end
            end
        end
        return base_fun(rock)
    end,
    ---@param rock Movable
    get_next_state_id = function(rock, _)
        if rock.stand_counter >= 300 then
            return 1
        end
    end
}

---@param rock Movable
set_post_entity_spawn(function(rock)
    local vanilla_state_idle = rock:get_base_behavior(1)

    if custom_state_idle.handle == nil then
        custom_state_idle.handle = make_custom_behavior("cool_rock_idle", 1, vanilla_state_idle)
        custom_state_idle.handle:set_get_next_state_id(custom_state_idle.get_next_state_id)

        custom_state_flee.handle = make_custom_behavior("cool_rock_flee", 2, vanilla_state_idle)
        custom_state_flee.handle:set_force_state(custom_state_flee.force_state)
        custom_state_flee.handle:set_on_enter(custom_state_flee.on_enter)
        custom_state_flee.handle:set_on_exit(custom_state_flee.on_exit)
        custom_state_flee.handle:set_update_logic(custom_state_flee.update_logic)
        custom_state_flee.handle:set_update_world(custom_state_flee.update_world)
        custom_state_flee.handle:set_get_next_state_id(custom_state_flee.get_next_state_id)
    end

    rock:clear_behavior(vanilla_state_idle)
    rock:add_behavior(custom_state_idle.handle)
    rock:add_behavior(custom_state_flee.handle)
end, SPAWN_TYPE.ANY, MASK.ANY, ENT_TYPE.ITEM_ROCK)
