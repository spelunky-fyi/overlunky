meta= {
    name = "Spawn Hooks",
    description = "Examples for how to hook entity spawns.",
    author = "Malacath"
}

-- Glitch player and monster animations
set_post_entity_spawn(function(entity)
    set_post_statemachine(entity.uid, function(movable)
        if math.random(0, 10) == 0 then
            movable.animation_frame = movable.animation_frame + math.random(-3, 3)
        end
    end)
end, SPAWN_TYPE.ANY, MASK.MONSTER | MASK.PLAYER)

-- Replace some monsters with mounts
set_pre_entity_spawn(function(ent_type, x, y, l, overlay)
    return spawn(ENT_TYPE.MOUNT_ROCKDOG, x, y, l, 0, 0)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_SNAKE)
set_pre_entity_spawn(function(ent_type, x, y, l, overlay)
    return spawn(ENT_TYPE.MOUNT_TURKEY, x, y, l, 0, 0)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_SKELETON)
set_pre_entity_spawn(function(ent_type, x, y, l, overlay)
    return spawn(ENT_TYPE.MOUNT_AXOLOTL, x, y, l, 0, 0)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MONS_CAVEMAN)

-- Tame all turkeys
set_post_entity_spawn(function(entity)
    entity:tame(true)
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.MOUNT_TURKEY)

-- Throw bombs instead of whipping
set_pre_entity_spawn(function(ent_type, x, y, l, overlay)
    return spawn(ENT_TYPE.ITEM_BOMB, x, y, l, 0, 0)
end, SPAWN_TYPE.SYSTEMIC, 0, ENT_TYPE.ITEM_WHIP)
