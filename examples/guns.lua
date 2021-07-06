meta.name = "Gun test"
meta.version = "WIP"
meta.description = "Testing guns."
meta.author = "Dregu"

-- get info on the item the player is holding on every frame
set_callback(function()
    for i, player in ipairs(players) do

        -- are we holding something
        if player.holding_uid > -1 then
            gun = get_entity(player.holding_uid)

            -- check if it's actually a gun
            if gun.type.id == ENT_TYPE.ITEM_SHOTGUN or gun.type.id == ENT_TYPE.ITEM_WEBGUN or gun.type.id ==
                ENT_TYPE.ITEM_CLONEGUN or gun.type.id == ENT_TYPE.ITEM_PLASMACANNON then

                --format string
                text_display = F"Player:{i}, Gun cooldown:{gun.cooldown}, shots:{gun.shots}, shots2:{gun.shots2}"
                    
                --if webgun, add the extra info
                if gun.type.id == ENT_TYPE.ITEM_WEBGUN then
                    text_display = F"{text_display}, chamber:{gun.in_chamber}"
                end
                text_display = F"{text_display}, frame:{gun.animation_frame}"
                    
                -- print the gun variables on screen
                draw_text(-0.67, (9 - i) / 10.0, 24, text_display, rgba(255, 0, 255, 255))

                -- make all guns super fast and infinite
                if gun.cooldown > 30 then
                    gun.cooldown = 15
                end
                gun.shots = 0
                gun.shots2 = 0

                -- reset clone gun to full, or it will loop through the whole sprite sheet
                if gun.type.id == ENT_TYPE.ITEM_CLONEGUN then
                    gun.animation_frame = 0x98
                end
            end
        end
    end
end, ON.GUIFRAME)
