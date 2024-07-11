#include "entities_mounts.hpp"

#include "movable.hpp" // for Movable
#include "search.hpp"  // for get_address

class Entity;

void Mount::carry(Movable* rider)
{
    using Carry = void(Entity*, Entity*);
    static Carry* carry = (Carry*)get_address("mount_carry");
    rider->move_state = 0x11;
    return carry(this, rider);
}
