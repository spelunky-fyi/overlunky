meta.name = "Drunk dash"
meta.version = "WIP"
meta.description = "Press door button to do an awesome dash."
meta.author = "Dregu"

-- check every frame
set_callback(function()
  -- all players can drunk dash
  for i,player in ipairs(players) do
    -- check if door button is pressed and we're not already stunned
    if test_flag(player.buttons, 6) and player.stun_timer == 0 then
      -- delay the dash for one frame to not mess with actual door entrance
      set_timeout(function()
        -- we don't want to dash in any of these states:
        -- stunned (18), entering (19), loading (20), exiting (21) or dead(22)
        if player.state < 18 then
          -- stun player and give them a little push in the right direction
          player.stun_timer = 30
          player.velocityx = player.movex/5
          player.velocityy = player.movey/10+0.1
        end
      end, 1)
    end
  end
end, ON.FRAME)
