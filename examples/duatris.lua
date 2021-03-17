meta.name = 'Duatris'
meta.version = '1.0'
meta.description = 'How about a nice game of Duatris? Hold DOOR button to stay still and control the pieces with D-PAD. Rotate with UP or ROPE/BOMB. While your piece is hilighted, you can move it. After the lock delay, it will spawn traps and foes, so watch out! Reach the finish line to stop spawning tetrominos and kill Osiris to... start all over again, faster!'
meta.author = 'Dregu'

register_option_int('baserate', 'Base fall rate (frames)', 50, 1, 180)
register_option_int('crates', 'Spawn lootboxes', 8, 0, 30)
register_option_bool('door', 'Only move pieces when holding DOOR button', true)
register_option_bool('drop', 'Hard drop with DOWN instead of moving faster', false)
register_option_bool('drop2', 'Hard drop with UP instead of rotating', false)
register_option_bool('enemies', 'Spawn enemies on blocks', true)
register_option_int('enemychance', "Enemy chance (percent)", 50, 1, 100)
register_option_bool('traps', 'Spawn traps on blocks', true)
register_option_int('trapschance', "Trap chance (percent)", 10, 1, 100)
register_option_bool('wgoodies', 'Get some goodies at start (for cheating)', false)
register_option_bool('whole', 'Draw whole stage (for debugging)', false)

tiny_to = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_OLMITE_NAKED,
           ENT_TYPE.ITEM_LANDMINE}

small_to = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_HANGSPIDER, ENT_TYPE.MONS_BAT,
            ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
            ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_MANTRAP, ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_WITCHDOCTOR,
            ENT_TYPE.MONS_MOSQUITO, ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.MONS_ROBOT,
            ENT_TYPE.MONS_FIREBUG, ENT_TYPE.MONS_FIREBUG_UNCHAINED, ENT_TYPE.MONS_IMP, ENT_TYPE.MONS_VAMPIRE,
            ENT_TYPE.MONS_VLAD, ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS,
            ENT_TYPE.MONS_CATMUMMY, ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI,
            ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_UFO, ENT_TYPE.MONS_ALIEN,
            ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER, ENT_TYPE.MONS_OLMITE_HELMET,
            ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED, ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_AMMIT,
            ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG, ENT_TYPE.MONS_GRUB, ENT_TYPE.MONS_JUMPDOG, ENT_TYPE.MONS_SCARAB,
            ENT_TYPE.MONS_LEPRECHAUN, ENT_TYPE.MONS_CAVEMAN_BOSS, ENT_TYPE.MONS_QUEENBEE, ENT_TYPE.MONS_GIANTFLY,
            ENT_TYPE.MONS_CRABMAN, ENT_TYPE.ITEM_LANDMINE}

generic_to = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP,
              ENT_TYPE.FLOOR_QUICKSAND, ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK, ENT_TYPE.ACTIVEFLOOR_POWDERKEG}

loot_to = {ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_PICKUP_PLAYERBAG,
           ENT_TYPE.ITEM_PICKUP_ROYALJELLY, ENT_TYPE.ITEM_PRESENT}

local keys = {
    LEFT = 1,
    RIGHT = 2,
    ROTL = 3,
    ROTR = 4,
    DOWN = 5,
    DROP = 6
}

local keystate = {
    LEFT = false,
    RIGHT = false,
    UP = false,
    DOWN = false,
    ROPE = false,
    BOMB = false
}

local keystart = {
    LEFT = 0,
    RIGHT = 0,
    UP = 0,
    DOWN = 0,
    ROPE = 0,
    BOMB = 0
}
local drop_sent = false

local orig_shapes = {
    {{0, 1, 0},
     {1, 1, 1},
     {0, 0, 0}},

    {{0, 1, 1},
     {1, 1, 0},
     {0, 0, 0}},

    {{1, 1, 0},
     {0, 1, 1},
     {0, 0, 0}},

    {{0, 0, 0, 0},
     {1, 1, 1, 1},
     {0, 0, 0, 0},
     {0, 0, 0, 0}},

    {{1, 1},
     {1, 1}},

    {{1, 0, 0},
     {1, 1, 1},
     {0, 0, 0}},

    {{0, 0, 1},
     {1, 1, 1},
     {0, 0, 0}}
}
local shapes = {}

-- local colors = {{255, 255, 255}, {128, 128, 255}, {80, 255, 255}, {80, 255, 80}, {255, 80, 255}, {255, 80, 80}, {255, 255, 80}}
local colors = {{255, 80, 255}, {128, 255, 128}, {255, 80, 80}, {80, 255, 255}, {255, 255, 80}, {128, 128, 255},
                {255, 160, 40}, {255, 255, 255}}
local color_alpha = 66

local game_state = 'playing'

local board_size = {
    x = 30,
    y = 81
}
local board = {} -- board[x][y] = shape_num; 0=empty; -1=border.
local val = {
    border = -1,
    empty = 0
} -- Shorthand to avoid magic numbers.

local moving_piece = {} -- Keys will be: shape, rot_num, x, y.
local moving_blocks = {-1, -1, -1, -1} -- store uids

local guicall = -1
local framecall = -1
local crates = {}
local ropes = -1
local bombs = -1

function init()
    game_state = 'playing'
    board = {}
    shapes = {}
    moving_piece = {}
    moving_blocks = {}
    crates = {}
    ropes = -1
    bombs = -1

    -- Set up the shapes table.
    for s_index, s in ipairs(orig_shapes) do
        shapes[s_index] = {}
        for rot_num = 1, 4 do
            -- Set up new_shape as s rotated by 90 degrees.
            local new_shape = {}
            local x_end = #s[1] + 1 -- Chosen so that x_end - x is in [1, x_max].
            for x = 1, #s[1] do -- Coords x & y are indexes for the new shape.
                new_shape[x] = {}
                for y = 1, #s do
                    new_shape[x][y] = s[y][x_end - x]
                end
            end
            s = new_shape
            shapes[s_index][rot_num] = s
        end
    end

    -- Set up the board.
    local border = {
        x = board_size.x + 1,
        y = board_size.y + 1
    }
    for x = 0, border.x do
        board[x] = {}
        for y = 1, border.y do
            board[x][y] = val.empty
            if x == 0 or x == border.x or y == border.y then
                board[x][y] = val.border -- This is a border cell.
            end
        end
    end

    -- Set up the next and currently moving piece.
    cx, cy = get_camera_position()
    x, y, l = get_position(players[1].uid)
    px = math.max(math.floor(x - 4), 1)
    py = math.max(math.floor(124 - cy) - 12, 1)
    moving_piece = {
        shape = math.random(#shapes),
        rot_num = 1,
        x = px,
        y = py
    }
    call_fn_for_xy_in_piece(moving_piece, function(x, y, c)
        gx = x + 2
        gy = 124 - y
        id = ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK
        newid = spawn(id, gx, gy, LAYER.FRONT, 0, 0)
        ent = get_entity(newid):as_movable()
        newflags = set_flag(ent.flags, 10) -- disable gravity
        newflags = set_flag(newflags, 6) -- disable damage
        newflags = clr_flag(newflags, 13) -- disable push
        ent.flags = newflags
        ent.color.r = colors[moving_piece.shape][1] / 255
        ent.color.g = colors[moving_piece.shape][2] / 255
        ent.color.b = colors[moving_piece.shape][3] / 255
        -- ent.color.a = 0.85
        moving_blocks[#moving_blocks + 1] = newid
    end)

    -- Use a table so functions can edit its value without having to return it.
    next_piece = {
        shape = math.random(#shapes)
    }

    -- fall.interval is the number of seconds between downward piece movements.
    local fall = {
        interval = math.max(options.baserate - state.level_count * 5, 5)
    } -- A 'last_at' time is added to this table later.

    return fall, next_piece
end

function get_button(fall)
    if #players < 1 then
        return nil
    end
    if test_flag(players[1].buttons, 6) then
        if ropes == -1 then
            ropes = players[1].inventory.ropes
        end
        if bombs == -1 then
            bombs = players[1].inventory.bombs
        end
        players[1].inventory.ropes = 0
        players[1].inventory.bombs = 0
        if players[1].standing_on_uid > 0 then
            players[1].type.max_speed = 0
        end
    else
        players[1].type.max_speed = 0.0725
        if ropes >= 0 then
            players[1].inventory.ropes = ropes
        end
        if bombs >= 0 then
            players[1].inventory.bombs = bombs
        end
        ropes = -1
        bombs = -1
    end
    if players[1].movex == 0 and players[1].movey == 0 then
        keystate.LEFT = false
        keystate.RIGHT = false
        keystate.UP = false
        keystate.DOWN = false
        drop_sent = false
    end
    if not test_flag(players[1].buttons, 3) then
        keystate.BOMB = false
    end
    if not test_flag(players[1].buttons, 4) then
        keystate.ROPE = false
    end
    if not keystate.LEFT and players[1].movex < 0 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystate.LEFT = true
        keystart.LEFT = get_frame()
        return keys.LEFT
    elseif not keystate.RIGHT and players[1].movex > 0 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystate.RIGHT = true
        keystart.RIGHT = get_frame()
        return keys.RIGHT
    elseif not keystate.UP and players[1].movey > 0 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystate.UP = true
        keystart.UP = get_frame()
        if not options.drop2 then
            return keys.ROTR
        end
    elseif not keystate.DOWN and players[1].movey < 0 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystate.DOWN = true
        keystart.DOWN = get_frame()
        return keys.DOWN
    elseif keystate.LEFT and players[1].movex < 0 and get_frame() >= keystart.LEFT + 15 and
        (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.LEFT = get_frame() - 10
        return keys.LEFT
    elseif keystate.RIGHT and players[1].movex > 0 and get_frame() >= keystart.RIGHT + 15 and
        (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.RIGHT = get_frame() - 10
        return keys.RIGHT
    elseif keystate.UP and players[1].movey > 0 and
        (not options.door or test_flag(players[1].buttons, 6)) then
        if not options.drop2 then
            if get_frame() >= keystart.UP + 15 then
                keystart.UP = get_frame() - 10
                return keys.ROTR
            end
        elseif not drop_sent then
            drop_sent = true
            return keys.DROP
        end
    elseif not options.drop and keystate.DOWN and players[1].movey < 0 and not drop_sent and get_frame() > keystart.DOWN +
        15 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.DOWN = get_frame() - 10
        return keys.DOWN
    elseif options.drop and keystate.DOWN and players[1].movey < 0 and not drop_sent and get_frame() > keystart.DOWN +
        15 and (not options.door or test_flag(players[1].buttons, 6)) then
        drop_sent = true
        return keys.DROP
    elseif not keystate.ROPE and test_flag(players[1].buttons, 4) and (not options.door or test_flag(players[1].buttons, 6)) then
        keystate.ROPE = true
        keystart.ROPE = get_frame()
        return keys.ROTL
    elseif keystate.ROPE and test_flag(players[1].buttons, 4) and get_frame() >= keystart.ROPE + 15 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.ROPE = get_frame() - 10
        return keys.ROTL
    elseif not keystate.BOMB and test_flag(players[1].buttons, 3) and (not options.door or test_flag(players[1].buttons, 6)) then
        keystate.BOMB = true
        keystart.BOMB = get_frame()
        return keys.ROTR
    elseif keystate.BOMB and test_flag(players[1].buttons, 3) and get_frame() >= keystart.BOMB + 15 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.BOMB = get_frame() - 10
        return keys.ROTR
    end
    return nil
end

function handle_input(fall, next_piece)
    local key = get_button(fall)
    if key == nil then
        return
    end

    if game_state ~= 'playing' then
        return
    end -- Arrow keys only work if playing.

    -- Handle the left, right, or up arrows.
    local new_rot_numr = (moving_piece.rot_num % 4) + 1 -- Map 1->2->3->4->1.
    local new_rot_numl = moving_piece.rot_num - 1
    if new_rot_numl < 1 then
        new_rot_numl = 4
    end
    local moves = {
        [keys.LEFT] = {
            x = moving_piece.x - 1
        },
        [keys.RIGHT] = {
            x = moving_piece.x + 1
        },
        [keys.DOWN] = {
            y = moving_piece.y + 1
        },
        [keys.ROTL] = {
            rot_num = new_rot_numl
        },
        [keys.ROTR] = {
            rot_num = new_rot_numr
        }
    }
    if moves[key] then
        local was_valid = set_moving_piece_if_valid(moves[key])
        if key == keys.DOWN and was_valid then
            fall.last_at = state.time_level
        end
    end

    -- Handle the down arrow.
    if key == keys.DROP then
        while set_moving_piece_if_valid({
            y = moving_piece.y + 1
        }) do
        end
        lock_and_update_moving_piece(fall, next_piece)
    end
end

-- Returns true if and only if the move was valid.
function set_moving_piece_if_valid(piece)
    level_to_board(false)
    -- Use values of moving_piece as defaults.
    for k, v in pairs(moving_piece) do
        if piece[k] == nil then
            piece[k] = moving_piece[k]
        end
    end
    local is_valid = true
    call_fn_for_xy_in_piece(piece, function(x, y)
        ents = get_entities_at(0, 0x180, x+2, 124-y, LAYER.FRONT, 0.65)
        if #ents > 0 then
            local is_moving = false
            for i,v in ipairs(ents) do
                for j,w in ipairs(moving_blocks) do
                    if v == w then
                        is_moving = true
                    end
                end
            end
            if not is_moving then
                is_valid = false
                if board[x] and board[x][y] then
                    board[x][y] = 8
                end
            end
        end
        if board[x] and board[x][y] ~= val.empty then
            is_valid = false
        end
    end)
    if is_valid then
        moving_piece = piece
    end
    return is_valid
end

function call_fn_for_xy_in_piece(piece, callback, param)
    local s = shapes[piece.shape][piece.rot_num]
    for x, row in ipairs(s) do
        for y, val in ipairs(row) do
            if val == 1 then
                callback(piece.x + x, piece.y + y, param)
            end
        end
    end
end

function random_offset(piece)
    minoff = 40
    maxoff = 0
    maxy = 120
    call_fn_for_xy_in_piece(piece, function(x, y, c)
        if x > maxoff then maxoff = x end
        if x < minoff then minoff = x end
        if y < maxy then maxy = y end
    end)
    return math.random(minoff, maxoff) + 2, 124 - maxy + 1
end

function update_moving_piece(fall, next_piece)
    level_to_board(false)
    -- Bring in the waiting next piece and set up a new next piece.
    cx, cy = get_camera_position()
    x, y, l = get_position(players[1].uid)
    px = math.max(math.floor(x - 4), 1)
    py = math.max(math.floor(124 - cy) - 12, 1)
    moving_piece = {
        shape = math.random(#shapes),
        rot_num = math.random(1, 4),
        x = px,
        y = py
    }
    if not set_moving_piece_if_valid(moving_piece) then
        ex = moving_piece.x + 4 + math.random(-1, 1)
        ey = 124 - moving_piece.y - 2 + math.random(-1, 1)
        trash = get_entities_at(0, 0x180, ex, ey, LAYER.FRONT, 8)
        for i,v in ipairs(trash) do
            ent = get_entity(v):as_movable()
            ent.flags = clr_flag(ent.flags, 6)
        end
        spawn(ENT_TYPE.FX_POWEREDEXPLOSION, ex, ey, LAYER.FRONT, 0, 0)
        set_timeout(function()
            update_moving_piece(fall, next_piece)
        end, options.baserate)
    else
        call_fn_for_xy_in_piece(moving_piece, function(x, y, c)
            gx = x + 2
            gy = 124 - y
            id = ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK
            newid = spawn(id, gx, gy, LAYER.FRONT, 0, 0)
            ent = get_entity(newid):as_movable()
            newflags = set_flag(ent.flags, 10) -- disable gravity
            newflags = set_flag(newflags, 6) -- disable damage
            newflags = clr_flag(newflags, 13) -- disable push
            ent.flags = newflags
            ent.color.r = colors[moving_piece.shape][1] / 255
            ent.color.g = colors[moving_piece.shape][2] / 255
            ent.color.b = colors[moving_piece.shape][3] / 255
            -- ent.color.a = 0.85
            moving_blocks[#moving_blocks + 1] = newid
        end)
        if options.enemies and math.random() - state.level_count / 10 < options.enemychance / 100 then
            gx, gy = random_offset(moving_piece)
            spawnid = tiny_to[math.random(#tiny_to)]
            spawn(spawnid, gx, gy, LAYER.FRONT, 0, 0)
        end
    end
    next_piece.shape = math.random(#shapes)
end

function replace_with_trap(blockid)
    set_timeout(function()
        x, y, l = get_position(blockid)
        if x > 0 then
            ent = get_entity(blockid):as_movable()
            r = ent.color.r
            g = ent.color.g
            b = ent.color.b
            trapid = generic_to[math.random(#generic_to)]
            kill_entity(blockid)
            newid = spawn(trapid, x, y, l, 0, 0)
            trap = get_entity(newid):as_movable()
            if trapid == ENT_TYPE.ACTIVEFLOOR_POWDERKEG then
                trap.color.r = 0.5
                trap.color.g = 0
                trap.color.b = 0
            elseif trapid == ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK then
                trap.color.r = 1
                trap.color.g = 1
                trap.color.b = 1
            else
                trap.color.r = r
                trap.color.g = g
                trap.color.b = b
            end
        end
    end, 1)
end

function lock_and_update_moving_piece(fall, next_piece)
    level_to_board(false)
    block_i = 1
    call_fn_for_xy_in_piece(moving_piece, function(x, y, c)
        board[x][y] = moving_piece.shape -- Lock the moving piece in place.
        gx = x + 2
        gy = 124 - y
        ent = get_entity(moving_blocks[block_i])
        if moving_blocks[block_i] and moving_blocks[block_i] > -1 and ent ~= nil then
            move_entity(moving_blocks[block_i], x + 2, 124 - y, LAYER.FRONT, 0, 0)
            ent = get_entity(moving_blocks[block_i])
            if ent == nil then return end
            ent = ent:as_movable()
            if ent then
                ent.flags = clr_flag(ent.flags, 6) -- enable damage
                -- ent.color.a = 1
            end
            if options.traps and math.random() - state.level_count / 30 < options.trapschance / 100 then
                replace_with_trap(moving_blocks[block_i])
            end
        end
        block_i = block_i + 1
    end)
    moving_blocks = {}
    if options.enemies and math.random() - state.level_count / 10 < options.enemychance / 100 then
        gx, gy = random_offset(moving_piece)
        spawnid = small_to[math.random(#small_to)]
        spawn(spawnid, gx, gy, LAYER.FRONT, 0, 0)
        spawn(ENT_TYPE.FX_TELEPORTSHADOW, gx, gy, LAYER.FRONT, 0, 0)
    end
    set_timeout(function()
        level_to_board(false)
    end, 5)
    update_moving_piece(fall, next_piece)
end

function lower_piece_at_right_time(fall, next_piece)
    -- This function does nothing if the game is paused or over.
    if game_state ~= 'playing' then
        return
    end

    local timestamp = state.time_level
    if fall.last_at == nil then
        fall.last_at = timestamp
    end -- Happens at startup.

    -- Do nothing until it's been fall.interval seconds since the last fall.
    if timestamp - fall.last_at < fall.interval then
        return
    end

    if not set_moving_piece_if_valid({
        y = moving_piece.y + 1
    }) then
        lock_and_update_moving_piece(fall, next_piece)
    end
    fall.last_at = timestamp
end

function level_to_board(all)
    if all then
        for x = 1, board_size.x, 1 do
            for y = board_size.y - 10, board_size.y, 1 do
                gx = x + 2
                gy = 124 - y
                block = get_entities_at(0, 0x180, gx, gy, LAYER.FRONT, 0.5)
                if #block > 0 then
                    board[x][y] = 8
                end
            end
        end
    else
        bx = moving_piece.x
        by = moving_piece.y
        for nx = math.max(bx - 6, 1), math.min(bx + 6, board_size.x) do
            for ny = math.max(by - 6, 1), math.min(by + 6, board_size.y) do
                if board[nx][ny] > 0 then
                    gx = nx + 2
                    gy = 124 - ny
                    block = get_entities_at(0, 0x180, gx, gy, LAYER.FRONT, 0.5)
                    if #block == 0 then
                        board[nx][ny] = 0
                    end
                --[[elseif board[nx][ny] == 0 then
                    gx = nx + 2
                    gy = 124 - ny
                    block = get_entities_at(0, 0x180, gx, gy, LAYER.FRONT, 0.5)
                    if #block == 0 then
                        is_moving = false
                        for i,v in ipairs(block) do
                            for j,w in ipairs(moving_blocks) do
                                if w == v then
                                    is_moving = true
                                end
                            end
                        end
                        if not is_moving then
                            board[nx][ny] = 1
                        end
                    end]]
                end
            end
        end
    end
end

function draw_point(x, y, color)
    draw_color = rgba(colors[moving_piece.shape][1], colors[moving_piece.shape][2], colors[moving_piece.shape][3],
                     color_alpha)
    if color then
        draw_color = rgba(colors[color][1], colors[color][2], colors[color][3], color_alpha)
    end
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(x + 2 - 0.5, 124 - y + 0.5)
    sx2, sy2 = screen_position(x + 2 + 0.5, 124 - y - 0.5)
    draw_rect(sx, sy, sx2, sy2, 4, 0, draw_color)
end

function draw_moving(x, y, color)
    draw_color = rgba(colors[moving_piece.shape][1], colors[moving_piece.shape][2], colors[moving_piece.shape][3],
                     color_alpha)
    if color then
        draw_color = rgba(colors[color][1], colors[color][2], colors[color][3], color_alpha)
    end
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(x + 2 - 0.5, 124 - y + 0.5)
    sx2, sy2 = screen_position(x + 2 + 0.5, 124 - y - 0.5)
    if sy < -1 then
        sy = -0.995
    end
    if sy > 1 then
        sy = 0.995
    end
    if sy2 < -1 then
        sy2 = -0.995
    end
    if sy2 > 1 then
        sy2 = 0.99
    end
    -- draw_rect(sx, sy, sx2, sy2, 4, 0, draw_color)
    draw_rect_filled(sx, sy, sx2, sy2, 0, rgba(255, 255, 255, 55))
    if moving_blocks[block_i] and moving_blocks[block_i] > -1 then
        move_entity(moving_blocks[block_i], x + 2, 124 - y, LAYER.FRONT, 0, 0)
    end
    block_i = block_i + 1
end

function draw_screen()
    if game_state ~= 'playing' or state.pause > 0 then
        return
    end

    if options.whole then
        for x = 1, board_size.x do
            for y = 1, board_size.y do
                if board[x][y] > 0 then
                    draw_point(x, y, board[x][y])
                end
            end
        end
    end
    block_i = 1
    call_fn_for_xy_in_piece(moving_piece, draw_moving, moving_piece.shape)

    -- draw finishline
    sx, sy = screen_position(2.5, 107)
    sx2, sy2 = screen_position(32.5, 107)
    draw_line(sx, sy, sx2, sy2, 4, rgba(255, 255, 255, color_alpha))
end

function game_over()
    game_state = 'over'
    clear_callback(guicall)
    --clear_callback(framecall)
    if #players > 0 then
        players[1].type.max_speed = 0.0725
        if bombs > 0 then
            players[1].inventory.bombs = bombs
            bombs = -1
        end
        if ropes > 0 then
            players[1].inventory.ropes = ropes
            ropes = -1
        end
    end
end

function clear_stage()
    zoom(0)
    clear_callback(guicall)
    clear_callback(framecall)
    x, y, l = get_position(players[1].uid)
    players[1].type.max_speed = 0.0725
    players[1].flags = set_flag(players[1].flags, 6)
    if options.wgoodies then
        spawn(ENT_TYPE.ITEM_CAPE, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PASTE, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_SPIKESHOES, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_SPRINGSHOES, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_ROYALJELLY, x, y, l, 0, 0)
    else
        if players[1].inventory.bombs < 4 then
            players[1].inventory.bombs = 4 -- get bombs on all levels so you don't get stuck
        end
    end
    cy = y + 4.5
    n = 1
    clearint = set_interval(function()
        spawn(ENT_TYPE.MONS_APEP_HEAD, -1, cy, LAYER.FRONT, 0, 0)
        n = n + 1
        cy = cy + 2
        if cy > 107 then
            clear_callback(clearint)
        end
    end, 1)
    set_timeout(function()
        fall, next_piece = init()
        set_timeout(function()
            players[1].flags = clr_flag(players[1].flags, 6)
        end, 3*60)
        level_to_board(true)
        guicall = set_callback(function()
            handle_input(fall, next_piece)
            lower_piece_at_right_time(fall, next_piece)
            draw_screen()
        end, ON.GUIFRAME)
        framecall = set_callback(function()
            if #players < 1 then
                game_over()
            else
                plx, ply, pll = get_position(players[1].uid)
                if ply > 107 then
                    game_over()
                end
            end
            for ci = 1, options.crates, 1 do
                h = math.floor(104 - (board_size.y - 20) / (options.crates + 1) * ci)
                if ply > h and not crates[h] then
                    crates[h] = true
                    xmin, ymin, xmax, ymax = get_bounds()
                    rx = math.random(math.floor(xmin), math.floor(xmax))
                    id = loot_to[math.random(#loot_to)]
                    spawn(id, rx, 107, LAYER.FRONT, math.random() - 0.5, 0)
                    toast('FREE LOOT!')
                end
            end
            crush = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP)
            for i,v in ipairs(crush) do
                x, y, l = get_position(v)
                if y > 108 and x > 15 and x < 20 then
                    spawn(ENT_TYPE.FX_EXPLOSION, x, y, l, 0, 0)
                end
            end
        end, ON.FRAME)
    end, 3 * 60)
end
set_callback(clear_stage, ON.LEVEL)

set_callback(function()
    state.world_next = 4
    state.level_next = 4
    state.theme_next = THEME.DUAT
end, ON.LOADING)

set_callback(game_over, ON.DEATH)
