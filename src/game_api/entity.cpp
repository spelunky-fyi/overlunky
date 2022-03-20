#include "entity.hpp"
#include "movable.hpp"

#include <cstdio>
#include <string>
#include <vector>

#include "entities_chars.hpp"
#include "memory.hpp"
#include "state.hpp"
#include "thread_utils.hpp"
#include "vtable_hook.hpp"

using namespace std::chrono_literals;
using EntityMap = std::unordered_map<std::string, uint16_t>;

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

ENT_TYPE to_id(std::string_view name)
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
        auto movable_ent = this->as<Movable>();
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

        using RemoveFromLayer = void(Layer*, Entity*);
        static RemoveFromLayer* remove_from_layer = (RemoveFromLayer*)get_address("remove_from_layer");
        remove_from_layer(ptr_from, this);
    }

    auto ptr_to = state.ptr()->layers[dest_layer];

    using AddToLayer = void(Layer*, Entity*);
    static AddToLayer* add_to_layer = (AddToLayer*)get_address("add_to_layer");
    add_to_layer(ptr_to, this);

    for (auto item : items.entities())
    {
        item->set_layer(layer_to);
    }
}

void Entity::remove()
{
    if (layer != 2)
    {
        auto state = State::get();
        auto ptr_from = state.ptr()->layers[layer];
        if ((this->type->search_flags & 1) == 0 || ((Player*)this)->ai != 0)
        {
            using RemoveFromLayer = void(Layer*, Entity*);
            static RemoveFromLayer* remove_from_layer = (RemoveFromLayer*)get_address("remove_from_layer");
            remove_from_layer(ptr_from, this);

            for (auto item : items.entities())
            {
                item->remove();
            }
        }
        layer = 2;
    }
}

void Entity::respawn(LAYER layer_to)
{
    set_layer(layer_to);
}

void Entity::perform_teleport(uint8_t delta_x, uint8_t delta_y)
{
    using TeleportFun = void(Entity*, uint8_t, uint8_t);
    static TeleportFun* tp = (TeleportFun*)get_address("teleport");
    tp(this, delta_x, delta_y);
}

std::pair<float, float> Entity::position()
{
    auto [x_pos, y_pos] = position_self();

    // overlay exists if player is riding something / etc
    Entity* overlay_nested = overlay;
    while (overlay_nested != nullptr)
    {
        x_pos += overlay_nested->x;
        y_pos += overlay_nested->y;
        overlay_nested = overlay_nested->overlay;
    }
    return {x_pos, y_pos};
}

std::pair<float, float> Entity::position_self() const
{
    return std::pair<float, float>(x, y);
}

void Entity::remove_item(uint32_t item_uid)
{
    auto entity = get_entity_ptr(item_uid);
    if (entity)
        remove_item_ptr(entity);
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

void Movable::broken_damage(uint32_t damage_dealer_uid, int8_t damage_amount, uint16_t stun_time, float velocity_x, float velocity_y)
{
    damage(damage_dealer_uid, damage_amount, stun_time, velocity_x, velocity_y, 80);
}

void Movable::damage(uint32_t damage_dealer_uid, int8_t damage_amount, uint16_t stun_time, float velocity_x, float velocity_y, uint16_t iframes)
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
    on_regular_damage(dealer, damage_amount, 0x1000, velocities, unknown, stun_time, iframes);
}

bool Movable::is_button_pressed(BUTTON button)
{
    return (buttons & button) == button && (buttons_previous & button) == 0;
}
bool Movable::is_button_held(BUTTON button)
{
    return (buttons & button) == button && (buttons_previous & button) == button;
}
bool Movable::is_button_released(BUTTON button)
{
    return (buttons & button) == 0 && (buttons_previous & button) == button;
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
        0x2);
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
    {
        if (ent->rendering_info != nullptr && !ent->rendering_info->stop_render)
            return std::make_tuple(ent->rendering_info->x, ent->rendering_info->y, ent->layer);
        else
            return get_position(uid);
    }
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
        else if (ent->is_liquid())
        {
            auto liquid_engine = State::get().get_correct_liquid_engine(ent->type->id);
            vx = liquid_engine->entity_velocities->first;
            vy = liquid_engine->entity_velocities->second;
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
    if (auto* new_texture = ::get_texture(texture_id))
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
        std::erase_if(it->on_player_instagib, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->on_damage, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->pre_statemachine, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->post_statemachine, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->on_open, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->pre_collision1, [id](auto& hook)
                      { return hook.id == id; });
        std::erase_if(it->pre_collision2, [id](auto& hook)
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
                      } });
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
            0x5);
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
            0x3);
    }
    hook_info.on_kill.push_back({reserved_callback_id, std::move(on_kill)});
}

void Entity::set_on_player_instagib(std::uint32_t reserved_callback_id, std::function<bool(Entity*)> on_instagib)
{
    EntityHooksInfo& hook_info = get_hooks();
    // no hooking here, because the instagib function is hooked in rpc.cpp
    hook_info.on_player_instagib.push_back({reserved_callback_id, std::move(on_instagib)});
}

void Entity::set_on_damage(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*, int8_t, float, float, uint16_t, uint8_t)> on_damage)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.on_damage.empty())
    {
        if ((this->type->search_flags & 0x1) == 0x1)
        {
            // Can't hook player::on_damage here, because this is permanently hooked for the god function.
            // The god function takes care of calling the script hooks in rpc.cpp
        }
        else
        {
            hook_vtable<void(Entity*, Entity*, int8_t, uint32_t, float*, float*, uint16_t, uint8_t)>(
                this,
                [](Entity* self, Entity* damage_dealer, int8_t damage_amount, uint32_t unknown1, float* velocities, float* unknown2, uint16_t stun_amount, uint8_t iframes, void (*original)(Entity*, Entity*, int8_t, uint32_t, float*, float*, uint16_t, uint8_t))
                {
                    EntityHooksInfo& _hook_info = self->get_hooks();
                    bool skip_orig = false;
                    for (auto& [id, on_damage] : _hook_info.on_damage)
                    {
                        if (on_damage(self, damage_dealer, damage_amount, velocities[0], velocities[1], stun_amount, iframes))
                        {
                            skip_orig = true;
                        }
                    }

                    if (!skip_orig)
                    {
                        original(self, damage_dealer, damage_amount, unknown1, velocities, unknown2, stun_amount, iframes);
                    }
                },
                0x30);
        }
    }
    hook_info.on_damage.push_back({reserved_callback_id, std::move(on_damage)});
}

bool Entity::is_movable()
{
    static const ENT_TYPE first_logical = to_id("ENT_TYPE_LOGICAL_CONSTELLATION");
    if (type->search_flags & 0b11111111) // PLAYER | MOUNT | MONSTER | ITEM | ROPE | EXPLOSION | FX | ACTIVEFLOOR
        return true;
    else if (type->search_flags & 0x1000) // LOGICAL - as it has some movable entities
        if (type->id < first_logical)     // actually check if it's not logical
            return true;

    return false;
}

bool Entity::is_liquid()
{
    static const ENT_TYPE liquid_water = to_id("ENT_TYPE_LIQUID_WATER");
    static const ENT_TYPE liquid_coarse_water = to_id("ENT_TYPE_LIQUID_COARSE_WATER");
    static const ENT_TYPE liquid_lava = to_id("ENT_TYPE_LIQUID_LAVA");
    static const ENT_TYPE liquid_stagnant_lava = to_id("ENT_TYPE_LIQUID_STAGNANT_LAVA");
    static const ENT_TYPE liquid_coarse_lava = to_id("ENT_TYPE_LIQUID_COARSE_LAVA");

    if (type->id == liquid_water || type->id == liquid_coarse_water || type->id == liquid_lava || type->id == liquid_stagnant_lava || type->id == liquid_coarse_lava)
        return true;

    return false;
}

void Entity::set_pre_collision1(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*)> pre_collision1)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.pre_collision1.empty())
    {
        hook_vtable<void(Entity*, Entity*)>(
            this,
            [](Entity* self, Entity* collision_entity, void (*original)(Entity*, Entity*))
            {
                EntityHooksInfo& _hook_info = self->get_hooks();

                bool skip_orig = false;
                for (auto& [id, pre] : _hook_info.pre_collision1)
                {
                    if (pre(self, collision_entity))
                    {
                        skip_orig = true;
                    }
                }

                if (!skip_orig)
                {
                    original(self, collision_entity);
                }
            },
            0x4);
    }
    hook_info.pre_collision1.push_back({reserved_callback_id, std::move(pre_collision1)});
}

void Entity::set_pre_collision2(std::uint32_t reserved_callback_id, std::function<bool(Entity*, Entity*)> pre_collision2)
{
    EntityHooksInfo& hook_info = get_hooks();
    if (hook_info.pre_collision2.empty())
    {
        hook_vtable<void(Entity*, Entity*)>(
            this,
            [](Entity* self, Entity* collision_entity, void (*original)(Entity*, Entity*))
            {
                EntityHooksInfo& _hook_info = self->get_hooks();

                bool skip_orig = false;
                for (auto& [id, pre] : _hook_info.pre_collision2)
                {
                    if (pre(self, collision_entity))
                    {
                        skip_orig = true;
                    }
                }

                if (!skip_orig)
                {
                    original(self, collision_entity);
                }
            },
            0x1A);
    }
    hook_info.pre_collision2.push_back({reserved_callback_id, std::move(pre_collision2)});
}

std::span<uint32_t> Entity::get_items()
{
    if (items.size)
        return std::span<uint32_t>(items.uids().begin(), items.uids().end());

    return {};
}

Entity* get_entity_ptr(uint32_t uid)
{
    auto state = State::get();
    auto p = state.find(uid);
    if (IsBadWritePtr(p, 0x178))
        return nullptr;
    return p;
}

std::vector<uint32_t> Movable::get_all_behaviors()
{
    std::vector<uint32_t> anims;
    anims.reserve(behavior_map.size());

    for (auto& cur : behavior_map)
    {
        anims.push_back(cur.first);
    }
    return anims;
}

bool Movable::set_behavior(uint32_t an)
{
    const auto& it = behavior_map.find(an);
    if (it != behavior_map.end())
    {
        current_behavior = it->second;
        return true;
    }
    return false;
}

uint32_t Movable::get_behavior()
{
    for (auto& cur : behavior_map)
    {
        if (cur.second == current_behavior)
        {
            return cur.first;
        }
    }
    return 0; // there is no id 0, but i can be wrong
}
