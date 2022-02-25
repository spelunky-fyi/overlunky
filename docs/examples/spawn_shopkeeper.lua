-- spawns a shopkeeper selling a shotgun next to you
-- converts the current room to a ROOM_TEMPLATE.SHOP with shop music and zoom effect
local x, y, l = get_position(get_player(1).uid)
local owner = spawn_shopkeeper(x+1, y, l)
add_item_to_shop(spawn_on_floor(ENT_TYPE.ITEM_SHOTGUN, x-1, y, l), owner)

-- spawns a shopkeeper selling a puppy next to you
-- also converts the room to a shop, but after the shopkeeper is spawned
-- this enables the safe zone for moving items, but disables shop music and zoom for whatever reason
local x, y, l = get_position(get_player(1).uid)
local owner = spawn_shopkeeper(x+1, y, l, ROOM_TEMPLATE.SIDE)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_DOG, x-1, y, l), owner)
local ctx = PostRoomGenerationContext:new()
local rx, ry = get_room_index(x, y)
ctx:set_room_template(rx, ry, l, ROOM_TEMPLATE.SHOP)
