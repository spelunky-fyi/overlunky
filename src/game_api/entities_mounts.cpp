#include "entities_mounts.hpp"

using Carry = void (*)(Entity*, Entity*);
void Mount::carry(Movable* rider)
{
    auto carry = (Carry)get_address("mount_carry_rider");
    rider->move_state = 0x11;
    return carry(this, rider);
}

void Mount::tame(bool value)
{
    tamed = value;
    flags = flags | 0x20000;
}
