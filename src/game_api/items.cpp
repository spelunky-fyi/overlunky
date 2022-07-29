#include "items.hpp"

#include <cstdint> // for uint8_t

Player* Items::player(uint8_t index)
{
    return players[index];
}
