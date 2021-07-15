#include "entities_items.hpp"
#include "rpc.hpp"

void ParachutePowerup::deploy()
{
    // this is not perfect solution as it won't work if you spawn_over parachutepowerup on player
    auto state = State::get();
    bool is_player = false;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (state.items()->player(i) == overlay)
        {
            is_player = true;
            break;
        }
    }
    if (is_player)
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
        if (y == 0.0)
            y = 0.5;
        offsety = 0.115;
        hitboxy = 0.285;
        duckmask = 2143944961; // hehe funny number
    }
}