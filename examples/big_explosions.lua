meta.name = "Big explosions"
meta.version = "WIP"
meta.description = "Makes huge explosions"
meta.author = "Zappatic"

-- make it so players aren't affected by explosions
set_explosion_mask(MASK.MOUNT | MASK.MONSTER | MASK.ITEM | MASK.ACTIVEFLOOR | MASK.FLOOR)

-- enlarge the hitboxes of the explosion entities, which increases the destruction radius
set_post_entity_spawn(function(ent)
    ent.hitboxx = 10
    ent.hitboxy = 10
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.FX_POWEREDEXPLOSION)

set_post_entity_spawn(function(ent)
    ent.hitboxx = 10
    ent.hitboxy = 10
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.FX_EXPLOSION)

-- make the bombs look bigger
set_post_entity_spawn(function(ent)
    ent.scale_hor = 5
    ent.scale_ver = 5
end, SPAWN_TYPE.ANY, 0, ENT_TYPE.ITEM_BOMB)
