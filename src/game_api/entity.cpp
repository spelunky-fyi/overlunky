#include "entity.hpp"

#include <Windows.h> // for IsBadWritePtr
#include <chrono>    // for operator<=>, operator-, operator+
#include <cmath>     // for round
#include <compare>   // for operator<, operator<=, operator>
#include <cstdint>   // for uint32_t, uint16_t, uint8_t
#include <cstdlib>   // for abs, NULL, size_t
#include <list>      // for _List_const_iterator
#include <map>       // for _Tree_iterator, map, _Tree_cons...
#include <new>       // for operator new
#include <string>    // for allocator, string, operator""sv
#include <thread>    // for sleep_for
#include <vector>    // for vector, _Vector_iterator, erase_if

#include "containers/custom_map.hpp" // for custom_map
#include "entities_chars.hpp"        // for Player
#include "entity_hooks_info.hpp"     // for EntityHooksInfo
#include "memory.hpp"                // for write_mem_prot
#include "movable.hpp"               // for Movable
#include "movable_behavior.hpp"      // for MovableBehavior
#include "render_api.hpp"            // for RenderInfo
#include "search.hpp"                // for get_address
#include "state.hpp"                 // for State, StateMemory, enum_to_layer
#include "state_structs.hpp"         // for LiquidPhysicsEngine
#include "texture.hpp"               // for get_texture, Texture
#include "vtable_hook.hpp"           // for hook_vtable, hook_dtor, unregis...

using namespace std::chrono_literals;

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
    static const size_t offset_first = get_address("first_poison_tick_timer_default");
    static const size_t offset_subsequent = get_address("subsequent_poison_tick_timer_default");
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

bool Movable::broken_damage(uint32_t damage_dealer_uid, int8_t damage_amount, uint16_t stun_time, float velocity_x, float velocity_y)
{
    return damage(damage_dealer_uid, damage_amount, stun_time, velocity_x, velocity_y, 80);
}

bool Movable::damage(uint32_t damage_dealer_uid, int8_t damage_amount, uint16_t stun_time, float velocity_x, float velocity_y, uint8_t iframes)
{
    /* why?
    if ((flags & (1 << 28)) > 0)
    {
        return;
    }*/

    auto dealer = get_entity_ptr(damage_dealer_uid);
    /* but it can be nil?
    if (dealer == nullptr)
    {
        return;
    }*/

    Vec2 velocity{velocity_x, velocity_y};
    uint8_t unknown1{0};
    bool unknown2{true};
    return on_damage(dealer, damage_amount, 0x1, &velocity, unknown1, stun_time, iframes, unknown2);
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
        if (ent->rendering_info != nullptr && !ent->rendering_info->render_inactive)
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
    if (texture)
        return texture->id;

    return -1;
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

bool Entity::is_player()
{
    if (type->search_flags & 1)
    {
        Player* pl = this->as<Player>();
        return pl->ai == nullptr;
    }
    return false;
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

void Entity::set_enable_turning(bool enabled)
{
    set_entity_turning(this, enabled);
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

Entity* get_entity_ptr_local(uint32_t uid)
{
    auto state = State::get();
    auto p = state.find_local(uid);
    if (IsBadWritePtr(p, 0x178))
        return nullptr;
    return p;
}

std::vector<uint32_t> Movable::get_all_behaviors()
{
    std::vector<uint32_t> anims;
    anims.reserve(behaviors_map.size());

    for (auto& cur : behaviors_map)
    {
        anims.push_back(cur.first);
    }
    return anims;
}

bool Movable::set_behavior(uint32_t an)
{
    const auto& it = behaviors_map.find(an);
    if (it != behaviors_map.end())
    {
        if (current_behavior != nullptr)
        {
            current_behavior->on_exit(this);
        }
        current_behavior = it->second;
        if (current_behavior != nullptr)
        {
            current_behavior->on_enter(this);
        }
        return true;
    }
    return false;
}

uint32_t Movable::get_behavior()
{
    for (auto& cur : behaviors_map)
    {
        if (cur.second == current_behavior)
        {
            return cur.first;
        }
    }
    return 0; // there is no id 0, but i can be wrong
}

void Movable::set_gravity(float gravity)
{
    hook_vtable<void(Movable*, float), 0x53>(
        this,
        [gravity](Movable* ent, [[maybe_unused]] float _gravity, void (*original)(Movable*, float))
        {
            original(ent, gravity);
        });
}

void Movable::reset_gravity()
{
    unregister_hook_function((void***)this, 0x53);
}

void Movable::set_position(float to_x, float to_y)
{
    if (overlay)
        return;
    auto dx = to_x - x;
    auto dy = to_y - y;
    x = to_x;
    y = to_y;
    if (rendering_info)
    {
        rendering_info->x += dx;
        rendering_info->y += dy;
        rendering_info->x_dupe1 += dx;
        rendering_info->y_dupe1 += dy;
        rendering_info->x_dupe2 += dx;
        rendering_info->y_dupe2 += dy;
        rendering_info->x_dupe3 += dx;
        rendering_info->y_dupe3 += dy;
        rendering_info->x_dupe4 += dx;
        rendering_info->y_dupe4 += dy;
    }
    if (State::get().ptr()->camera->focused_entity_uid == uid)
    {
        State::get().ptr()->camera->focus_x += dx;
        State::get().ptr()->camera->focus_y += dy;
        State::get().ptr()->camera->adjusted_focus_x += dx;
        State::get().ptr()->camera->adjusted_focus_y += dy;
        State::get().ptr()->camera->calculated_focus_x += dx;
        State::get().ptr()->camera->calculated_focus_y += dy;
    }
}
