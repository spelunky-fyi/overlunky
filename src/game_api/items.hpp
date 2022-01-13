#pragma once

#include "entities_chars.hpp"
#include "entity.hpp"
#include "state_structs.hpp"

struct Items
{
    void* __vftable;
    Player* players[MAX_PLAYERS];
    std::array<Inventory, MAX_PLAYERS> player_inventories;
    std::array<SelectPlayerSlot, MAX_PLAYERS> player_select_slots;
    /// Pet information for level transition
    std::array<ENT_TYPE, 4> saved_pets;
    std::array<bool, 4> is_pet_cursed;
    std::array<bool, 4> is_pet_poisoned;
    uint8_t saved_pets_count;

    uint8_t player_count;

    Player* player(uint8_t index);
};
