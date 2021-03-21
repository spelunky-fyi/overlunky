meta.name = "GPS2"
meta.version = "WIP"
meta.description = "Draws a path to the exit using Yonaba/Jumper. It runs almost fast enough to not lag your game, but is really just an example on real modules..."
meta.author = "Dregu"

local Grid = require "jumper.grid"
local Pathfinder = require "jumper.pathfinder"
local finders = Pathfinder:getFinders()

register_option_combo('finder', 'Finder algorithm to use', table.concat(finders, '\0'))

local grid = nil
local finder = nil
local exitpath = nil
local board = {}
local ntiles = 0
local tox, toy = 0, 0
local xmin, ymin, xmax, ymax = 0, 0, 0, 0

function recalculate()
    while true do
        if #players < 1 then
            return
        end
        tiles = get_entities_by(0, 0x100, 0)
        if #tiles ~= ntiles then
            board = {}
            xmin, ymin, xmax, ymax = get_bounds()
            for y = ymin - 0.5, ymax + 0.5, -1 do
                row = {}
                for x = xmin + 0.5, xmax - 0.5, 1 do
                    row[#row + 1] = 0
                end
                board[#board + 1] = row
            end
            for i,v in ipairs(tiles) do
                if v > 0 then
                    ent = get_entity(v)
                    if test_flag(ent.flags, 3) then
                        x, y, l = get_position(v)
                        tx = math.floor(x - xmin + 1)
                        ty = math.floor(ymin - y + 1)
                        if tx >= 1 and tx <= #board[1] and ty >= 1 and ty <= #board then
                            board[ty][tx] = 1
                        end
                    end
                end
            end
            grid = Grid(board)
            finder = Pathfinder(grid, finders[options.finder], 0)    
        end
        x, y, l = get_position(players[1].uid)
        fromx = math.floor(x - xmin + 1)
        fromy = math.floor(ymin - y + 1)
        exitpath = finder:getPath(fromx, fromy, tox, toy)
        coroutine.yield()
    end
end

set_callback(function()
    local exits = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
    if #exits < 1 then
        return
    end
    xmin, ymin, xmax, ymax = get_bounds()
    local ex, ey, el = get_position(exits[1])
    tox = math.floor(ex - xmin + 1)
    toy = math.floor(ymin - ey + 1)
    co = coroutine.create(recalculate)
    set_interval(function()
        coroutine.resume(co)
    end, 30)
end, ON.LEVEL)

set_callback(function()
    if state.screen ~= 12 then return end
    if exitpath ~= nil then
        local x, y, l = get_position(players[1].uid)
        local lastx, lasty = screen_position(x, y)
        for node, count in exitpath:nodes() do
            local sx, sy = screen_position(xmin + node:getX() - 0.5, ymin - node:getY() + 0.5)
            draw_line(lastx, lasty, sx, sy, 2, rgba(0, 255, 0, 255))
            lastx = sx
            lasty = sy
        end
    end
end, ON.GUIFRAME)
