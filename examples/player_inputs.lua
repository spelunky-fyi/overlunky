meta.name = 'Player inputs'
meta.version = '1'
meta.description = 'Shows buttons/input information about the player in slot 1'
meta.author = 'Zappatic'

player_slot = state.player_inputs.player_slot_1
player_slot_settings = state.player_inputs.player_slot_1_settings
white = rgba(255, 255, 255, 255)
red = rgba(230, 60, 45, 255)
font_size = 16
up_width, line_height = draw_text_size(font_size, "UP")
down_width, _ = draw_text_size(font_size, "DOWN")
left_width, _ = draw_text_size(font_size, "LEFT")
right_width, _ = draw_text_size(font_size, "RIGHT")
cross_center_x = -0.70
cross_center_hor_margin = 0.05

set_callback(function(draw_ctx)
    y = -1 - line_height * 9.5;
    x = -0.98

    autorun = player_slot_settings.auto_run_enabled and "Auto run enabled" or "Auto run disabled"
    draw_ctx:draw_text(x, y, font_size, autorun, white)

    y = y + line_height

    color = test_flag(player_slot.buttons, 1) and red or white
    draw_ctx:draw_text(x, y, font_size, "JUMP", color)

    y = y + line_height

    color = test_flag(player_slot.buttons, 2) and red or white
    draw_ctx:draw_text(x, y, font_size, "ATTACK", color)

    y = y + line_height

    color = test_flag(player_slot.buttons, 3) and red or white
    draw_ctx:draw_text(x, y, font_size, "BOMB", color)

    y = y + line_height

    color = test_flag(player_slot.buttons, 4) and red or white
    draw_ctx:draw_text(x, y, font_size, "ROPE", color)

    y = y + line_height

    color = test_flag(player_slot.buttons, 5) and red or white
    draw_ctx:draw_text(x, y, font_size, "RUN", color)
    
    y = y + line_height

    color = test_flag(player_slot.buttons, 6) and red or white
    draw_ctx:draw_text(x, y, font_size, "USE DOOR/BUY", color)
    
    y = y + line_height

    color = test_flag(player_slot.buttons, 7) and red or white
    draw_ctx:draw_text(x, y, font_size, "PAUSE", color)
    
    y = y + line_height

    color = test_flag(player_slot.buttons, 8) and red or white
    draw_ctx:draw_text(x, y, font_size, "JOURNAL", color)

    y = -1 - line_height * 5.5;

    color = test_flag(player_slot.buttons, 9) and red or white
    draw_ctx:draw_text(cross_center_x - cross_center_hor_margin - left_width, y, font_size, "LEFT", color)

    color = test_flag(player_slot.buttons, 10) and red or white
    draw_ctx:draw_text(cross_center_x + cross_center_hor_margin, y, font_size, "RIGHT", color)

    y = -1 - line_height * 7.5;

    color = test_flag(player_slot.buttons, 11) and red or white
    draw_ctx:draw_text(cross_center_x - (up_width / 2.0), y, font_size, "UP", color)

    y = -1 - line_height * 3.5;

    color = test_flag(player_slot.buttons, 12) and red or white
    draw_ctx:draw_text(cross_center_x - (down_width / 2.0), y, font_size, "DOWN", color)

end, ON.GUIFRAME)
