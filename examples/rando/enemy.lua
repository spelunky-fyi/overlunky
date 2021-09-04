local module = {}
enemy_cbs = {}

small_from = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_HANGSPIDER, ENT_TYPE.MONS_BAT,
              ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
              ENT_TYPE.MONS_MANTRAP, ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_WITCHDOCTOR, ENT_TYPE.MONS_MOSQUITO,
              ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.MONS_ROBOT, ENT_TYPE.MONS_FIREBUG,
              ENT_TYPE.MONS_FIREBUG_UNCHAINED, ENT_TYPE.MONS_IMP, ENT_TYPE.MONS_VAMPIRE, ENT_TYPE.MONS_VLAD,
              ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS, ENT_TYPE.MONS_CATMUMMY,
              ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI, ENT_TYPE.MONS_FISH,
              ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_UFO, ENT_TYPE.MONS_ALIEN,
              ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER, ENT_TYPE.MONS_SHOPKEEPERCLONE,
              ENT_TYPE.MONS_OLMITE_HELMET, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED,
              ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_AMMIT, ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG, ENT_TYPE.MONS_GRUB,
              ENT_TYPE.MONS_JUMPDOG, ENT_TYPE.MONS_EGGPLANT_MINISTER, ENT_TYPE.MONS_LEPRECHAUN}
small_to = {ENT_TYPE.MONS_SNAKE, ENT_TYPE.MONS_SPIDER, ENT_TYPE.MONS_HANGSPIDER, ENT_TYPE.MONS_BAT,
            ENT_TYPE.MONS_CAVEMAN, ENT_TYPE.MONS_SKELETON, ENT_TYPE.MONS_SCORPION, ENT_TYPE.MONS_HORNEDLIZARD,
            ENT_TYPE.MONS_MOLE, ENT_TYPE.MONS_MANTRAP, ENT_TYPE.MONS_TIKIMAN, ENT_TYPE.MONS_WITCHDOCTOR,
            ENT_TYPE.MONS_MOSQUITO, ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.MONS_ROBOT,
            ENT_TYPE.MONS_FIREBUG, ENT_TYPE.MONS_FIREBUG_UNCHAINED, ENT_TYPE.MONS_IMP, ENT_TYPE.MONS_VAMPIRE,
            ENT_TYPE.MONS_VLAD, ENT_TYPE.MONS_CROCMAN, ENT_TYPE.MONS_COBRA, ENT_TYPE.MONS_SORCERESS,
            ENT_TYPE.MONS_CATMUMMY, ENT_TYPE.MONS_NECROMANCER, ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI,
            ENT_TYPE.MONS_FISH, ENT_TYPE.MONS_OCTOPUS, ENT_TYPE.MONS_HERMITCRAB, ENT_TYPE.MONS_UFO, ENT_TYPE.MONS_ALIEN,
            ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_PROTOSHOPKEEPER, ENT_TYPE.MONS_SHOPKEEPERCLONE,
            ENT_TYPE.MONS_OLMITE_HELMET, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_OLMITE_NAKED,
            ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_AMMIT, ENT_TYPE.MONS_FROG, ENT_TYPE.MONS_FIREFROG, ENT_TYPE.MONS_GRUB,
            ENT_TYPE.MONS_JUMPDOG, ENT_TYPE.MONS_SCARAB, ENT_TYPE.MONS_LEPRECHAUN, ENT_TYPE.MOUNT_TURKEY,
            ENT_TYPE.MOUNT_ROCKDOG, ENT_TYPE.MOUNT_AXOLOTL}
big_from = {ENT_TYPE.MONS_CAVEMAN_BOSS, ENT_TYPE.MONS_LAVAMANDER, ENT_TYPE.MONS_MUMMY, ENT_TYPE.MONS_ANUBIS,
            ENT_TYPE.MONS_GIANTFISH, ENT_TYPE.MONS_YETIKING, ENT_TYPE.MONS_YETIQUEEN, ENT_TYPE.MONS_ALIENQUEEN,
            ENT_TYPE.MONS_LAMASSU, ENT_TYPE.MONS_QUEENBEE, ENT_TYPE.MONS_GIANTFLY, ENT_TYPE.MONS_CRABMAN,
            ENT_TYPE.MOUNT_MECH}
big_to = {ENT_TYPE.MONS_CAVEMAN_BOSS, ENT_TYPE.MONS_LAVAMANDER, ENT_TYPE.MONS_MUMMY, ENT_TYPE.MONS_ANUBIS,
          ENT_TYPE.MONS_GIANTFISH, ENT_TYPE.MONS_YETIKING, ENT_TYPE.MONS_YETIQUEEN, ENT_TYPE.MONS_ALIENQUEEN,
          ENT_TYPE.MONS_LAMASSU, ENT_TYPE.MONS_QUEENBEE, ENT_TYPE.MONS_GIANTFLY, ENT_TYPE.MONS_CRABMAN,
          ENT_TYPE.MOUNT_MECH}
olmec_ammo = {ENT_TYPE.ACTIVEFLOOR_PUSHBLOCK, ENT_TYPE.ITEM_CHEST, ENT_TYPE.ITEM_POT, ENT_TYPE.ITEM_BOMB,
              ENT_TYPE.ITEM_PASTEBOMB, ENT_TYPE.ITEM_EGGPLANT, ENT_TYPE.ITEM_TV, ENT_TYPE.ITEM_PUNISHBALL,
              ENT_TYPE.ITEM_LANDMINE, ENT_TYPE.ITEM_SCRAP, ENT_TYPE.MONS_MAGMAMAN, ENT_TYPE.ACTIVEFLOOR_POWDERKEG}
kingu_ammo = {ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI, ENT_TYPE.MONS_OCTOPUS,
              ENT_TYPE.ACTIVEFLOOR_POWDERKEG, ENT_TYPE.MONS_YETI, ENT_TYPE.MONS_VAMPIRE, ENT_TYPE.MONS_LEPRECHAUN,
              ENT_TYPE.MONS_BEE, ENT_TYPE.MONS_OLMITE_BODYARMORED, ENT_TYPE.MONS_MONKEY, ENT_TYPE.MONS_PROTOSHOPKEEPER}
enemy_done = {}

function enemy_replaced(id)
    for i, v in pairs(enemy_done) do
        if v == id then
            return true
        end
    end
    return false
end

function move_or_kill(v)
    e = get_entity(v)
    if e.health == 0 then
        return
    end
    if state.theme == THEME.COSMIC_OCEAN then
        -- message("Killing "..tostring(v).." "..tostring(e.type.id))
        move_entity(v, 20, 120 - ((state.height) * 8) + 4, 0, 0)
        kill_entity(v)
    else
        -- message("Moving "..tostring(v).." "..tostring(e.type.id))
        move_entity(v, 20, 120 - ((state.height + 1) * 8), 0, 0)
    end
end

function create_rider(id, type, x, y, l)
    rid = spawn(ENT_TYPE.MONS_CAVEMAN, x, y, l, 0, 0)
    carry(id, rid)
end

function replace_enemy(id, from)
    x, y, l = get_position(id)
    if x == 0 or (l == LAYER.BACK and state.theme ~= THEME.ICE_CAVES and state.theme ~= THEME.OLMEC) then
        return
    end
    move_or_kill(id)
    newid = from[math.random(#from)]
    if l == LAYER.BACK and state.theme == THEME.OLMEC then
        newid = ENT_TYPE.MONS_CAVEMAN_BOSS
    end
    new = spawn(newid, x, y, l, 0, 0)
    if newid >= ENT_TYPE.MOUNT_TURKEY then
        create_rider(new, newid, x, y, l)
    end
end

function replace_projectile(id, from)
    local x, y, l = get_position(id)
    local ent = get_entity(id)
    local vx = ent.velocityx
    local vy = ent.velocityy
    move_entity(id, 0, 0, 0, 0)
    local new = from[math.random(#from)]
    local newid = spawn(new, x, y, l, vx, vy)
    enemy_done[#enemy_done + 1] = newid
end

function replace_kingu(id, from)
    local x, y, l = get_position(id)
    local ent = get_entity(id)
    local vx = ent.velocityx
    local vy = ent.velocityy
    local new = from[math.random(#from)]
    local newid = spawn(new, x + math.random() * 4 - 2, y, l, vx, vy)
    local newent = get_entity(newid)
    newent.velocityx = math.random() * 0.2 - 0.1
    newent.velocityy = math.random() * 0.25
    enemy_done[#enemy_done + 1] = newid
end

function module.start()
    enemy_cbs[#enemy_cbs+1] = set_callback(function()
        enemy_done = {}
        set_timeout(function()
            mons_small = get_entities_by_type(small_from)
            mons_big = get_entities_by_type(big_from)
            to = 0
            for i, v in ipairs(mons_small) do
                set_timeout(function()
                    replace_enemy(v, small_to)
                end, to)
                to = to + 1
            end

            for i, v in ipairs(mons_big) do
                set_timeout(function()
                    replace_enemy(v, big_to)
                end, to)
                to = to + 1
            end
        end, 5)

        if state.theme == THEME.OLMEC then
            set_interval(function()
                local ufos = get_entities_by_type(ENT_TYPE.MONS_UFO)
                for i, v in ipairs(ufos) do
                    if not enemy_replaced(v) then
                        replace_enemy(v, small_to)
                    end
                    enemy_done[#enemy_done + 1] = v
                end
                ox = 0
                oy = 0
                local olmecs = get_entities_by_type(ENT_TYPE.ACTIVEFLOOR_OLMEC)
                for i, v in ipairs(olmecs) do
                    ox, oy, l = get_position(v)
                end
                local bombs = get_entities_by_type(ENT_TYPE.ITEM_BOMB)
                for i, v in ipairs(bombs) do
                    x, y, l = get_position(v)
                    ent = get_entity(v)
                    if math.abs(y - oy) < 1 and math.abs(x - ox) < 2.5 and not enemy_replaced(v) then
                        replace_projectile(v, olmec_ammo)
                        enemy_done[#enemy_done + 1] = v
                    end
                end
            end, 1)
        end

        if state.theme == THEME.ABZU then
            set_interval(function()
                local enes = get_entities_by_type(ENT_TYPE.MONS_JIANGSHI, ENT_TYPE.MONS_FEMALE_JIANGSHI, ENT_TYPE.MONS_OCTOPUS)
                for i, v in ipairs(enes) do
                    if not enemy_replaced(v) then
                        replace_kingu(v, kingu_ammo)
                        enemy_done[#enemy_done + 1] = v
                    end
                end
            end, 1)
        end

        set_interval(function()
            local skeles = get_entities_by_type(ENT_TYPE.MONS_REDSKELETON)
            for i, v in ipairs(skeles) do
                if not enemy_replaced(v) then
                    replace_enemy(v, small_to)
                    enemy_done[#enemy_done + 1] = v
                end
            end
        end, 1)
    end, ON.LEVEL)
end

function module.stop()
    for i,v in ipairs(enemy_cbs) do
        clear_callback(v)
    end
    enemy_cbs = {}
end

return module
