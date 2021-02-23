meta.name = "Hemophobia"
meta.version = "WIP"
meta.description = "One could call this a true pacifist mode. No, it doesn't help with your hemophobia, it gives you hemophobia."
meta.author = "Dregu"

health = {4, 4, 4, 4}

set_callback(function()
    for i,player in ipairs(players) do
        health[i] = player.health
    end
    set_interval(function()
        for i,player in ipairs(players) do
            if player.inventory.kills_total > 0 or player.health < health[i] then
                kill_entity(player.uid)
            end
            x, y, l = get_position(player.uid)
            blood = get_entities_at(ENT_TYPE.ITEM_BLOOD, 0, x, y, l, 1.0)
            if #blood > 0 then
                kill_entity(player.uid)
            end
            health[i] = player.health
        end
    end, 1)
end, ON.LEVEL)

set_callback(function()
    spawn(ENT_TYPE.ITEM_PICKUP_KAPALA, 0, -0.4, -1, 0, 0)
end, ON.START)
