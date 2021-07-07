#include "entities_activefloors.hpp"
#include "rpc.hpp"

uint8_t Olmec::broken_floaters()
{
    static auto olmec_floater_id = to_id("ENT_TYPE_FX_OLMECPART_FLOATER");
    uint8_t broken = 0;
    int* pitems = (int*)items.begin;
    for (uint8_t x = 0; x < items.count; ++x)
    {
        auto type = get_entity_type(pitems[x]);
        if (type == olmec_floater_id)
        {
            auto olmec_floater = get_entity_ptr(pitems[x]);
            if (olmec_floater->animation_frame == 0x27)
            {
                broken++;
            }
        }
    }
    return broken;
}
