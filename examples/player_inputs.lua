meta.name = 'Player inputs'
meta.version = '1'
meta.description = 'Shows buttons/input information about the player in slot 1'
meta.author = 'Zappatic'

player_slot = state.player_inputs.player_slot_1
player_slot_settings = state.player_inputs.player_slot_1_settings
white = rgba(255, 255, 255, 255)
red = rgba(230, 60, 45, 255)

set_callback(function()
    y = -0.50;

    autorun = player_slot_settings.auto_run_enabled and "Auto run enabled" or "Auto run disabled"
    draw_text(-0.95, y, 0, autorun, white)

    y = y - 0.05

    color = test_flag(player_slot.buttons, 1) and red or white
    draw_text(-0.95, y, 0, "JUMP", color)

    y = y - 0.05

    color = test_flag(player_slot.buttons, 2) and red or white
    draw_text(-0.95, y, 0, "ATTACK", color)

    y = y - 0.05

    color = test_flag(player_slot.buttons, 3) and red or white
    draw_text(-0.95, y, 0, "BOMB", color)

    y = y - 0.05

    color = test_flag(player_slot.buttons, 4) and red or white
    draw_text(-0.95, y, 0, "ROPE", color)

    y = y - 0.05

    color = test_flag(player_slot.buttons, 5) and red or white
    draw_text(-0.95, y, 0, "RUN", color)
    
    y = y - 0.05

    color = test_flag(player_slot.buttons, 6) and red or white
    draw_text(-0.95, y, 0, "USE DOOR/BUY", color)
    
    y = y - 0.05

    color = test_flag(player_slot.buttons, 7) and red or white
    draw_text(-0.95, y, 0, "PAUSE", color)
    
    y = y - 0.05

    color = test_flag(player_slot.buttons, 8) and red or white
    draw_text(-0.95, y, 0, "JOURNAL", color)

    y = y + 0.20
    x = -0.75

    color = test_flag(player_slot.buttons, 9) and red or white
    draw_text(x, y, 0, "LEFT", color)

    color = test_flag(player_slot.buttons, 10) and red or white
    draw_text(x + 0.10, y, 0, "RIGHT", color)

    color = test_flag(player_slot.buttons, 11) and red or white
    draw_text(x + 0.06, y + 0.10, 0, "UP", color)

    color = test_flag(player_slot.buttons, 12) and red or white
    draw_text(x + 0.035, y - 0.10, 0, "DOWN", color)


end, ON.GUIFRAME)
