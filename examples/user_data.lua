meta.name = "User data"
meta.description = "Extra lives user_data example"
meta.version = "1.0"
meta.author = "Dregu"

set_callback(function()
    players[1].user_data = {lives = 2}
    print(F"You have {players[1].user_data.lives} extra lives!")
end, ON.START)

set_callback(function()
    if state.screen_next == SCREEN.DEATH and players[1] and players[1].user_data.lives and players[1].user_data.lives > 0 then
        players[1].flags = clr_flag(players[1].flags, ENT_FLAG.DEAD)
        players[1].health = 4
        players[1]:set_behavior(1)
        players[1].user_data.lives = players[1].user_data.lives - 1
        print(F"You have {players[1].user_data.lives} lives left!")
        return true
    end
end, ON.PRE_LOAD_SCREEN)
