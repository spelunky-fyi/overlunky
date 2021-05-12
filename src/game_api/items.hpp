#pragma once

#include "entity.hpp"

#define MAX_PLAYERS 4

struct Items
{
    void* __vftable;
    Player* players[MAX_PLAYERS];
    Player* player(size_t index);
};
