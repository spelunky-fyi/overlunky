meta.name = "Trap Randomizer"
meta.version = "WIP"
meta.description = "Add more traps, change default wall traps and put snap traps in unexpected places."
meta.author = "Dregu"

register_option_float("add_traps", "% of traps to add", 3, 0, 20)

floor = {73, 439, 469, 596, 604}
generic_from = {43, 46}
generic_to = {43, 46, 596}
wall_from = {40, 41, 43, 45}
wall_to = {40, 41, 43, 45, 46}
ceiling_from = {60, 80, 461}
ceiling_to = {46, 60, 80}

set_callback(function()
set_timeout(function()
    -- replace generic traps
    for i,v in ipairs(get_entities_by_type(generic_from)) do
        x, y, l = get_position(v)
        e = get_entity(v)
        newid = generic_to[math.random(#generic_to)]
        if e.type.id ~= newid then
            kill_entity(v)
            spawn(newid, x, y, l, 0, 0)
        end
    end

    --replace wall traps
    for i,v in ipairs(get_entities_by_type(wall_from)) do
        x, y, l = get_position(v)
        e = get_entity(v)
        facing_left = test_flag(e.flags, 17)
        newid = wall_to[math.random(#wall_to)]
        if e.type.id ~= newid then
            kill_entity(v)
            newe = spawn(newid, x, y, l, 0, 0)
            if facing_left then
                flip(newe)
            end
        end
    end

    --replace ceiling traps
    for i,v in ipairs(get_entities_by_type(ceiling_from)) do
        x, y, l = get_position(v)
        e = get_entity(v)
        newid = ceiling_to[math.random(#ceiling_to)]
        if e.type.id ~= newid then
            kill_entity(v)
            spawn(newid, x, y, l, 0, 0)
        end
    end

    -- replace a % of floor with new traps
    for i,v in ipairs(get_entities_by(ENT_TYPE.FLOOR_GENERIC, 0, LAYER.FRONT)) do
        if math.random() < options.add_traps/100.0 then
            x, y, l = get_position(v)
            top = get_entities_at(0, 0x100, x, y+1, l, 0.1)
            bottom = get_entities_at(0, 0x100, x, y-1, l, 0.1)
            left = get_entities_at(0, 0x100, x-1, y, l, 0.1)
            right = get_entities_at(0, 0x100, x+1, y, l, 0.1)
            if #top == 0 then
                if math.random() < 0.5 then -- replace with generic tile trap
                    kill_entity(v)
                    newid = generic_to[math.random(#generic_to)]
                    spawn(newid, x, y, l, 0, 0)
                else -- spawn item on floor
                    newid = floor[math.random(#floor)]
                    spawn(newid, x, y+1, l, 0, 0)
                end
            elseif #bottom == 0 then -- replace with ceiling trap
                kill_entity(v)
                newid = ceiling_to[math.random(#ceiling_to)]
                spawn(newid, x, y, l, 0, 0)
            elseif #left > 0 and #right == 0 then -- make right facing wall trap
                kill_entity(v)
                newid = wall_to[math.random(#wall_to)]
                spawn(newid, x, y, l, 0, 0)
            elseif #left == 0 and #right > 0 then -- make left facing wall trap
                kill_entity(v)
                newid = wall_to[math.random(#wall_to)]
                newe = spawn(newid, x, y, l, 0, 0)
                flip(newe)
            elseif #left == 0 and #right == 0 then -- make random wall trap
                kill_entity(v)
                newid = wall_to[math.random(#wall_to)]
                newe = spawn(newid, x, y, l, 0, 0)
                if math.random() < 0.5 then
                    flip(newe)
                end
            end
        end
    end
end, 10)
end, ON.LEVEL)
