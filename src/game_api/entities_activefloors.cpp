#include "entities_activefloors.hpp"

#include "entity.hpp"        // for Entity, to_id, EntityDB
#include "items.hpp"         // IWYU pragma: keep
#include "layer.hpp"         // for EntityList::Range, EntityList, EntityList::Ent...
#include "search.hpp"        // for get_address
#include "sound_manager.hpp" //

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

void Drill::trigger(std::optional<bool> play_sound_effect)
{
    if (move_state != 0 || standing_on_uid != -1)
    {
        return;
    }
    detach(true);
    move_state = 6;
    flags = flags & ~(1U << (10 - 1));

    sound1 = construct_soundmeta(0x159, false);
    sound1->start();
    sound2 = construct_soundmeta(0x153, false);
    sound2->start();
    if (play_sound_effect.value_or(false))
        play_sound(0xA4, uid);
}
