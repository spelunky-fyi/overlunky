meta.name = "GPS"
meta.version = "WIP"
meta.description = "Draws a path to the exit using Dijkstra algorithm. Pathfinding code edited from Yonaba/dijkstra-map."
meta.author = "Dregu"

register_option_bool("update", "Recalculate paths when blowing stuff up (Slow!)", false)

board = {}
ntiles = 0
exitpath = {}

function dijkstra(grid, source)
    listOfNodes = grid.getAllNodes() -- Get the list of all nodes
    -- We set all nodes distance to infinity, and clear their previous property, if defined.
    for i, node in ipairs(listOfNodes) do
        node.distance = math.huge
        node.previous = nil
    end

    -- We set the source node distance to 0, and sort the list of node in increasing 
    -- distance order, so that the source node bubbles at the top of this list.
    source.distance = 0
    table.sort(listOfNodes, function(nodeA, nodeB)
        return nodeA.distance < nodeB.distance
    end)

    -- While there are still some nodes to examine in our list
    while (#listOfNodes > 0) do
        -- We pop and remove the node at the top of this list.
        -- It should be the node with the lowest distance.
        currentNode = listOfNodes[1]
        table.remove(listOfNodes, 1)

        -- In case we got an unprocessed node, we stop everything (this should not occur, normally).
        if currentNode.distance == math.huge then
            break
        end

        -- We get the neighbors of the current node
        neighbors = grid.getNeighbors(currentNode)
        for _, neighborNode in ipairs(neighbors) do
            -- We calculate the cost of move from the current node to its neighbor
            costOfMoveToNeighborNode = grid.getMapValue(neighborNode.x, neighborNode.y)
            distanceToNeighborNode = grid.distance(currentNode, neighborNode, costOfMoveToNeighborNode)
            alt = currentNode.distance + distanceToNeighborNode
            -- We relax the edge (currentNode->neighbor) in case there is a better alternative
            if alt < neighborNode.distance then
                neighborNode.distance = alt
                neighborNode.previous = currentNode
                -- If so, as the neighbor node was updated, we sort the list of nodes in increasing distance order)
                table.sort(listOfNodes, function(nodeA, nodeB)
                    return nodeA.distance < nodeB.distance
                end)
            end
        end
    end
end

-- Direction vectors will be used to retrieve the neighbors of a given node
cardinalVectors = {{
    x = 0,
    y = -1
}, {
    x = -1,
    y = 0
}, {
    x = 1,
    y = 0
}, {
    x = 0,
    y = 1
}}
diagonalVectors = {{
    x = -1,
    y = -1
}, {
    x = 1,
    y = -1
}, {
    x = -1,
    y = 1
}, {
    x = 1,
    y = 1
}}

-- Gridmap handler template
-- The passable field should be provided. It acts as a validation function 
-- that checks if a value on the weighted map should be considered as passable or not.
handler = {
    map = nil, -- This will serves as a reference to the game map (weighted map)   
    nodes = {}, -- 2D array of nodes (see node.lua)
    diagonal = false, -- Specifies if diagonal moves are allowed (not allowed by default)
    passable = nil -- to be implemented.
}

-- Inits a gridmap handler. We pass it a weighted map,
-- and it processes this map to create the number of nodes needed.
function handler.create(map)
    handler.map = map
    handler.nodes = {}
    for y, row in ipairs(map) do
        handler.nodes[y] = {}
        for x in ipairs(row) do
            handler.nodes[y][x] = createNode(x, y)
        end
    end
end

-- Returns an array list of all the nodes
function handler.getAllNodes()
    listOfNodes = {}
    for y, row in ipairs(handler.nodes) do
        for x, node in ipairs(row) do
            table.insert(listOfNodes, node)
        end
    end
    return listOfNodes
end

-- Returns the map value at coordinate pair (x,y)
-- Returns nil in case (x,y) is not a valid pair
function handler.getMapValue(x, y)
    return handler.map[y] and handler.map[y][x]
end

-- Returns the node at coordinate pair (x,y)
-- Returns nil in case (x,y) is not a valid pair
function handler.getNode(x, y)
    return handler.nodes[y] and handler.nodes[y][x]
end

-- Checks if (x,y) is a valid pair of coordinates and if
-- mapValue(x,y) is passable.
function handler.isPassableNode(x, y)
    mapValue = handler.getMapValue(x, y)
    if mapValue then
        return handler.passable(mapValue)
    end
    return false
end

-- Returns Manhattan distance between nodes a and node b
-- This should be the heuristic of choice for cardinal grids.
function handler.calculateManhattanDistance(a, b, costOfMove)
    dx, dy = a.x - b.x, a.y - b.y
    return (costOfMove or 1) * (math.abs(dx) + math.abs(dy))
end

-- Returns diagonal distance between node a and node b
-- This should be the heuristic of choice for octal grids.
function handler.calculateDiagonalDistance(a, b, costOfMove)
    dx, dy = math.abs(a.x - b.x), math.abs(a.y - b.y)
    return (costOfMove or 1) * math.max(dx, dy)
end

-- Returns an array-list of neighbors of node n.
function handler.getNeighbors(n)
    neighbors = {}
    -- Gets the list of cardinal passable neighbors
    for _, axis in ipairs(cardinalVectors) do
        x, y = n.x + axis.x, n.y + axis.y
        if handler.isPassableNode(x, y) then
            table.insert(neighbors, handler.getNode(x, y))
        end
    end
    -- In case diagonal movement is allowed
    if handler.diagonal then
        -- Adds also adjacent passable neighbors
        for _, axis in ipairs(diagonalVectors) do
            x, y = n.x + axis.x, n.y + axis.y
            if handler.isPassableNode(x, y) then
                table.insert(neighbors, handler.getNode(x, y))
            end
        end
    end
    return neighbors
end

-- Returns a path from start node to goal node which does not exceed
-- a maximumCost. This function reads the graph actual state after it
-- has been processed by Dijkstra. As such, it goes from the start nodes
-- and walks towards the goal node while counting the cost of the total
-- move. In case the actualCost of move exceeds to maximumCost allowed,
-- it stops and returns an incomplete path. In case one wants the full path,
-- maximumCost should be omitted, thus will fallback to infinity.
function handler.findPath(start, goal, maximumCost)
    path = {{
        x = start.x,
        y = start.y
    }} -- add the start node to the path
    previous, oldPrevious = start

    actualCost = 0
    maximumCost = maximumCost or math.huge

    -- Let us backtrack the path by moving downhill the distance map.
    repeat
        oldPrevious = previous
        previous = previous.previous
        -- we calculate the cost of move of a single step
        costOfMove = (oldPrevious.distance - previous.distance)
        -- in case we cannot afford it, we stop and return an incomplete path
        if actualCost + costOfMove > maximumCost then
            break
        else
            -- otherwise, we increase the actual cost, and register the step in the path
            actualCost = actualCost + costOfMove
            table.insert(path, {
                x = previous.x,
                y = previous.y
            })
        end
    until previous == goal -- stop backtracking when we have reached the goal.
    
    -- we return the path, plus the total cost.
    return path, actualCost
end

function createNode(x, y, dist)
    newNode = {
        x = x, -- the x coordinate of the node
        y = y, -- the y-coordinate of the node
        distance = dist or 0 -- the distance (will be calculated by the search algorithm)
    }
    return newNode
end

function recalculate()
    if #players < 1 then
        return
    end
    exits = get_entities_by_type(ENT_TYPE.FLOOR_DOOR_EXIT)
    if #exits < 1 then
        return
    end
    tiles = get_entities_by(0, 0x180, 0)
    board = {}
    xmin, ymin, xmax, ymax = get_bounds()
    for y = ymin - 0.5, ymax + 0.5, -1 do
        row = {}
        for x = xmin + 0.5, xmax - 0.5, 1 do
            row[#row + 1] = 1
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
                cost = 200
                if ent.type.id == ENT_TYPE.FLOOR_PIPE or ent.type.search_flags == 0x80 then
                    cost = 5
                end
                if tx >= 1 and tx <= #board[1] and ty >= 1 and ty <= #board then
                    board[ty][tx] = cost
                end
            end
        end
    end
    handler.create(board)
    handler.passable = function(value) return true end
    handler.distance = handler.calculateManhattanDistance
    ex, ey, el = get_position(exits[1])
    tox = math.floor(ex - xmin + 1)
    toy = math.floor(ymin - ey + 1)
    target = handler.getNode(tox, toy)    
    dijkstra(handler, target)
end

set_callback(function()
    exitpath = {}
    handler.map = nil
    handler.nodes = {}
    recalculate()
    set_interval(function()
        tiles = get_entities_by(0, 0x180, 0)
        if ntiles ~= #tiles and options.update then
            ntiles = #tiles
            recalculate()
        end
        x, y, l = get_position(players[1].uid)
        fromx = math.floor(x - xmin + 1)
        fromy = math.floor(ymin - y + 1)
        start = handler.getNode(fromx, fromy)
        exitpath, cost = handler.findPath(start, target)
    end, 5)
end, ON.LEVEL)

set_callback(function(draw_ctx)
    if state.screen ~= 12 then return end
    if exitpath ~= nil and #exitpath > 1 then
        xmin, ymin, xmax, ymax = get_bounds()
        x, y, l = get_position(players[1].uid)
        lastx, lasty = screen_position(x, y)
        for i, v in ipairs(exitpath) do
            sx, sy = screen_position(xmin + v.x - 0.5, ymin - v.y + 0.5)
            draw_ctx:draw_line(lastx, lasty, sx, sy, 2, rgba(0, 255, 0, 255))
            lastx = sx
            lasty = sy
        end
    end
end, ON.GUIFRAME)
