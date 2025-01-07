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
#include "entities_monsters.hpp"     //
#include "entity_hooks_info.hpp"     // for EntityHooksInfo
#include "entity_lookup.hpp"         //
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

void Entity::set_layer(LAYER layer_to)
{
    uint8_t dest_layer = enum_to_layer(layer_to);
    if (layer == dest_layer)
        return;

    auto& state = State::get();
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

void Entity::apply_layer()
{
    auto ptr_to = State::get().ptr()->layers[layer];

    using AddToLayer = void(Layer*, Entity*);
    static AddToLayer* add_to_layer = (AddToLayer*)get_address("add_to_layer");
    add_to_layer(ptr_to, this);

    for (auto item : items.entities())
    {
        item->apply_layer();
    }
}

void Entity::remove()
{
    if (layer != 2)
    {
        auto& state = State::get();
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

void Entity::perform_teleport(uint8_t delta_x, uint8_t delta_y)
{
    using TeleportFun = void(Entity*, uint8_t, uint8_t);
    static TeleportFun* tp = (TeleportFun*)get_address("teleport");
    tp(this, delta_x, delta_y);
}

Vec2 Entity::abs_position() const
{
    // if (abs_x != -FLT_MAX && abs_y != -FLT_MAX) // shortcut, if available
    //     return {abs_x, abs_y}; // using abs_x/y may have some issues https://github.com/spelunky-fyi/overlunky/issues/408

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

Vec2 Entity::get_absolute_velocity() const
{
    Vec2 velocity;
    if (is_movable())
    {
        auto mov = static_cast<const Movable*>(this);
        velocity.x = mov->velocityx;
        velocity.y = mov->velocityy;
    }
    else if (is_liquid())
    {
        auto liquid_engine = State::get().get_correct_liquid_engine(type->id);
        velocity.x = liquid_engine->entity_velocities->x;
        velocity.y = liquid_engine->entity_velocities->y;
    }

    if (overlay)
        velocity += overlay->get_absolute_velocity();

    return velocity;
}

AABB Entity::get_hitbox(std::optional<bool> use_render_pos) const
{
    Vec2 pos;
    if (use_render_pos.value_or(false) && rendering_info && !rendering_info->render_inactive)
    {
        pos.x = rendering_info->x;
        pos.y = rendering_info->y;
    }
    else
        pos = abs_position();

    return AABB{
        pos.x - hitboxx + offsetx,
        pos.y + hitboxy + offsety,
        pos.x + hitboxx + offsetx,
        pos.y - hitboxy + offsety,
    };
}

TEXTURE Entity::get_texture() const
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

bool Entity::is_player() const
{
    if (type->search_flags & 1)
    {
        auto pl = static_cast<const Player*>(this);
        return pl->ai == nullptr;
    }
    return false;
}

bool Entity::is_movable() const
{
    static const ENT_TYPE first_logical = to_id("ENT_TYPE_LOGICAL_CONSTELLATION");
    if (type->search_flags & 0b11111111) // PLAYER | MOUNT | MONSTER | ITEM | ROPE | EXPLOSION | FX | ACTIVEFLOOR
        return true;
    else if (type->search_flags & 0x1000) // LOGICAL - as it has some movable entities
        if (type->id < first_logical)     // actually check if it's not logical
            return true;

    return false;
}

bool Entity::is_liquid() const
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

Entity* get_entity_ptr(uint32_t uid)
{
    auto p = State::find(State::get().ptr(), uid);
    // if (IsBadWritePtr(p, 0x178))
    //     return nullptr;
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
        State::get().set_camera_position(dx, dy);
}

template <typename F>
bool recursive(Entity* ent, std::optional<uint32_t> mask, std::vector<ENT_TYPE> ent_types, RECURSIVE_MODE rec_mode, F func)
{
    auto acutal_mask = [](uint32_t m) -> uint32_t // for the MASK.ANY
    { return m == 0 ? 0xFFFF : m; };

    if (rec_mode == RECURSIVE_MODE::EXCLUSIVE)
    {
        if (mask.has_value() && (acutal_mask(mask.value()) & ent->type->search_flags) != 0)
            return false;

        if (std::find(ent_types.begin(), ent_types.end(), ent->type->id) != ent_types.end())
            return false;
    }
    else if (rec_mode == RECURSIVE_MODE::INCLUSIVE)
    {
        if (mask.has_value() && (acutal_mask(mask.value()) & ent->type->search_flags) == 0)
        {
            if (std::find(ent_types.begin(), ent_types.end(), ent->type->id) == ent_types.end())
                return false;
        }
        else if (std::find(ent_types.begin(), ent_types.end(), ent->type->id) == ent_types.end())
            return false;
    }
    const std::vector<Entity*> items{ent->items.entities().begin(), ent->items.entities().end()};
    for (auto entity : items)
    {
        recursive(entity, mask, ent_types, rec_mode, func);
    }

    {
        static const ENT_TYPE jellys[] = {
            to_id("ENT_TYPE_MONS_MEGAJELLYFISH"),
            to_id("ENT_TYPE_MONS_MEGAJELLYFISH_BACKGROUND"),
        };
        static const ENT_TYPE jellys_tails[] = {
            to_id("ENT_TYPE_FX_MEGAJELLYFISH_TAIL"),
            to_id("ENT_TYPE_FX_MEGAJELLYFISH_TAIL_BG"),
        };

        if (ent->type->id == jellys[0] || ent->type->id == jellys[1]) // special only for MEGAJELLYFISH
        {
            auto true_type = (MegaJellyfish*)ent;
            auto currend_uid = true_type->tail_bg_uid;
            for (int idx = 0; idx < 8; ++idx)
            {
                auto tail_ent = get_entity_ptr(currend_uid + idx);
                if (tail_ent != nullptr && (tail_ent->type->id == jellys_tails[0] || tail_ent->type->id == jellys_tails[1])) // only kill the tail
                {
                    recursive(tail_ent, mask, ent_types, rec_mode, func);
                }
            }
        }
    }
    func(ent);
    return true;
}

void Entity::kill_recursive(bool destroy_corpse, Entity* responsible, std::optional<uint32_t> mask, const std::vector<ENT_TYPE> ent_types, RECURSIVE_MODE rec_mode)
{
    auto kill_func = [destroy_corpse, &responsible](Entity* ent) -> void
    {
        ent->kill(destroy_corpse, responsible);
    };
    if (!recursive(this, mask, get_proper_types(ent_types), rec_mode, kill_func))
        kill(destroy_corpse, responsible);
}

void Entity::destroy_recursive(std::optional<uint32_t> mask, const std::vector<ENT_TYPE> ent_types, RECURSIVE_MODE rec_mode)
{
    auto destroy_func = [](Entity* ent) -> void
    {
        ent->destroy();
    };
    if (!recursive(this, mask, get_proper_types(ent_types), rec_mode, destroy_func))
        destroy();
}
