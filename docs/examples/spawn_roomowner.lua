-- spawns waddler selling pets
-- all the aggro etc mechanics from a normal shop still apply
local x, y, l = get_position(get_player(1).uid)
local owner = spawn_roomowner(ENT_TYPE.MONS_STORAGEGUY, x+1, y, l, ROOM_TEMPLATE.SHOP)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_DOG, x-1, y, l), owner)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_CAT, x-2, y, l), owner)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_HAMSTER, x-3, y, l), owner)
