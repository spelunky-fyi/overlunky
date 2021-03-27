meta.name = "Projectile Randomizer"
meta.version = "WIP"
meta.description = "Replace projectiles with... something."
meta.author = "Dregu"

from = {ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_FREEZERAYSHOT, ENT_TYPE.ITEM_CLONEGUNSHOT, ENT_TYPE.ITEM_UFO_LASER_SHOT,
        ENT_TYPE.ITEM_LAMASSU_LASER_SHOT, ENT_TYPE.ITEM_SORCERESS_DAGGER_SHOT, ENT_TYPE.ITEM_LASERTRAP_SHOT,
        ENT_TYPE.ITEM_SCEPTER_PLAYERSHOT, ENT_TYPE.ITEM_FIREBALL, ENT_TYPE.ITEM_HUNDUN_FIREBALL, ENT_TYPE.ITEM_ACIDSPIT,
        ENT_TYPE.ITEM_INKSPIT, ENT_TYPE.ITEM_WOODEN_ARROW}
to = {ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_EGGPLANT, ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_UFO_LASER_SHOT,
      ENT_TYPE.ITEM_LAMASSU_LASER_SHOT, ENT_TYPE.ITEM_SORCERESS_DAGGER_SHOT, ENT_TYPE.ITEM_LASERTRAP_SHOT,
      ENT_TYPE.ITEM_FIREBALL, ENT_TYPE.ITEM_HUNDUN_FIREBALL, ENT_TYPE.ITEM_ACIDSPIT, ENT_TYPE.ITEM_INKSPIT,
      ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_NUGGET_SMALL, ENT_TYPE.ITEM_SKULL}
done = {}

function replaced(id)
    for i, v in ipairs(done) do
        if v == id then
            return true
        end
    end
    return false
end

function replace_projectiles()
    ents = get_entities_by_type(from)
    for i, ent in ipairs(ents) do
        if not replaced(ent) then
            x, y, l = get_position(ent)
            e = get_entity(ent):as_movable()
            vx = e.velocityx
            vy = e.velocityy
            kill_entity(ent)
            newent = to[math.random(#to)]
            newid = spawn(newent, x, y, l, vx, vy)
            done[#done + 1] = newid
        end
    end
end

set_callback(function()
    done = {}
end, ON.LEVEL)

set_callback(function()
    replace_projectiles()
end, ON.FRAME)
