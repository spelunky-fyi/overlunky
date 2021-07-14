#include "entities_items.hpp"

void ParachutePowerup::deploy()
{
    // this is not perfect solution as it won't work if you spawn_over parachutepowerup on player/alien
    static auto alien = to_id("ENT_TYPE_MONS_ALIEN");
    if (overlay->type->search_flags == 0b1 // MASK.PLAYER
        || overlay->type->id == alien)
    {
        falltime_deploy = 0;
    }
    else
    {
        deployed = true;
        deployed2 = true;
        flags = flags ^ 0b1001;           // unset 1 & 4
        more_flags = more_flags | 0b1000; // set 4
        y = 0.5;
        offsety = 0.115;
        hitboxy = 0.285;
        duckmask = 2143944961; // hehe funny number
    }
}