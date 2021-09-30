#include "entities_mounts.hpp"

void Mount::carry(Movable* rider)
{
    using Carry = void(Entity*, Entity*);
    static Carry* carry = (Carry*)get_address("mount_carry");
    rider->move_state = 0x11;
    return carry(this, rider);
}

void Mount::tame(bool value)
{
    tamed = value;
    flags = flags | 0x20000;
}
