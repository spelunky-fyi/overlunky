meta.name = "Gun test"
meta.version = "WIP"
meta.description = "Testing guns."
meta.author = "Dregu"

-- get info on the item the player is holding on every frame
set_callback(function()
    for i, player in ipairs(players) do

        -- are we holding something
        if player.holding_uid > -1 then
            gun = get_entity(player.holding_uid):as_gun()

            -- check if it's actually a gun
            if gun.type.id == ENT_TYPE.ITEM_SHOTGUN or gun.type.id == ENT_TYPE.ITEM_WEBGUN or gun.type.id ==
                ENT_TYPE.ITEM_CLONEGUN or gun.type.id == ENT_TYPE.ITEM_PLASMACANNON then

                -- print the gun variables on screen
                draw_text(-0.67, 0.87,
                    "Gun cooldown:" .. tostring(gun.cooldown) .. ", shots:" .. tostring(gun.shots) .. ", shots2:" ..
                        tostring(gun.shots2) .. ", chamber:" .. tostring(gun.in_chamber) .. " frame:" ..
                        tostring(gun.animation_frame), rgba(255, 0, 255, 255))

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
