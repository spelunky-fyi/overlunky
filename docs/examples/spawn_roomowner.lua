-- spawns waddler selling pets
-- all the aggro etc mechanics from a normal shop still apply
local x, y, l = get_position(get_player(1).uid)
local owner = spawn_roomowner(ENT_TYPE.MONS_STORAGEGUY, x+1, y, l, ROOM_TEMPLATE.SHOP)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_DOG, x-1, y, l), owner)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_CAT, x-2, y, l), owner)
add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_HAMSTER, x-3, y, l), owner)

-- use in a tile code to add shops to custom levels
-- this may spawn some shop related decorations too
define_tile_code("pet_shop_boys")
set_pre_tile_code_callback(function(x, y, layer)
    local owner = spawn_roomowner(ENT_TYPE.MONS_YANG, x, y, layer, ROOM_TEMPLATE.SHOP)
    -- another dude for the meme, this has nothing to do with the shop
    spawn_on_floor(ENT_TYPE.MONS_BODYGUARD, x+1, y, layer)
    add_item_to_shop(spawn_on_floor(ENT_TYPE.MONS_PET_HAMSTER, x+2, y, layer), owner)
    return true
end, "pet_shop_boys")
