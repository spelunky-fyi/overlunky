-- find all cavemen and give them bombs
-- using a type and mask in get_entities_by speeds up the search, cause the api knows which bucket to search in
for i,uid in ipairs(get_entities_by(ENT_TYPE.MONS_CAVEMAN, MASK.MONSTER, LAYER.BOTH)) do
    local x, y, l = get_position(uid)
    spawn_entity_snapped_to_floor(ENT_TYPE.ITEM_BOMB, x, y, l)
end
