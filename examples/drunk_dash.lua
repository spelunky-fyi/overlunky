meta.name = "Drunk dash"
meta.version = "WIP"
meta.description = "Press door button to do an awesome dash."
meta.author = "Dregu"

prev = false

set_callback(function()
  if #players < 1 then return end
  door = test_flag(players[1].buttons, 6)
  if door and door ~= prev and players[1].stun_timer == 0 then
    set_timeout(function()
      if players[1].state < 19 then
        players[1].stun_timer = 30
        players[1].velocityx = players[1].movex/5
        players[1].velocityy = players[1].movey/10+0.1
      end
    end, 1)
  end
  prev = door
end, ON.FRAME)
