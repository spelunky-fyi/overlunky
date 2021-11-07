meta.name = "Duat from any altar self-sacrifice"
meta.version = "WIP"
meta.description = "Sacrificing yourself on any altar with an ankh transitions you to Duat"
meta.author = "Zappatic"


transition_started = false
original_health = 4

function transition_to_duat()
    -- When transitioning, make sure we set the health back, otherwise we immediately die in Duat
    -- The player is dead though, so we set it directly in the player inventory
    state.items.player_inventory[1].health = original_health

    state.screen_next = ON.TRANSITION
    state.world_next = 4
    state.level_next = 4
    state.theme_next = THEME.DUAT
    state.ingame = 1
    state.fadeout = 18
    state.fadein = 18
    state.loading_black_screen_timer = 0
    state:force_current_theme(THEME.CITY_OF_GOLD) -- this makes it so we "come from" COG, which renders the correct limbo transition
    state.loading = 1
    transition_started = false
end

set_callback(function(render_ctx, page_type, page)
    local altars = get_entities_by_type(ENT_TYPE.FLOOR_ALTAR)
    for i, altar_uid in ipairs(altars) do
        -- Collision 2 on FLOOR_ALTAR is what does all the sacrificing stuff, so we do our own stuff just before that
        set_pre_collision2(altar_uid, function(self, collision_entity)
            if transition_started == false and collision_entity.uid == players[1].uid and players[1].state == 18 and players[1]:has_powerup(ENT_TYPE.ITEM_POWERUP_ANKH) then
                transition_started = true

                local powerups = players[1]:get_powerups()
                for j, powerup_uid in ipairs(powerups) do
                    local powerup = get_entity(powerup_uid)
                    if powerup ~= nil and powerup.type.id == ENT_TYPE.ITEM_POWERUP_ANKH then
                        powerup.move_state = 0 -- prevent the ankh from showing
                        break
                    end
                end
                players[1]:remove_powerup(ENT_TYPE.ITEM_POWERUP_ANKH)

                original_health = players[1].health -- save health for restoration later
                set_timeout(transition_to_duat, 100) -- transition after 100 frames (just like the game does)
            end
        end)
    end
end, ON.LEVEL)
