set_callback(function(ctx, hud)
    -- draw on screen bottom but keep neat animations
    if hud.y > 0 then hud.y = -hud.y end
    -- spoof some values
    hud.data.inventory[1].health = prng:random_int(1, 99, 0)
    -- hide generic pickup items
    hud.data.inventory[1].item_count = 0
    -- hide money element
    hud.data.money.opacity = 0
    -- get real current opacity of p1 inventory element
    prinspect(hud.data.players[1].opacity * hud.data.opacity * hud.opacity)
end, ON.RENDER_PRE_HUD)
