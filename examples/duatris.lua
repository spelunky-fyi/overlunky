meta.name = 'Duatris'
meta.version = 'WIP'
meta.description = 'How about a nice game of Duatris? Use directions to move, hold down to drop. Hold door button to stay still. Reach the finish line to stop spawning tetrominos and kill Osiris to... start all over again, faster!'
meta.author = 'Dregu'

register_option_int('baserate', 'Base fall rate (frames)', 60, 1, 180)
register_option_int('crates', 'Spawn lootboxes', 8, 0, 30)
register_option_bool('door', 'Only move pieces when holding door button', true)
register_option_bool('enemies', 'Spawn enemies on blocks', true)
register_option_int('enemychance', "Enemy chance (percent)", 50, 1, 100)
register_option_bool('traps', 'Spawn traps on blocks', true)
register_option_int('trapschance', "Trap chance (percent)", 10, 1, 100)
register_option_bool('wgoodies', 'Get some goodies at start (for cheating)', false)
register_option_bool('whole', 'Draw whole stage (for debugging)', false)

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
            ENT_TYPE.MONS_LEPRECHAUN}

generic_to = {ENT_TYPE.FLOOR_JUNGLE_SPEAR_TRAP, ENT_TYPE.FLOOR_SPARK_TRAP, ENT_TYPE.ACTIVEFLOOR_CRUSH_TRAP,
              ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK, ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.FLOOR_QUICKSAND}

loot_to = {ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_CRATE, ENT_TYPE.ITEM_PICKUP_PLAYERBAG,
           ENT_TYPE.ITEM_PICKUP_ROYALJELLY, ENT_TYPE.ITEM_PRESENT}

local keys = {
    LEFT = 1,
    RIGHT = 2,
    UP = 3,
    DOWN = 4
}

local keystate = {
    LEFT = false,
    RIGHT = false,
    UP = false,
    DOWN = false
}

local keystart = {
    LEFT = 0,
    RIGHT = 0,
    UP = 0,
    DOWN = 0
}
local down_sent = false

local orig_shapes = {{{0, 1, 0}, {1, 1, 1}}, {{0, 1, 1}, {1, 1, 0}}, {{1, 1, 0}, {0, 1, 1}}, {{1, 1, 1, 1}},
                     {{1, 1}, {1, 1}}, {{1, 0, 0}, {1, 1, 1}}, {{0, 0, 1}, {1, 1, 1}}}
local shapes = {}

local colors = {{rgba(255, 255, 255, 200), ENT_TYPE.FLOORSTYLED_STONE},
                {rgba(0, 0, 255, 200), ENT_TYPE.FLOORSTYLED_DUAT}, {rgba(0, 255, 255, 200), ENT_TYPE.FLOOR_SURFACE},
                {rgba(0, 255, 0, 200), ENT_TYPE.FLOOR_JUNGLE}, {rgba(255, 0, 255, 200), ENT_TYPE.FLOORSTYLED_BABYLON},
                {rgba(255, 0, 0, 200), ENT_TYPE.FLOORSTYLED_VLAD}, {rgba(255, 255, 0, 200), ENT_TYPE.FLOORSTYLED_COG}}

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

local guicall = -1
local framecall = -1
local crates = {}

function init()
    game_state = 'playing'
    board = {}
    shapes = {}
    moving_piece = {}
    crates = {}
    players[1].type.max_speed = 0.0725

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

function get_button()
    if #players < 1 then
        return nil
    end
    if test_flag(players[1].buttons, 6) and players[1].standing_on_uid > 0 then
        players[1].type.max_speed = 0
    else
        players[1].type.max_speed = 0.0725
    end
    if players[1].movex == 0 then
        keystate.LEFT = false
        keystate.RIGHT = false
        keystart.LEFT = 0
        keystart.RIGHT = 0
    end
    if players[1].movey == 0 then
        keystate.UP = false
        keystate.DOWN = false
        keystart.UP = 0
        keystart.DOWN = 0
        down_sent = false
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
        return keys.UP
    elseif not keystate.DOWN and players[1].movey < 0 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystate.DOWN = true
        keystart.DOWN = get_frame()
    elseif keystate.LEFT and players[1].movex < 0 and get_frame() >= keystart.LEFT + 15 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.LEFT = get_frame() - 10
        return keys.LEFT
    elseif keystate.RIGHT and players[1].movex > 0 and get_frame() >= keystart.RIGHT + 15 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.RIGHT = get_frame() - 10
        return keys.RIGHT
    elseif keystate.UP and players[1].movey > 0 and get_frame() >= keystart.UP + 15 and (not options.door or test_flag(players[1].buttons, 6)) then
        keystart.UP = get_frame() - 10
        return keys.UP
    elseif keystate.DOWN and players[1].movey < 0 and not down_sent and get_frame() >= keystart.DOWN + 15 and (not options.door or test_flag(players[1].buttons, 6)) then
        down_sent = true
        return keys.DOWN
    end
    return nil
end

function handle_input(fall, next_piece)
    local key = get_button()
    if key == nil then
        return
    end

    if game_state ~= 'playing' then
        return
    end -- Arrow keys only work if playing.

    -- Handle the left, right, or up arrows.
    local new_rot_num = (moving_piece.rot_num % 4) + 1 -- Map 1->2->3->4->1.
    local moves = {
        [keys.LEFT] = {
            x = moving_piece.x - 1
        },
        [keys.RIGHT] = {
            x = moving_piece.x + 1
        },
        [keys.UP] = {
            rot_num = new_rot_num
        }
    }
    if moves[key] then
        set_moving_piece_if_valid(moves[key])
    end

    -- Handle the down arrow.
    if key == keys.DOWN then
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

function update_moving_piece(fall, next_piece)
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
        spawn(ENT_TYPE.FX_POWEREDEXPLOSION, ex, ey, LAYER.FRONT, 0, 0)
        set_timeout(function()
            update_moving_piece(fall, next_piece)
        end, 45)
    end
    next_piece.shape = math.random(#shapes)
end

function lock_and_update_moving_piece(fall, next_piece)
    level_to_board(false)
    call_fn_for_xy_in_piece(moving_piece, function(x, y, c)
        board[x][y] = moving_piece.shape -- Lock the moving piece in place.
        gx = x + 2
        gy = 124 - y
        --id = colors[moving_piece.shape][2]
        id = ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK
        --[[if options.traps and math.random() - state.level_count / 30 < options.trapschance / 100 then
            id = generic_to[math.random(#generic_to)]
        end]]
        newid = spawn(id, gx, gy, LAYER.FRONT, 0, 0)
        ent = get_entity(newid)
        newflags = set_flag(ent.flags, 10) -- disables gravity
        newflags = clr_flag(newflags, 13) -- disables push
        ent.flags = newflags
    end)
    if options.enemies and math.random() - state.level_count / 10 < options.enemychance / 100 then
        gx = moving_piece.x + 4 + math.random(-1, 1)
        gy = 124 - moving_piece.y + 1
        spawnid = small_to[math.random(#small_to)]
        spawn(spawnid, gx, gy, LAYER.FRONT, 0, 0)
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
                    board[x][y] = 1
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
                end
            end
        end
    end
end

function draw_point(x, y, color)
    draw_color = colors[moving_piece.shape][1]
    if color then
        draw_color = colors[color][1]
    end
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(x + 2 - 0.5, 124 - y + 0.5)
    sx2, sy2 = screen_position(x + 2 + 0.5, 124 - y - 0.5)
    draw_rect(sx, sy, sx2, sy2, 4, 0, draw_color)
end

function draw_moving(x, y, color)
    draw_color = colors[moving_piece.shape][1]
    if color then
        draw_color = colors[color][1]
    end
    xmin, ymin, xmax, ymax = get_bounds()
    sx, sy = screen_position(x + 2 - 0.5, 124 - y + 0.5)
    sx2, sy2 = screen_position(x + 2 + 0.5, 124 - y - 0.5)
    if sy < -1 then
        sy = -0.99
    end
    if sy > 1 then
        sy = 0.99
    end
    if sy2 < -1 then
        sy2 = -0.99
    end
    if sy2 > 1 then
        sy2 = 0.99
    end
    draw_rect(sx, sy, sx2, sy2, 4, 0, draw_color)
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

    call_fn_for_xy_in_piece(moving_piece, draw_moving, moving_piece.shape)

    -- draw finishline
    sx, sy = screen_position(2.5, 107)
    sx2, sy2 = screen_position(32.5, 107)
    draw_line(sx, sy, sx2, sy2, 4, colors[6][1])
end

function game_over()
    game_state = 'over'
    clear_callback(guicall)
    clear_callback(framecall)
end

function clear_stage()
    zoom(0)
    clear_callback(guicall)
    clear_callback(framecall)
    x, y, l = get_position(players[1].uid)
    if options.wgoodies then
        spawn(ENT_TYPE.ITEM_VLADS_CAPE, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PASTE, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_PLAYERBAG, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_SPIKESHOES, x, y, l, 0, 0)
        spawn(ENT_TYPE.ITEM_PICKUP_SPRINGSHOES, x, y, l, 0, 0)
    end
    spawn(ENT_TYPE.ITEM_PICKUP_BOMBBAG, x, y, l, 0, 0) -- get bombs anyway so you don't get stuck
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
        level_to_board(true)
        guicall = set_callback(function()
            handle_input(fall, next_piece)
            lower_piece_at_right_time(fall, next_piece)
            draw_screen()
        end, ON.GUIFRAME)
        framecall = set_callback(function()
            if #players < 1 then
                game_over()
                return
            end
            plx, ply, pll = get_position(players[1].uid)
            if ply > 107 then
                game_over()
                return
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
