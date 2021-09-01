meta.name = "Drunk dash"
meta.version = "WIP"
meta.description = "Press door button to do an awesome dash."
meta.author = "Dregu"

-- save door button state here to check if it's changed
local prev_door = { false, false, false, false }

-- check if player can use something with the door button, so we don't dash when buying something or entering a door
function can_use(uid)
  local x, y, l = get_position(uid)
  for i,v in ipairs(get_entities_at(ENT_TYPE.FX_BUTTON, 0, x, y, l, 5)) do
    local e = get_entity(v)
    if e.color.a > 0 then
      return true
    end
  end
  return false
end

-- check every frame
set_callback(function()
  -- all players can drunk dash
  for i,player in ipairs(players) do
    -- check if door button is pressed, we're not already stunned and there's nothing else we can do with the door button
    -- we don't want to dash in any of these states either:
    -- stunned (18), entering (19), loading (20), exiting (21) or dead(22)
    if test_flag(player.buttons, 6) and not prev_door[i] and player.stun_timer == 0 and not can_use(player.uid) and player.state < 18 then
      -- stun player and give them a little push in the right direction
      player.stun_timer = 30
      player.velocityx = player.movex/5
      player.velocityy = player.movey/10+0.1
    end
    prev_door[i] = test_flag(player.buttons, 6)
  end
end, ON.FRAME)
