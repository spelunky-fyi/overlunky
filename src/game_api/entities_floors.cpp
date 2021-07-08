#include "entities_floors.hpp"
#include "rpc.hpp"
#include "spawn_api.hpp"

void Door::set_target(uint8_t w, uint8_t l, uint8_t t)
{
    uint8_t array[5] = {1, l, 1, w, t};
    DEBUG("Making door go to {}-{}, {}", w, l, t);
    write_mem(pointer() + 0xc1, std::string((char*)array, sizeof(array)));
}

std::tuple<uint8_t, uint8_t, uint8_t> Door::get_target()
{
    uint8_t l = read_u8(pointer() + 0xc2);
    uint8_t w = read_u8(pointer() + 0xc4);
    uint8_t t = read_u8(pointer() + 0xc5);
    return std::make_tuple(w, l, t);
}

void Arrowtrap::rearm()
{
    if (arrow_shot)
    {
        static auto arrow_trap_trigger_id = to_id("ENT_TYPE_LOGICAL_ARROW_TRAP_TRIGGER");
        arrow_shot = false;
        auto trigger = get_entity_ptr(spawn_entity_over(arrow_trap_trigger_id, uid, 0., 0.));
        if ((flags & (1 << 16)) > 0)
        {
            trigger->flags |= (1 << 16);
        }
    }
}
