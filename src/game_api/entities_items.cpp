#include "entities_items.hpp"
#include "rpc.hpp"

void ParachutePowerup::deploy()
{
    auto parachute_powerup = to_id("ENT_TYPE_ITEM_POWERUP_PARACHUTE");
    if (((PowerupCapable*)overlay)->has_powerup(parachute_powerup))
    {
        falltime_deploy = 0;
    }
    else
    {
        deployed = true;
        deployed2 = true;
        if ((flags & 0b1001) == 0b1001)
            flags = flags ^ 0b1001; // unset 1 & 4
        if ((more_flags & 0b1000) != 0b1000)
            more_flags = more_flags | 0b1000; // set 4
        if (y == 0.0f)
            y = 0.5f;
        offsety = 0.115f;
        hitboxy = 0.285f;
        duckmask = 2143944961; // hehe funny number
    }
}
