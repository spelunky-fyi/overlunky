#pragma once

#include "entity.hpp"
#include "state_structs.hpp"

#define MAX_PLAYERS 4

struct Items
{
    void* __vftable;
    Player* players[MAX_PLAYERS];
    Inventory player_inventories[MAX_PLAYERS];
    SelectPlayerSlot player_select_slots[MAX_PLAYERS];

    Player* player(size_t index);
};
