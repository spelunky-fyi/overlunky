meta.name = "Inventory Example"
meta.version = "0.1"
meta.description = "Examples for using player inventory."
meta.author = "jeremyhay"

local names = {}
for i,v in pairs(ENT_TYPE) do
  names[v] = i
end

-- display the items and the powerups in the player's inventory
set_callback(function()
    if players and players[1] then
        x, y, l = get_position(players[1].uid)
        -- you may want to mask only the items and logical (powerups)
        -- here so you don't get any unwanted FX
        -- note that ITEM_WHIP shows up while you are whipping
        local ANY_ITEM = 0
        local items = entity_get_items_by(players[1].uid, ANY_ITEM, MASK.ITEM | MASK.LOGICAL)
        local str = ""
        for i, uid in ipairs(items) do
            local entity = get_entity(uid)
            local type = get_type(entity.type.id)
            str = str .. " " .. names[entity.type.id]
        end
        sx, sy = screen_position(x, y-1)
        draw_text(sx, sy, 0, str, rgba(255, 255, 255, 255))
    end
end, ON.GUIFRAME)


-- Auto drop broken arrows
set_callback(function()
    if players and players[1] then
        local items = entity_get_items_by(players[1].uid, ENT_TYPE.ITEM_BROKEN_ARROW, 0)
        if (#items > 0) then
            for i, uid in ipairs(items) do
                entity_remove_item(players[1].uid, uid)
            end
        end
    end
end, ON.GAMEFRAME)
