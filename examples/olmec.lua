meta.name = "Olmec"
meta.version = "WIP"
meta.description = "Displays Olmec timings"
meta.author = "Zappatic"

set_callback(function()
    olmecs = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_OLMEC)
    x = -0.95
    for i, olmec_uid in ipairs(olmecs) do
        olmec = get_entity(olmec_uid):as_olmec()
        y = -0.50
        white = rgba(255, 255, 255, 255)

        attack_phase = "stomp"
        if olmec.attack_phase == 1 then attack_phase = "bombs"
        elseif olmec.attack_phase == 2 then attack_phase = "stomp+ufos"
        elseif olmec.attack_phase == 3 then attack_phase = "in lava" end

        draw_text(x, y, 0, "Olmec attack phase: " .. attack_phase, white)
        y = y - 0.05
        draw_text(x, y, 0, "Attack timer: " .. tostring(olmec.attack_timer), white)
        y = y - 0.05
        draw_text(x, y, 0, "Jump timer: " .. tostring(olmec.jump_timer), white)
        y = y - 0.05
        draw_text(x, y, 0, "AI timer: " .. tostring(olmec.ai_timer), white)
        y = y - 0.05
        
        move_direction = "down"
        if olmec.move_direction < 0 then move_direction = "left" 
        elseif olmec.move_direction > 0 then move_direction = "right" end
        draw_text(x, y, 0, "Move direction: " .. move_direction, white)
        y = y - 0.05
        
        if olmec.attack_phase == 1 then
            draw_text(x, y, 0, "Bomb salvos left: " .. olmec.phase1_amount_of_bomb_salvos, white)
            y = y - 0.05
            draw_text(x, y, 0, "Broken floaters: " .. olmec:broken_floaters(), white)
            y = y - 0.05
            draw_text(x, y, 0, "Unknown attack state: " .. olmec.unknown_attack_state, white)
            y = y - 0.05
        elseif olmec.attack_phase == 2 then
            draw_text(x, y, 0, "Unknown attack state: " .. olmec.unknown_attack_state, white)
            y = y - 0.05
        end
        x = x + 0.4
    end
end, ON.GUIFRAME)
