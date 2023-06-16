meta.name = 'Barrymod'
meta.version = 'WIP'
meta.description = 'Restarts the current level on death or manually like nothing happened. Not everything from Inventory is implemented, cause it\'s not in the api yet. Sometimes also gets the level gen wrong and atm screws up journal progress by design.'
meta.author = 'Dregu'

local status = {}
local restart = false

local vars = {
    state = {'seed', 'level_count', 'time_total', 'shoppie_aggro', 'shoppie_aggro_next', 'merchant_aggro', 'kali_favor', 'kali_status', 'kali_altars_destroyed', 'level_flags', 'quest_flags', 'journal_flags', 'presence_flags', 'special_visibility_flags', 'kills_npc', 'damage_taken', 'time_last_level', 'saved_dogs', 'saved_cats', 'saved_hamsters', 'money_last_levels', 'money_shop_total', 'correct_ushabti'},
    quests = {'yang_state', 'jungle_susters_flags', 'van_horsing_state', 'sparrow_state', 'madame_tusk_state', 'beg_state'},
    inventory = {'health', 'bombs', 'ropes', 'held_item', 'held_item_metadata', 'kapala_blood_amount', 'poison_tick_timer', 'cursed', 'elixir_buff', 'mount_type', 'mount_metadata', 'kills_level', 'kills_total', 'collected_money_total'}
}

local names = {}
for i,v in pairs(ENT_TYPE) do
  names[v] = i
end

local function save(from, arr)
    for i,v in ipairs(arr) do
        status[v] = from[v]
    end
    print("Saved state")
end

local function load(to, arr)
    for i,v in ipairs(arr) do
        if status[v] ~= nil then
            to[v] = status[v]
        end
    end
    print("Loaded state")
end

local function clear()
    status = {}
    status.back = -1
    status.power = {}
    status.rng = {}
    print("Cleared state")
end

local function restart_level()
    restart = true
    state.screen_next = SCREEN.LEVEL
    state.screen_last = SCREEN.TRANSITION
    state.world_next = state.world
    state.level_next = state.level
    state.theme_next = state.theme
    state.quest_flags = 1
    state.loading = 2
end

register_option_button('restart', 'Restart level', function()
    restart_level()
end)

set_callback(function()
    if state.items.player_inventory[1].health < 1 then
        state.items.player_inventory[1].health = 4
    end

    if restart then
        if status.rng then
            for i,v in pairs(status.rng) do
                    prng:set_pair(i, v.a, v.b)
            end
        end
        load(state, vars.state)
        load(state.quests, vars.quests)
        load(state.items.player_inventory[1], vars.inventory)
    else
        if not status.rng then
            status.rng = {}
        end
        for i=0,9 do
            local a,b = prng:get_pair(i)
            status.rng[i] = { a=a, b=b }
        end
        save(state, vars.state)
        save(state.quests, vars.quests)
        save(state.items.player_inventory[1], vars.inventory)
    end
end, ON.PRE_LEVEL_GENERATION)

set_callback(function()
    local ent = players[1]
    if restart then
        if status.power then
            for i,v in ipairs(status.power) do
                local m = string.find(names[v], 'PACK')
                if not m and not ent:has_powerup(v) then
                    ent:give_powerup(v)
                end
            end
        end
        if status.back and status.back ~= -1 and ent:worn_backitem() == -1 then
            pick_up(ent.uid, spawn(status.back, 0, 0, LAYER.PLAYER, 0, 0))
        end
    else
        status.back = -1
        local backitem = worn_backitem(players[1].uid)
        if backitem ~= -1 then
            status.back = get_entity(backitem).type.id
        end

        status.power = {}
        for i,v in ipairs(players[1]:get_powerups()) do
            status.power[i] = v
        end
    end

    set_on_kill(ent.uid, function()
        restart_level()
    end)

    set_on_destroy(ent.uid, function()
        restart_level()
    end)

    restart = false
end, ON.LEVEL)

set_callback(function()
    local tile = get_grid_entity_at(6, 121, LAYER.FRONT)
    if tile then
        get_entity(tile):destroy()
    end
end, ON.TRANSITION)
