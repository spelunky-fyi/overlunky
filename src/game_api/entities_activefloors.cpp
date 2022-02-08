#include "entities_activefloors.hpp"
#include "rpc.hpp"

uint8_t Olmec::broken_floaters()
{
    static auto olmec_floater_id = to_id("ENT_TYPE_FX_OLMECPART_FLOATER");
    uint8_t broken = 0;
    for (auto item : items.entities())
    {
        if (item->type->id == olmec_floater_id)
        {
            if (item->animation_frame == 0x27)
            {
                broken++;
            }
        }
    }
    return broken;
}

void Drill::trigger()
{
    if (move_state != 0 || standing_on_uid != -1)
    {
        return;
    }

    if (overlay != nullptr)
    {
        overlay->remove_item_ptr(this);
    }

    move_state = 6;
    flags = flags & ~(1U << (10 - 1));

    using construct_soundposition_ptr_fun_t = SoundPosition*(uint32_t id, uint32_t unknown);
    static auto construct_soundposition_ptr_call = (construct_soundposition_ptr_fun_t*)get_address("construct_soundposition_ptr");
    sound_pos1 = construct_soundposition_ptr_call(0x159, 0);
    sound_pos2 = construct_soundposition_ptr_call(0x153, 0);
}
