meta = {
    name = "SetRooms",
    description = "Place new setrooms in the level.",
    author = "Malacath",
    version = "WIP"
}

local has_udjat_room

-- force to load the quillback level
set_callback(function(ctx)
    if state.theme == THEME.DWELLING and state.level ~= 4 then
        has_udjat_room = false
        ctx:override_level_files({"generic.lvl", "cavebossarea.lvl"})
    end
end, ON.PRE_LOAD_LEVEL_FILES)

set_callback(function(ctx)
    if state.theme == THEME.DWELLING and state.level ~= 4 then
        -- set the whole level to use setrooms
        for x = 0, state.width - 1 do
            for y = 0, state.height - 1 do
                ctx:mark_as_set_room(x, y, LAYER.FRONT)
            end
        end

        -- place the coffin manually in the middle instead of the shortcut, this does not use a setroom
        ctx:unmark_as_set_room(2, state.height - 1, LAYER.FRONT)
        if prng:random() % 2 == 0 then
            ctx:set_room_template(2, state.height - 1, LAYER.FRONT, ROOM_TEMPLATE.COFFIN_UNLOCKABLE)
        else
            ctx:set_room_template(2, state.height - 1, LAYER.FRONT, ROOM_TEMPLATE.COFFIN_UNLOCKABLE_LEFT)
        end

        -- place the entrance in the top middle, still uses a setroom but makes sure that the room spawns an entrance door
        ctx:set_room_template(2, 0, LAYER.FRONT, ROOM_TEMPLATE.ENTRANCE)

        -- place the exits in the corners, still uses a setroom but makes sure that the room spawns an entrance door
        ctx:set_room_template(0, 0, LAYER.FRONT, ROOM_TEMPLATE.EXIT)
        ctx:set_room_template(state.width - 1, 0, LAYER.FRONT, ROOM_TEMPLATE.EXIT)
    end
end, ON.POST_ROOM_GENERATION)

-- Replace the coffin in the bottom middle with a udjat chest
set_pre_tile_code_callback(function(x, y, l, room_template)
    if has_udjat_room then
        has_udjat_room = false
        spawn_entity(ENT_TYPE.ITEM_LOCKEDCHEST, x, y, l, 0, 0)
        return true
    end
end, "coffin")

-- make sure the game doesn't piss itself when trying to spawn the udjat room
set_callback(function(_, _, _, room_template)
    if room_template == ROOM_TEMPLATE.UDJATTOP then
        has_udjat_room = true
        return [[
XXXXXXXXXX
XXXXXXXXXX
XXXXXXXXXX
XXXXXXXXXX
XXXXXXXXXX
XXXXXXXXXX
XXXXXXXXXX
XXXXXXXXXX
        ]]
    end
end, ON.PRE_GET_RANDOM_ROOM)
