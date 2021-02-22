meta.name = "Trap Randomizer"
meta.version = "WIP"
meta.description = "Add more traps, change default wall traps and put snap traps in unexpected places."
meta.author = "Dregu"

register_option_float("add_traps", "% of traps to add", 4, 0, 20)

floor_from = {43, 46, 85, 89, 596, 604}
floor_to = {43, 46, 89, 596, 604}
floor_item = {73, 439, 469, 596}
generic_from = {43, 46, 604}
generic_to = {43, 46, 604}
wall_from = {40, 41, 43, 45}
wall_to = {40, 41, 43, 45, 46}
ceiling_from = {60, 80, 461}
ceiling_to = {46, 60, 80}
floortypes = {ENT_TYPE.FLOOR_GENERIC, ENT_TYPE.FLOORSTYLED_TEMPLE, ENT_TYPE.FLOORSTYLED_COG, ENT_TYPE.FLOORSTYLED_BABYLON, ENT_TYPE.FLOORSTYLED_DUAT, ENT_TYPE.FLOORSTYLED_STONE}
replaced = {}

set_callback(function()
    replaced = {}
    set_timeout(function()
        -- replace generic tile traps
        for i,v in ipairs(get_entities_by_type(generic_from)) do
            x, y, l = get_position(v)
            e = get_entity(v)
            newid = generic_to[math.random(#generic_to)]
            bottom = get_entities_at(0, 0x180, x, y-1, l, 0.1)
            if e.type.id ~= newid and not replaced[v] and (bottom > 0 or newid ~= 596) then
                kill_entity(v)
                spawn(newid, x, y, l, 0, 0)
                replaced[v] = true
            end
        end

        -- replace floor tile traps
        for i,v in ipairs(get_entities_by_type(floor_from)) do
            x, y, l = get_position(v)
            e = get_entity(v)
            newid = floor_to[math.random(#floor_to)]
            bottom = get_entities_at(0, 0x180, x, y-1, l, 0.1)
            if e.type.id ~= newid and not replaced[v] and (bottom > 0 or newid ~= 596) then
                kill_entity(v)
                spawn(newid, x, y, l, 0, 0)
                replaced[v] = true
            end
        end

        --replace wall traps
        for i,v in ipairs(get_entities_by_type(wall_from)) do
            x, y, l = get_position(v)
            e = get_entity(v)
            facing_left = test_flag(e.flags, 17)
            newid = wall_to[math.random(#wall_to)]
            if e.type.id ~= newid and not replaced[v] then
                kill_entity(v)
                newe = spawn(newid, x, y, l, 0, 0)
                if facing_left then
                    flip_entity(newe)
                end
                replaced[v] = true
            end
        end

        --replace ceiling traps
        if state.theme ~= THEME.CITY_OF_GOLD then -- these textures are glitched
            for i,v in ipairs(get_entities_by_type(ceiling_from)) do
                x, y, l = get_position(v)
                e = get_entity(v)
                newid = ceiling_to[math.random(#ceiling_to)]
                if e.type.id ~= newid and not replaced[v] then
                    kill_entity(v)
                    spawn(newid, x, y, l, 0, 0)
                    replaced[v] = true
                end
            end
        end
    end, 5)

    set_timeout(function()
        -- replace a % of floor with new traps
        for i,v in ipairs(get_entities_by_type(floortypes)) do
            x, y, l = get_position(v)
            if l == LAYER.FRONT and math.random() < options.add_traps/100.0 then
                top = get_entities_at(0, 0x180, x, y+1, l, 0.1)
                bottom = get_entities_at(0, 0x180, x, y-1, l, 0.1)
                left = get_entities_at(0, 0x180, x-1, y, l, 0.1)
                right = get_entities_at(0, 0x180, x+1, y, l, 0.1)
                lava = get_entities_at(ENT_TYPE.LIQUID_LAVA, 0, x, y, l, 2.0)
                shop = get_entities_at(ENT_TYPE.MONS_SHOPKEEPER, 0, x, y, l, 10.0)
                merch = get_entities_at(ENT_TYPE.MONS_MERCHANT, 0, x, y, l, 8.0)
                plr = get_entities_at(0, 1, x, y, l, 4.0)

                if #shop ~= 0 or #merch ~= 0 or #plr ~= 0 then goto continue end -- don't put traps around shops or entrance
                if #top == 0 then
                    if math.random() < 0.5 and #bottom > 0 then -- replace with floor tile trap
                        newid = floor_to[math.random(#floor_to)]
                        if newid ~= 596 or #lava == 0 then
                            kill_entity(v)
                            spawn(newid, x, y, l, 0, 0)
                        end
                    else -- spawn floor item on top
                        newid = floor_item[math.random(#floor_item)]
                        if newid ~= 596 or #lava == 0 then
                            spawn(newid, x, y+0.9, l, 0, 0)
                        end
                    end
                elseif #bottom == 0 and state.theme ~= THEME.CITY_OF_GOLD then -- replace with ceiling trap
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
                    flip_entity(newe)
                elseif #left == 0 and #right == 0 then -- make random wall trap
                    kill_entity(v)
                    newid = wall_to[math.random(#wall_to)]
                    newe = spawn(newid, x, y, l, 0, 0)
                    if math.random() < 0.5 then
                        flip_entity(newe)
                    end
                else -- make generic tile trap
                    newid = generic_to[math.random(#generic_to)]
                    if newid ~= 596 or #lava == 0 then
                        kill_entity(v)
                        spawn(newid, x, y, l, 0, 0)
                    end
                end
                ::continue::
            end
        end
    end, 10)
end, ON.LEVEL)

message("Initialized")
