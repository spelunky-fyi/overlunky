#pragma once

#include "entities_chars.hpp"
#include "entity.hpp"
#include "state_structs.hpp"

struct Items
{
    void* __vftable;
    Player* players[MAX_PLAYERS];
    Inventory player_inventories[MAX_PLAYERS];
    SelectPlayerSlot player_select_slots[MAX_PLAYERS];

    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint8_t unknown7;

    uint8_t player_count;

    Player* player(uint8_t index);
};
