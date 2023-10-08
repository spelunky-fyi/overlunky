 
function add_money(amount)
    -- need to change the amount first, this alone would just change the amount of money in the HUD
    -- without the little pop up below, showing how much money you gained
    -- there is also state.money_last_levels but that's only used in the transition to display the difference
    state.money_shop_total += amount
    
    -- it's actually subtracting the amount from the number on screen first for the effect
    add_money_hud(amount)
end

-- just another way of achieving the same thing
-- there doesn't seam to be difference between changing money in state.money_shop_total vs inventory.money
function add_money_slot(amount, slot)
    -- check if the money will be negative after the transaction
    -- it's actually fine for the game to have negative money, this is just example
    if get_current_money() + amount >= 0 then
        -- just another of way of changing the money
        -- there is also inventory.collected_money_total but that's meant for the money last level, so no reason to edit it
        state.items.player_inventory[slot].money += amount
        add_money_hud(amount)
        return true
    end
    return false
end
