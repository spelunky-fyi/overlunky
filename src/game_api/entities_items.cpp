#include "entities_items.hpp"

#include "vtable_hook.hpp"

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

void Container::set_on_open(std::uint32_t reserved_callback_id, std::function<void(Container*, Movable*)> on_open)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.on_open.empty())
    {
        hook_vtable<void(Container*, Movable*)>(
            this,
            [](Container* self, Movable* opener, void (*original)(Container*, Movable*))
            {
                if (opener->movey > 0)
                {
                    EntityHooksInfo& _hook_info = self->get_hooks();
                    for (auto& [id, on_open] : _hook_info.on_open)
                    {
                        on_open(self, opener);
                    }
                }
                original(self, opener);
            },
            0x18);
    }
    hook_info.on_open.push_back({reserved_callback_id, std::move(on_open)});
}
