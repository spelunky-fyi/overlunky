#include "entity.hpp"

#include <cstdio>
#include <string>
#include <vector>

#include "character_def.hpp"
#include "entities_items.hpp"
#include "logger.h"
#include "render_api.hpp"
#include "rpc.hpp"
#include "spawn_api.hpp"
#include "state.hpp"
#include "texture.hpp"
#include "vtable_hook.hpp"

using namespace std::chrono_literals;
using EntityMap = std::unordered_map<std::string, uint16_t>;

template <class FunT>
struct HookWithId
{
    std::uint32_t id;
    std::function<FunT> fun;
};
struct EntityHooksInfo
{
    void* entity;
    std::uint32_t cbcount;
    std::vector<HookWithId<void(Entity*)>> on_dtor;
    std::vector<HookWithId<void(Entity*)>> on_destroy;
    std::vector<HookWithId<void(Entity*, Entity*)>> on_kill;
    std::vector<HookWithId<bool(Movable*)>> pre_statemachine;
    std::vector<HookWithId<void(Movable*)>> post_statemachine;
    std::vector<HookWithId<void(Container*, Movable*)>> on_open;
};
std::vector<EntityHooksInfo> g_entity_hooks;

struct EntityBucket
{
    void** begin;
    void** current; // Note, counts down from end to begin instead of up from begin to end :shrug:
    void** end;
};
struct EntityPool
{
    std::uint32_t slot_size;
    std::uint32_t initial_slots;
    std::uint32_t slots_growth;
    std::uint32_t current_slots;
    std::uint64_t _ulong_0;
    EntityBucket* _some_bucket;
    EntityBucket* bucket;
};
struct EntityFactory
{
    EntityDB types[0x395];
    bool type_set[0x395];
    std::unordered_map<std::uint32_t, OnHeapPointer<EntityPool>> entity_instance_map;
    EntityMap entity_map;
    void* _ptr_7;
};

EntityFactory* entity_factory()
{
    static EntityFactory* cache_entity_factory = *(EntityFactory**)get_address("entity_factory"sv);
    while (cache_entity_factory == 0)
    {
        std::this_thread::sleep_for(500ms);
        cache_entity_factory = *(EntityFactory**)get_address("entity_factory"sv);
    }
    return cache_entity_factory;
}

AddLayer get_add_layer()
{
    ONCE(AddLayer)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x40\x56\x41\x54\x48\x83\xec\x58\x4c\x8b\xe1\x48\x8b\xf2\x48\x83\xc1\x08\xe8\xd9\xe1\xff\xff"s, memory.after_bundle);
        off = function_start(memory.at_exe(off));
        return res = (AddLayer)off;
    }
}

RemoveLayer get_remove_layer()
{
    ONCE(RemoveLayer)
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x40\x53\x56\x41\x55\x48\x83\xec\x50\x4c\x8b\xe9\x48\x8b\xf2\x48\x83\xc1\x08"s, memory.after_bundle);
        off = function_start(memory.at_exe(off));
        return res = (RemoveLayer)off;
    }
}

std::vector<EntityItem> list_entities()
{
    const EntityFactory* entity_factory_ptr = entity_factory();
    if (!entity_factory_ptr)
        return {};

    const EntityMap& map = entity_factory_ptr->entity_map;

    std::vector<EntityItem> result;
    for (const auto& [name, id] : map)
    {
        result.emplace_back(name, id);
    }
    return result;
}

EntityDB* get_type(uint32_t id)
{
    EntityFactory* entity_factory_ptr = entity_factory();

    // Special case: map_ptr might be 0 if it's not initialized.
    // This only occurs in list_entities; for others, do not check the pointer
    // to see if this assumption works.
    if (!entity_factory_ptr)
        return nullptr;

    return entity_factory_ptr->types + id;
}

uint32_t to_id(std::string_view name)
{
    const EntityFactory* entity_factory_ptr = entity_factory();
    if (!entity_factory_ptr)
        return {};
    const EntityMap& map = entity_factory_ptr->entity_map;
    auto it = map.find(std::string(name));
    return it != map.end() ? it->second : -1;
}

void Entity::teleport(float dx, float dy, bool s, float vx, float vy, bool snap)
{
    if (overlay)
        overlay->remove_item(uid);
    overlay = NULL;
    auto topmost = topmost_mount();
    if (!s)
    {
        auto [x_pos, y_pos] = topmost->position();
        // player relative coordinates
        x_pos += dx;
        y_pos += dy;
        if (snap)
        {
            x_pos = round(x_pos);
            y_pos = round(y_pos);
        }
        topmost->x = x_pos;
        topmost->y = y_pos;
    }
    else
    {
        // screen coordinates -1..1
        // log::debug!("Teleporting to screen {}, {}", x, y);
        auto state = State::get();
        auto [x_pos, y_pos] = state.click_position(dx, dy);
        if (snap && abs(vx) + abs(vy) <= 0.04f)
        {
            x_pos = round(x_pos);
            y_pos = round(y_pos);
        }
        // log::debug!("Teleporting to {}, {}", x, y);
        topmost->x = x_pos;
        topmost->y = y_pos;
    }
    // set velocity
    if (topmost->is_movable())
    {
        auto movable_ent = (Movable*)topmost;
        movable_ent->velocityx = vx;
        movable_ent->velocityy = vy;
    }
    return;
}

void Entity::teleport_abs(float dx, float dy, float vx, float vy)
{
    if (overlay)
        overlay->remove_item(uid);
    overlay = NULL;
    x = dx;
    y = dy;
    if (is_movable())
    {
        auto movable_ent = (Movable*)pointer();
        movable_ent->velocityx = vx;
        movable_ent->velocityy = vy;
    }
}

void Entity::set_layer(LAYER layer_to)
{
    uint8_t dest_layer = enum_to_layer(layer_to);
    if (layer == dest_layer)
        return;

    auto state = State::get();
    if (this != this->topmost_mount())
        this->topmost_mount()->set_layer(layer_to);

    if (layer == 0 || layer == 1)
    {
        auto ptr_from = state.ptr()->layers[layer];
        auto remove_layer_func = get_remove_layer();
        remove_layer_func(ptr_from, this);
    }

    auto ptr_to = state.ptr()->layers[dest_layer];
    auto add_layer_func = get_add_layer();
    add_layer_func(ptr_to, this);

    int* pitems = (int*)items.begin;
    for (uint8_t idx = 0; idx < items.count; ++idx)
    {
        auto item = get_entity_ptr(pitems[idx]);
        item->set_layer(layer_to);
    }
}

void Entity::remove()
{
    auto state = State::get();
    auto ptr_from = state.ptr()->layers[layer];
    auto remove_layer_func = get_remove_layer();
    if ((this->type->search_flags & 1) == 0 || ((Player*)this)->ai != 0)
    {
        remove_layer_func(ptr_from, this);
        int* pitems = (int*)items.begin;
        for (uint8_t idx = 0; idx < items.count; ++idx)
        {
            auto item = get_entity_ptr(pitems[idx]);
            item->remove();
        }
    }
    layer = 2;
}

void Entity::respawn(LAYER layer_to)
{
    set_layer(layer_to);
}

std::pair<float, float> Entity::position()
{
    // Return the resolved position
    // overlay exists if player is riding something / etc
    auto [x_pos, y_pos] = position_self();
    // log::debug!("Item #{}: Position is {}, {}", unique_id(), x, y);
    switch ((size_t)overlay)
    {
    case NULL:
        return {x_pos, y_pos};
    default:
    {
        float _x, _y;
        std::tie(_x, _y) = overlay->position();
        return {x_pos + _x, y_pos + _y};
    }
    }
}

std::pair<float, float> Entity::position_self() const
{
    return std::pair<float, float>(x, y);
}

std::pair<float, float> Entity::position_render() const
{
    // This isn't perfect but at least it fixes the trigger hitboxes for now
    auto [x_pos, y_pos] = position_self();
    switch ((size_t)overlay)
    {
    case NULL:
        return {rendering_info->x, rendering_info->y};
    default:
    {
        float _x, _y;
        std::tie(_x, _y) = overlay->position();
        return {x_pos + _x, y_pos + _y};
    }
    }
}

void Entity::remove_item(uint32_t id)
{
    remove_item_ptr(State::get().find(id));
}

void Player::set_jetpack_fuel(uint8_t fuel)
{
    static auto jetpackID = to_id("ENT_TYPE_ITEM_JETPACK");
    int* pitems = (int*)items.begin;
    for (uint8_t idx = 0; idx < items.count; ++idx)
    {
        auto ent_type = get_entity_type(pitems[idx]);
        if (ent_type == jetpackID)
        {
            auto jetpack = get_entity_ptr(pitems[idx])->as<Jetpack>();
            jetpack->fuel = fuel;
            break;
        }
    }
}

uint8_t Player::kapala_blood_amount()
{
    static auto kapalaPowerupID = to_id("ENT_TYPE_ITEM_POWERUP_KAPALA");
    int* pitems = (int*)items.begin;
    for (uint8_t idx = 0; idx < items.count; ++idx)
    {
        auto ent_type = get_entity_type(pitems[idx]);
        if (ent_type == kapalaPowerupID)
        {
            auto kapala = get_entity_ptr(pitems[idx])->as<KapalaPowerup>();
            return kapala->amount_of_blood;
        }
    }
    return 0;
}

void Movable::poison(int16_t frames)
{
    static size_t offset_first = 0;
    static size_t offset_subsequent = 0;
    if (offset_first == 0)
    {
        offset_first = get_address("first_poison_tick_timer_default");
        offset_subsequent = get_address("subsequent_poison_tick_timer_default");
    }
    poison_tick_timer = frames;

    if (frames == -1)
    {
        frames = 1800;
    }
    write_mem_prot(offset_first, frames, true);
    write_mem_prot(offset_subsequent, frames, true);
}

bool Movable::is_poisoned()
{
    return (poison_tick_timer != -1);
}

void Movable::damage(uint32_t damage_dealer_uid, int8_t damage_amount, uint16_t stun_time, float velocity_x, float velocity_y)
{
    if ((flags & (1 << 28)) > 0)
    {
        return;
    }

    auto dealer = get_entity_ptr(damage_dealer_uid);
    if (dealer == nullptr)
    {
        return;
    }

    float velocities[] = {velocity_x, velocity_y};
    float unknown[] = {0.0f, 0.0f};
    on_regular_damage(dealer, damage_amount, 0x1000, velocities, unknown, stun_time);
}

bool Movable::is_button_pressed(BUTTON button)
{
    return (buttons & button) != 0 && (buttons_previous & button) == 0;
}
bool Movable::is_button_held(BUTTON button)
{
    return (buttons & button) != 0 && (buttons_previous & button) != 0;
}
bool Movable::is_button_released(BUTTON button)
{
    return (buttons & button) == 0 && (buttons_previous & button) != 0;
}

void hook_movable_state_machine(Movable* _self)
{
    hook_vtable<void(Movable*)>(
        _self,
        [](Movable* self, void (*original)(Movable*))
        {
            EntityHooksInfo& hook_info = self->get_hooks();

            bool skip_orig = false;
            for (auto& [id, pre] : hook_info.pre_statemachine)
            {
                if (pre(self))
                {
                    skip_orig = true;
                }
            }

            if (!skip_orig)
            {
                original(self);
            }

            for (auto& [id, post] : hook_info.post_statemachine)
            {
                post(self);
            }
        },
        0x24);
}
void Movable::set_pre_statemachine(std::uint32_t reserved_callback_id, std::function<bool(Movable*)> pre_state_machine)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.pre_statemachine.empty() && hook_info.post_statemachine.empty())
    {
        hook_movable_state_machine(this);
    }
    hook_info.pre_statemachine.push_back({reserved_callback_id, std::move(pre_state_machine)});
}
void Movable::set_post_statemachine(std::uint32_t reserved_callback_id, std::function<void(Movable*)> post_state_machine)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.pre_statemachine.empty() && hook_info.post_statemachine.empty())
    {
        hook_movable_state_machine(this);
    }
    hook_info.post_statemachine.push_back({reserved_callback_id, std::move(post_state_machine)});
}

std::tuple<float, float, uint8_t> get_position(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent)
        return std::make_tuple(ent->position().first, ent->position().second, ent->layer);
    return {0.0f, 0.0f, (uint8_t)0};
}

std::tuple<float, float, uint8_t> get_render_position(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent)
        return std::make_tuple(ent->position_render().first, ent->position_render().second, ent->layer);
    return {0.0f, 0.0f, (uint8_t)0};
}

std::tuple<float, float> get_velocity(uint32_t uid)
{
    if (Entity* ent = get_entity_ptr(uid))
    {
        float vx{0.0f};
        float vy{0.0f};
        if (ent->is_movable())
        {
            Movable* mov = ent->as<Movable>();
            vx = mov->velocityx;
            vy = mov->velocityy;
        }
        if (ent->overlay)
        {
            auto [ovx, ovy] = get_velocity(ent->overlay->uid);
            vx += ovx;
            vy += ovy;
        }
        return std::tuple{vx, vy};
    }
    return std::tuple{0.0f, 0.0f};
}

AABB get_hitbox(uint32_t uid, bool use_render_pos)
{
    if (Entity* ent = get_entity_ptr(uid))
    {
        auto [x, y, l] = (use_render_pos ? get_render_position : get_position)(uid);
        return AABB{
            x - ent->hitboxx + ent->offsetx,
            y + ent->hitboxy + ent->offsety,
            x + ent->hitboxx + ent->offsetx,
            y - ent->hitboxy + ent->offsety,
        };
    }
    return AABB{0.0f, 0.0f, 0.0f, 0.0f};
}

TEXTURE Entity::get_texture()
{
    return texture->id;
}
bool Entity::set_texture(TEXTURE texture_id)
{
    if (auto* new_texture = RenderAPI::get().get_texture(texture_id))
    {
        apply_texture(new_texture);
        return true;
    }
    return false;
}

void Entity::unhook(std::uint32_t id)
{
    auto it = std::find_if(g_entity_hooks.begin(), g_entity_hooks.end(), [this](auto& hook)
                           { return hook.entity == this; });
    if (it != g_entity_hooks.end())
    {
        std::erase_if(it->on_dtor, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->on_destroy, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->on_kill, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->pre_statemachine, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->post_statemachine, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->on_open, [id](auto& hook)
                      { return hook.id == id; });
    }
}
EntityHooksInfo& Entity::get_hooks()
{
    auto it = std::find_if(g_entity_hooks.begin(), g_entity_hooks.end(), [this](auto& hook)
                           { return hook.entity == this; });
    if (it == g_entity_hooks.end())
    {
        hook_dtor(this, [](void* self)
                  {
                      auto _it = std::find_if(g_entity_hooks.begin(), g_entity_hooks.end(), [self](auto& hook)
                                              { return hook.entity == self; });
                      if (_it != g_entity_hooks.end())
                      {
                          for (auto& cb : _it->on_dtor)
                          {
                              cb.fun((Entity*)self);
                          }
                          g_entity_hooks.erase(_it);
                      }
                  });
        g_entity_hooks.push_back({this});
        return g_entity_hooks.back();
    }
    return *it;
}

std::uint32_t Entity::set_on_dtor(std::function<void(Entity*)> cb)
{
    EntityHooksInfo& hook_info = get_hooks();
    hook_info.on_dtor.push_back({hook_info.cbcount++, std::move(cb)});
    return hook_info.on_dtor.back().id;
}
std::uint32_t Entity::reserve_callback_id()
{
    EntityHooksInfo& hook_info = get_hooks();
    return hook_info.cbcount++;
}
void Entity::set_on_destroy(std::uint32_t reserved_callback_id, std::function<void(Entity*)> on_destroy)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.on_destroy.empty())
    {
        hook_vtable<void(Entity*)>(
            this,
            [](Entity* self, void (*original)(Entity*))
            {
                EntityHooksInfo& _hook_info = self->get_hooks();
                for (auto& [id, on_destroy] : _hook_info.on_destroy)
                {
                    on_destroy(self);
                }
                original(self);
            },
            0x4);
    }
    hook_info.on_destroy.push_back({reserved_callback_id, std::move(on_destroy)});
}
void Entity::set_on_kill(std::uint32_t reserved_callback_id, std::function<void(Entity*, Entity*)> on_kill)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.on_kill.empty())
    {
        hook_vtable<void(Entity*, bool, Entity*)>(
            this,
            [](Entity* self, bool _some_bool, Entity* from, void (*original)(Entity*, bool, Entity*))
            {
                EntityHooksInfo& _hook_info = self->get_hooks();
                for (auto& [id, on_kill] : _hook_info.on_kill)
                {
                    on_kill(self, from);
                }
                original(self, _some_bool, from);
            },
            0x2);
    }
    hook_info.on_kill.push_back({reserved_callback_id, std::move(on_kill)});
}

bool Entity::is_movable()
{
    if (type->search_flags & 0b11111111) // PLAYER | MOUNT | MONSTER | ITEM | ROPE | EXPLOSION | FX | ACTIVEFLOOR
        return true;
    else if (type->search_flags & 0x1000) // LOGICAL - as it has some movable entities
        if (type->id < 842)               // actually check if it's not logical
            return true;

    return false;
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
            0x17);
    }
    hook_info.on_open.push_back({reserved_callback_id, std::move(on_open)});
}
