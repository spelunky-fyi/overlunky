meta = {
    name = 'Weapon Of Choice',
    description = [[Replace your whip with anything, but that is the only weapon you can swing, shoot or throw, besides bombs and ropes. Unless you choose bombs or ropes, then that's the only thing you can use.

Your weapon won't take damage and won't damage you. You can pick up your weapon and can carry it through all doors. Don't lose your weapon or you're screwed, unless you find a new one!

There are a few completely useless items in case you want to do some 'carry a useless item' challenge.]],
    version = '1.0',
    author = 'Dregu'
}

weapons = {
    'MONS_FISH',
    'MONS_CAVEMAN',
    'MONS_HORNEDLIZARD',
    'MONS_MANTRAP',
    'MONS_TIKIMAN',
    'MONS_WITCHDOCTOR',
    'MONS_MAGMAMAN',
    'MONS_CROCMAN',
    'MONS_SORCERESS',
    'MONS_NECROMANCER',
    'MONS_OCTOPUS',
    'MONS_HERMITCRAB',
    'MONS_YETI',
    'MONS_PROTOSHOPKEEPER',
    'MONS_SHOPKEEPERCLONE',
    'MONS_OLMITE_NAKED',
    'MONS_JUMPDOG',
    'MONS_SHOPKEEPER',
    'MONS_MERCHANT',
    'MONS_YANG',
    'MONS_MARLA_TUNNEL',
    'MONS_SISTER_PARSLEY',
    'MONS_SISTER_PARSNIP',
    'MONS_SISTER_PARMESAN',
    'MONS_OLD_HUNTER',
    'MONS_THIEF',
    'MONS_BODYGUARD',
    'MONS_HUNDUNS_SERVANT',
    'MONS_LEPRECHAUN',
    'MONS_PET_DOG',
    'MONS_PET_CAT',
    'MONS_PET_HAMSTER',
    'ITEM_BOMB',
    'ITEM_PASTEBOMB',
    'ITEM_ROPE',
    'ITEM_IDOL',
    'ITEM_MADAMETUSK_IDOL',
    'ITEM_ROCK',
    'ITEM_WOODEN_ARROW',
    'ITEM_METAL_ARROW',
    'ITEM_LIGHT_ARROW',
    'ITEM_CRABMAN_CLAW',
    'ITEM_CHEST',
    'ITEM_KEY',
    'ITEM_LOCKEDCHEST',
    'ITEM_LOCKEDCHEST_KEY',
    'ITEM_CRATE',
    'ITEM_BOOMBOX',
    'ITEM_TV',
    'ITEM_TORCH',
    'ITEM_LAMP',
    'ITEM_PRESENT',
    'ITEM_PUNISHBALL',
    'ITEM_LANDMINE',
    'ITEM_USHABTI',
    'ITEM_DIE',
    'ITEM_POTOFGOLD',
    'ITEM_SNAP_TRAP',
    'ITEM_POT',
    'ITEM_CURSEDPOT',
    'ITEM_SKULL',
    'ITEM_LAVAPOT',
    'ITEM_SCRAP',
    'ITEM_EGGPLANT',
    'ITEM_WEBGUN',
    'ITEM_SHOTGUN',
    'ITEM_FREEZERAY',
    'ITEM_CROSSBOW',
    'ITEM_CAMERA',
    'ITEM_TELEPORTER',
    'ITEM_MATTOCK',
    'ITEM_BOOMERANG',
    'ITEM_MACHETE',
    'ITEM_EXCALIBUR',
    'ITEM_BROKENEXCALIBUR',
    'ITEM_PLASMACANNON',
    'ITEM_SCEPTER',
    'ITEM_CLONEGUN',
    'ITEM_HOUYIBOW',
    'ITEM_WOODEN_SHIELD',
    'ITEM_METAL_SHIELD',
    'MOUNT_TURKEY',
    'MOUNT_ROCKDOG',
    'MOUNT_AXOLOTL',
    'MOUNT_QILIN'
}

weapon_options = table.concat(weapons, '\0') .. '\0\0'

register_option_combo('aweapon', 'Weapon of Choice', weapon_options, 1)
register_option_bool('shiny', 'Shiny weapon', 'They even glow in the dark, harder to lose!', true)
register_option_bool('corpse', 'Dead weapon', 'Your weapon will be a dead monster when applicaple, instead of stunned.', false)
register_option_bool('switch', 'Switch weapon', 'Allow switching weapons by carrying something else to the exit.', false)
register_option_bool('brandom', 'Random weapon per run', 'Get a different random weapon every run!', false)
register_option_bool('brandom2', 'Random weapon per level', 'Get a different random weapon every level!', false)
register_option_bool('unbreak', 'Unbreakable weapon', 'Your weapon won\'t get damaged by most things.', true)

WEAPON = ENT_TYPE.MONS_FISH
WEAPON_UID = -1

function get_ushabti_frame()
    return prng:random_int(0, 9, PRNG_CLASS.LEVEL_DECO) + prng:random_int(0, 9, PRNG_CLASS.LEVEL_DECO) * 12
end

set_callback(function()
    if state.level_count == 0 or options.brandom2 then
        if options.brandom or options.brandom2 then
            options.aweapon = prng:random_index(#weapons, PRNG_CLASS.LEVEL_DECO)
        end
        WEAPON = ENT_TYPE[weapons[options.aweapon]]
        if WEAPON == ENT_TYPE.ITEM_PASTEBOMB then
            state.items.player_inventory[1].acquired_powerups[1] = ENT_TYPE.ITEM_POWERUP_PASTE
        end
        if get_type(WEAPON).search_flags == MASK.MOUNT then
            state.items.player_inventory[1].mount_type = WEAPON
            state.items.player_inventory[1].mount_metadata = 0x4000
        elseif WEAPON ~= ENT_TYPE.ITEM_ROPE and WEAPON ~= ENT_TYPE.ITEM_BOMB and WEAPON ~= ENT_TYPE.ITEM_PASTEBOMB then
            state.items.player_inventory[1].held_item = WEAPON
            if WEAPON == ENT_TYPE.ITEM_USHABTI then
                state.items.player_inventory[1].held_item_metadata = get_ushabti_frame()
            end
        end
    end
    if HOLD then
        state.items.player_inventory[1].held_item = HOLD
        HOLD = nil
    end
    WEAPON_UID = -1
end, ON.PRE_LEVEL_GENERATION)

set_post_entity_spawn(function(p)
    if WEAPON == ENT_TYPE.MONS_MANTRAP or WEAPON == ENT_TYPE.ITEM_SNAP_TRAP then
        p:set_pre_kill(function(_, _, killer)
            if killer and killer.uid == WEAPON_UID then
                return true
            end
        end)
    end
end, SPAWN_TYPE.ANY, MASK.PLAYER)

function hook_weapon(ent)
    if options.shiny then
        ent.rendering_info.shader = 18
    end
    ent.type.properties_flags = set_flag(ent.type.properties_flags, 18)
    ent.flags = set_flag(ent.flags, ENT_FLAG.LOCKED)
    ent.flags = set_flag(ent.flags, ENT_FLAG.THROWABLE_OR_KNOCKBACKABLE)
    ent.flags = set_flag(ent.flags, ENT_FLAG.PICKUPABLE)
    ent.flags = set_flag(ent.flags, ENT_FLAG.PASSES_THROUGH_PLAYER)
    ent:set_post_update_state_machine(function(ent)
        ent.flags = set_flag(ent.flags, ENT_FLAG.PICKUPABLE)
    end)
    if WEAPON == ENT_TYPE.ITEM_CLONEGUN and options.unbreak then
        ent:set_post_update_state_machine(function(ent)
            ent.shots2 = 0
            ent.animation_frame = 152
        end)
    elseif WEAPON == ENT_TYPE.ITEM_MATTOCK and options.unbreak then
        ent:set_post_update_state_machine(function(ent)
            ent.remaining = 255
        end)
    elseif WEAPON == ENT_TYPE.MONS_MAGMAMAN then
        ent:set_post_update_state_machine(function(ent)
            ent.alive_timer = 60
        end)
    elseif WEAPON == ENT_TYPE.ITEM_LANDMINE then
        ent:set_post_destroy(function(ent)
            if ent.timer >= 56 then
                local x, y, l = get_position(ent.uid)
                x = x+(prng:random()*0.1-0.05)
                hook_weapon(get_entity(spawn_critical(ENT_TYPE.ITEM_LANDMINE, x, y, l, 0, 0)))
            end
        end)
    end
    if get_type(WEAPON).search_flags ~= MASK.MOUNT and options.unbreak then
        ent.flags = set_flag(ent.flags, ENT_FLAG.TAKE_NO_DAMAGE)
    end
    if options.unbreak and WEAPON ~= ENT_TYPE.ITEM_IDOL and WEAPON ~= ENT_TYPE.ITEM_MADAMETUSK_IDOL then
        ent:set_pre_kill(function() return true end)
        ent:set_post_update_state_machine(function(ent)
            ent.health = 99
        end)
    end
    ent:set_post_update_state_machine(function(ent)
        if ent.state == CHAR_STATE.STUNNED and ent.overlay and ent.overlay.type.search_flags == MASK.PLAYER then
            ent.stun_timer = 90
        end
    end)
    if ent.type.id == ENT_TYPE.MONS_HORNEDLIZARD then
        ent:set_behavior(18)
    elseif ent.type.id == ENT_TYPE.ITEM_DIE then
        ent:set_post_update_state_machine(function(ent)
            ent.animation_frame = prng:random_int(245, 250, PRNG_CLASS.LEVEL_DECO)
            clear_callback()
        end)
    elseif ent.type.search_flags == MASK.MONSTER and ent.type.leaves_corpse_behind then
        if options.corpse then
            ent:set_behavior(22)
            ent.state = 22
            ent.flags = set_flag(ent.flags, ENT_FLAG.DEAD)
            ent.health = 0
        else
            ent:set_behavior(18)
            ent.state = 18
        end
    end
    spawn_over(ENT_TYPE.FX_COMPASS, ent.uid, 0, 0)
    WEAPON_UID = ent.uid
end

set_post_entity_spawn(function(ent)
    if ent.type.id ~= WEAPON then return end
    if WEAPON == ENT_TYPE.ITEM_BOMB or WEAPON == ENT_TYPE.ITEM_ROPE then return end
    if ((ent.x == 0 and ent.y == 0) or (math.floor(ent.x) == math.floor(state.level_gen.spawn_x) and math.floor(ent.y) == math.floor(state.level_gen.spawn_y))) and state.time_level < 2 then
        hook_weapon(ent)
    end
end, SPAWN_TYPE.ANY, MASK.ANY)

function set_weapon(t)
    if options.switch then
        WEAPON = t
    end
end

set_callback(function()
    if state.screen == SCREEN.TRANSITION and state.screen_next == SCREEN.LEVEL then
        if state.items.player_inventory[1].collected_money_count > 0 then
            for i=1,state.items.player_inventory[1].collected_money_count do
                local item = state.items.player_inventory[1].collected_money[i]
                if WEAPON == item then
                    HOLD = item
                end
                set_weapon(item)
            end
        end
        if players[1].holding_uid ~= -1 then
            set_weapon(get_entity(players[1].holding_uid).type.id)
        elseif players[1]:topmost_mount().type.search_flags == MASK.MOUNT then
            set_weapon(players[1]:topmost_mount().type.id)
        elseif state.items.saved_pets_count > 0 then
            for i=1,state.items.saved_pets_count do
                if WEAPON == state.items.saved_pets[i] then
                    HOLD = state.items.saved_pets[i]
                end
            end
            if not HOLD then
                set_weapon(state.items.saved_pets[1])
                HOLD = state.items.saved_pets[1]
            end
        end
    end
end, ON.PRE_LOAD_SCREEN)

set_callback(function()
    if not players[1] then return end
    if players[1].state ~= CHAR_STATE.DUCKING and (players[1].holding_uid == -1 or players[1].holding_uid ~= WEAPON_UID) and (players[1]:topmost_mount().uid ~= WEAPON_UID or (players[1].holding_uid ~= -1 and (not test_mask(players[1].input.buttons_gameplay, INPUTS.DOWN) or players[1]:topmost_mount().standing_on_uid == -1))) then
        players[1].input.buttons_gameplay = clr_mask(players[1].input.buttons_gameplay, INPUTS.WHIP)
    end
    if WEAPON == ENT_TYPE.ITEM_BOMB or WEAPON == ENT_TYPE.ITEM_PASTEBOMB then
        players[1].inventory.bombs = 99
        players[1].inventory.ropes = 0
    end
    if WEAPON == ENT_TYPE.ITEM_ROPE then
        players[1].inventory.bombs = 0
        players[1].inventory.ropes = 99
    end
end, ON.PRE_UPDATE)
