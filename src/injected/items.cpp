#include "items.h"
#include "entity.hpp"

Player *Items::player(size_t index) {
    auto pointer = ((uint64_t *) this)[1 + index];
    return (Player *) (pointer);
}
