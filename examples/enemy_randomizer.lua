meta.name = "Enemy Randomizer"
meta.version = "WIP"
meta.description = "Replace normal enemies with random ones!"
meta.author = "Dregu"

small = {220,221,222,224,225,227,229,230,231,233,234,235,237,238,239,240,241,242,243,245,246,247,248,250,251,252,260,261,262,263,264,266,267,268,272,273,275,276,277,278,280,283,284,286,295,310,884,885,886,888}
big = {232,244,249,253,265,269,270,271,274,279,288,311}
to = 0
tob = 0

function move_or_kill(v)
    e = get_entity(v):as_movable()
    if e.health == 0 then return end
    if state.theme == THEME.COSMIC_OCEAN then
        message("Killing "..tostring(v).." "..tostring(e.type.id))
        kill_entity(v)
    else
        message("Moving "..tostring(v).." "..tostring(e.type.id))
        move_entity(v, 20, 120-((state.height+1)*8), 0, 0)
    end
end

set_callback(function()
    to = 0
    tob = 0
    set_timeout(function()
        for i,v in ipairs(get_entities_by_type(220,221,222,224,225,227,229,230,233,234,235,237,238,239,240,241,242,243,245,246,247,248,250,251,252,260,261,262,263,264,266,267,268,272,273,275,276,277,278,280,283,284,286,295,310)) do
            set_timeout(function()
                x, y, l = get_position(v)
                if l == LAYER.FRONT then
                    move_or_kill(v)
                    newid = small[math.random(#small)]
                    new = spawn(newid, x, y, l, 0, 0)
                end
            end, to)
            to = to + 1
        end
        for i,v in ipairs(get_entities_by_type(232,244,249,253,265,269,270,271,274,279,288,311,887)) do
            set_timeout(function()
                x, y, l = get_position(v)
                if l == LAYER.FRONT then
                    move_or_kill(v)
                    newid = big[math.random(#big)]
                    new = spawn(newid, x, y, l, 0, 0)
                end
            end, tob)
            tob = tob + 1
        end
    end, 1)
end, ON.LEVEL)
