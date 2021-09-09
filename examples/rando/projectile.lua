local module = {}
projectile_cbs = {}

proj_from = {ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_FREEZERAYSHOT, ENT_TYPE.ITEM_CLONEGUNSHOT, ENT_TYPE.ITEM_UFO_LASER_SHOT,
        ENT_TYPE.ITEM_LAMASSU_LASER_SHOT, ENT_TYPE.ITEM_SORCERESS_DAGGER_SHOT, ENT_TYPE.ITEM_LASERTRAP_SHOT,
        ENT_TYPE.ITEM_SCEPTER_PLAYERSHOT, ENT_TYPE.ITEM_FIREBALL, ENT_TYPE.ITEM_HUNDUN_FIREBALL, ENT_TYPE.ITEM_ACIDSPIT,
        ENT_TYPE.ITEM_INKSPIT, ENT_TYPE.ITEM_WOODEN_ARROW}
proj_to = {ENT_TYPE.ITEM_ROCK, ENT_TYPE.ITEM_EGGPLANT, ENT_TYPE.ITEM_BULLET, ENT_TYPE.ITEM_UFO_LASER_SHOT,
      ENT_TYPE.ITEM_LAMASSU_LASER_SHOT, ENT_TYPE.ITEM_SORCERESS_DAGGER_SHOT, ENT_TYPE.ITEM_LASERTRAP_SHOT,
      ENT_TYPE.ITEM_FIREBALL, ENT_TYPE.ITEM_HUNDUN_FIREBALL, ENT_TYPE.ITEM_ACIDSPIT, ENT_TYPE.ITEM_INKSPIT,
      ENT_TYPE.ITEM_WOODEN_ARROW, ENT_TYPE.ITEM_NUGGET_SMALL, ENT_TYPE.ITEM_SKULL}
projectile_done = {}

function projectile_replaced(id)
    for i, v in pairs(projectile_done) do
        if v == id then
            return true
        end
    end
    return false
end

function replace_projectiles()
    ents = get_entities_by_type(proj_from)
    for i, ent in ipairs(ents) do
        if not projectile_replaced(ent) then
            x, y, l = get_position(ent)
            e = get_entity(ent)
            vx = e.velocityx
            vy = e.velocityy
            kill_entity(ent)
            newent = proj_to[math.random(#proj_to)]
            newid = spawn(newent, x, y, l, vx, vy)
            projectile_done[#projectile_done + 1] = newid
        end
    end
end

function module.start()
    projectile_cbs[#projectile_cbs+1] = set_callback(function()
        projectile_done = {}
    end, ON.LEVEL)

    projectile_cbs[#projectile_cbs+1] = set_callback(function()
        replace_projectiles()
    end, ON.FRAME)
end

function module.stop()
    for i,v in ipairs(projectile_cbs) do
        clear_callback(v)
    end
    projectile_cbs = {}
end

return module
