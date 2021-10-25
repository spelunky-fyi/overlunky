#include "entities_activefloors.hpp"
#include "rpc.hpp"

uint8_t Olmec::broken_floaters()
{
    static auto olmec_floater_id = to_id("ENT_TYPE_FX_OLMECPART_FLOATER");
    uint8_t broken = 0;
    int* pitems = (int*)items.begin;
    for (uint8_t idx = 0; idx < items.count; ++idx)
    {
        auto ent_type = get_entity_type(pitems[idx]);
        if (ent_type == olmec_floater_id)
        {
            auto olmec_floater = get_entity_ptr(pitems[idx]);
            if (olmec_floater->animation_frame == 0x27)
            {
                broken++;
            }
        }
    }
    return broken;
}

void Drill::trigger()
{
    if (move_state != 0 || overlay == nullptr)
    {
        return;
    }

    move_state = 6;
    overlay->remove_item_ptr(this);
    flags = flags & ~(1U << (10 - 1));

    using construct_soundposition_ptr_fun_t = SoundPosition*(uint32_t id, uint32_t unknown);
    static auto construct_soundposition_ptr_call = (construct_soundposition_ptr_fun_t*)get_address("construct_soundposition_ptr");
    sound_pos1 = construct_soundposition_ptr_call(0x159, 0);
    sound_pos2 = construct_soundposition_ptr_call(0x153, 0);
}
