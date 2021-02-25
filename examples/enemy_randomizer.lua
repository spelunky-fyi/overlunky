meta.name = "Enemy Randomizer"
meta.version = "WIP"
meta.description = "Replace normal enemies with random ones!"
meta.author = "Dregu"

small = {220,221,222,224,225,227,229,230,231,233,234,235,237,238,239,240,241,242,243,245,246,247,248,250,251,252,260,261,262,263,264,266,267,268,272,273,275,276,277,278,280,283,284,286,289,295,310,884,885,886}
big = {232,244,249,253,265,269,270,271,274,279,288,311,887}

function move_or_kill(v)
    e = get_entity(v):as_movable()
    if e.health == 0 then return end
    if state.theme == THEME.COSMIC_OCEAN then
        --message("Killing "..tostring(v).." "..tostring(e.type.id))
        move_entity(v, 20, 120-((state.height)*8)+4, 0, 0)
        kill_entity(v)
    else
        --message("Moving "..tostring(v).." "..tostring(e.type.id))
        move_entity(v, 20, 120-((state.height+1)*8), 0, 0)
    end
end

function create_rider(id, type, x, y, l)
    rid = spawn(ENT_TYPE.MONS_CAVEMAN, x, y, l, 0, 0)
    carry(id, rid)
end

function replace_enemy(id, from)
    x, y, l = get_position(id)
    if x == 0 or (l == LAYER.BACK and state.theme ~= THEME.ICE_CAVES and state.theme ~= THEME.OLMEC) then return end
    move_or_kill(id)
    newid = from[math.random(#from)]
    if l == LAYER.BACK and state.theme == THEME.OLMEC then
        newid = ENT_TYPE.MONS_CAVEMAN_BOSS
    end
    new = spawn(newid, x, y, l, 0, 0)
    if newid > 880 then
        create_rider(new, newid, x, y, l)
    end
end

set_callback(function()
    set_timeout(function()
        mons_small = get_entities_by_type(220,221,222,224,225,227,229,230,233,234,235,237,238,239,240,241,242,243,245,246,247,248,250,251,252,260,261,262,263,264,266,267,268,272,273,275,276,277,278,280,283,284,286,289,290,310)
        mons_big = get_entities_by_type(232,244,249,253,265,269,270,271,274,279,288,311,887)
        to = 0
        for i,v in ipairs(mons_small) do
            set_timeout(function()
                replace_enemy(v, small)
            end, to)
            to = to + 1
        end

        for i,v in ipairs(mons_big) do
            set_timeout(function()
                replace_enemy(v, big)
            end, to)
            to = to + 1
        end
    end, 5)
end, ON.LEVEL)

message("Initialized")
