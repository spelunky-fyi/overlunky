meta.name = "Trap Randomizer"
meta.version = "WIP"
meta.description = "Add more traps, change default wall traps and put snap traps in unexpected places."
meta.author = "Dregu"

register_option_float("add_traps", "% of traps to add", 5, 0, 20)

floor_to = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_TIMED_FORCEFIELD,
            ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP, ENT_TYPE.ACTIVEFLOOR_ELEVATOR}
floor_item = {ENT_TYPE.FLOOR_SPRING_TRAP, ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SNAP_TRAP,
              ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
generic_to = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP}
wall_to = {ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP,
           ENT_TYPE.FLOOR_LASER_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP}
ceiling_to = {ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_SPIKEBALL_CEILING}
floortypes = {ENT_TYPE.FLOOR_GENERIC, ENT_TYPE.FLOORSTYLED_TEMPLE, ENT_TYPE.FLOORSTYLED_COG,
              ENT_TYPE.FLOORSTYLED_BABYLON, ENT_TYPE.FLOORSTYLED_DUAT, ENT_TYPE.FLOORSTYLED_STONE,
              ENT_TYPE.FLOOR_ARROW_TRAP, ENT_TYPE.FLOOR_POISONED_ARROW_TRAP, ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP,
              ENT_TYPE.FLOOR_LASER_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.FLOOR_TIMED_FORCEFIELD,
              ENT_TYPE.FLOOR_SPIKEBALL_CEILING, ENT_TYPE.FLOOR_STICKYTRAP_CEILING}
to = 0

function replace_trap(v)
    x, y, l = get_position(v)
    if l == LAYER.BACK and state.theme ~= THEME.ICE_CAVES then
        return false
    end

    top = get_entities_at(0, 0x180, x, y + 1, l, 0.1)
    bottom = get_entities_at(0, 0x180, x, y - 1.5, l, 0.6)
    left = get_entities_at(0, 0x180, x - 1, y, l, 0.1)
    right = get_entities_at(0, 0x180, x + 1, y, l, 0.1)
    lava = get_entities_at(ENT_TYPE.LIQUID_LAVA, 0, x, y, l, 2.0)
    shop = get_entities_at(ENT_TYPE.FX_SALEDIALOG_CONTAINER, 0, x, y, l, 6.0)
    shoppie = get_entities_at(ENT_TYPE.MONS_SHOPKEEPER, 0, x, y, l, 4.0)
    tusk = get_entities_at(ENT_TYPE.MONS_MADAMETUSK, 0, x, y, l, 6.0)
    altar = get_entities_at(ENT_TYPE.FLOOR_ALTAR, 0, x, y, l, 3.0)
    plr = get_entities_at(0, 1, x, y, l, 4.0)

    if #shop ~= 0 or #shoppie ~= 0 or #tusk ~= 0 or #altar ~= 0 or #plr ~= 0 then
        return false
    end -- don't put traps around shops, entrance or altars

    if #top == 0 then
        if math.random() < 0.5 and #bottom > 0 then -- replace with floor tile trap
            newid = floor_to[math.random(#floor_to)]
            if newid ~= 596 or #lava == 0 then
                kill_entity(v)
                spawn(newid, x, y, l, 0, 0)
            end
        else -- spawn floor item on top
            newid = floor_item[math.random(#floor_item)]
            if #lava == 0 then
                spawn(newid, x, y + 0.9, l, 0, 0)
            end
        end
    elseif #bottom == 0 and
        (state.theme ~= THEME.CITY_OF_GOLD and state.theme ~= THEME.ICE_CAVES and state.theme ~= THEME.TIAMAT) then -- replace with ceiling trap
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
        if newid ~= ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP then
            flip_entity(newe)
        end
    elseif #left == 0 and #right == 0 then -- make random wall trap
        kill_entity(v)
        newid = wall_to[math.random(#wall_to)]
        newe = spawn(newid, x, y, l, 0, 0)
        if math.random() < 0.5 and newid ~= ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP then
            flip_entity(newe)
        end
    else -- make generic tile trap
        newid = generic_to[math.random(#generic_to)]
        if newid ~= 596 or (#lava == 0 and #bottom > 0) then
            kill_entity(v)
            spawn(newid, x, y, l, 0, 0)
        end
    end
    return true
end

function rpairs(t)
    return function(t, i)
        i = i - 1
        if i ~= 0 then
            return i, t[i]
        end
    end, t, #t + 1
end

set_callback(function()
    set_timeout(function()
        to = 0
        if state.theme == THEME.SUNKEN_CITY or state.theme == THEME.HUNDUN or state.theme == THEME.EGGPLANT_WORLD or
            state.theme == THEME.DUAT then
            for i, v in rpairs(get_entities_by_type(floortypes)) do
                if math.random() < options.add_traps / 100.0 then
                    set_timeout(function()
                        replace_trap(v)
                    end, math.floor(to))
                    to = to + 10 / options.add_traps
                end
            end
        else
            for i, v in ipairs(get_entities_by_type(floortypes)) do
                if math.random() < options.add_traps / 100.0 then
                    set_timeout(function()
                        replace_trap(v)
                    end, math.floor(to))
                    to = to + 10 / options.add_traps
                end
            end
        end
    end, 10)
end, ON.LEVEL)

message("Initialized")
