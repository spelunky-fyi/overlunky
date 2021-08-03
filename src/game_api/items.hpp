#pragma once

#include "entities_chars.hpp"
#include "entity.hpp"
#include "state_structs.hpp"

#define MAX_PLAYERS 4

struct Items
{
    void* __vftable;
    Player* players[MAX_PLAYERS];
    Inventory player_inventories[MAX_PLAYERS];
    SelectPlayerSlot player_select_slots[MAX_PLAYERS];

    Player* player(uint8_t index);
};
