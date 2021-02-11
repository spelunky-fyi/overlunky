meta.name = "Enemy Randomizer"
meta.version = "WIP"
meta.description = "Replace normal enemies with random ones!"
meta.author = "Dregu"

small = {220,221,222,224,225,227,229,230,231,233,234,235,237,238,239,240,241,242,243,245,246,247,248,250,251,252,260,261,263,264,266,267,268,272,273,275,276,277,278,280,283,284,286,310}
big = {232,244,249,253,265,269,270,271,274,279,288,311}

set_callback(function()
    set_timeout(function()
        for i,v in ipairs(get_entities_by_type(220,221,222,224,225,227,229,230,233,234,235,237,238,239,240,242,243,245,246,247,248,250,251,252,260,261,263,264,266,267,268,272,273,275,276,277,278,280,283,284,286,310)) do
            x, y, l = get_position(v)
            move_entity(v, 0, 0, 0, 0) -- we don't have a destroy function yet
            spawn(small[math.random(#small)], x, y, l, 0, 0)
        end
        for i,v in ipairs(get_entities_by_type(232,244,249,253,265,269,270,271,274,279,288,311)) do
            x, y, l = get_position(v)
            move_entity(v, 0, 0, 0, 0) -- we don't have a destroy function yet
            spawn(big[math.random(#big)], x, y, l, 0, 0)
        end
    end, 20)
end, ON.LEVEL)
