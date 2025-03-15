#include "entities_items.hpp"

#include <new>         // for operator new
#include <type_traits> // for move
#include <vector>      // for vector, _Vector_iterator, _Vector_cons...

#include "entities_chars.hpp" // for PowerupCapable
#include "entity.hpp"         // SHAPE
#include "entity_db.hpp"      // to_id

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
        shape = SHAPE::RECTANGLE;
        hitbox_enabled = true;
    }
}
