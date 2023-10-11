#include "rpc.hpp"

#include <Windows.h>        // for VirtualFree, MEM_RELEASE, GetCurrent...
#include <array>            // for array
#include <cmath>            // for round, pow, sqrt
#include <cstring>          // for size_t, memcpy
#include <detours.h>        // for DetourAttach, DetourTransactionBegin
#include <fmt/format.h>     // for check_format_string, format, vformat
#include <initializer_list> // for initializer_list
#include <iosfwd>           // for ofstream
#include <list>             // for _List_const_iterator
#include <map>              // for map, _Tree_iterator, _Tree_const_ite...
#include <memory>           // for remove
#include <new>              // for operator new
#include <set>              // for set, set<>::iterator
#include <span>             // for span
#include <string>           // for operator""sv, string, operator""s
#include <string_view>      // for string_view
#include <type_traits>      // for move, hash
#include <unordered_set>    // for _Uset_traits<>::allocator_type, _Use...
#include <utility>          // for min, max, pair, find

#include "custom_types.hpp"     // for get_custom_entity_types, CUSTOM_TYPE
#include "entities_chars.hpp"   // for Player (ptr only), PowerupCapable
#include "entities_floors.hpp"  // for ExitDoor, Door
#include "entities_items.hpp"   // for StretchChain, PunishBall, Container
#include "entities_liquids.hpp" // for Liquid
#include "entities_mounts.hpp"  // for Mount
#include "entity.hpp"           // for get_entity_ptr, to_id, Entity, EntityDB
#include "game_manager.hpp"     //
#include "game_patches.hpp"     //
#include "items.hpp"            // for Items
#include "layer.hpp"            // for EntityList, EntityList::Range, Layer
#include "logger.h"             // for DEBUG
#include "math.hpp"             // for AABB
#include "memory.hpp"           // for write_mem_prot, write_mem_recoverable
#include "movable.hpp"          // for Movable
#include "particles.hpp"        // for ParticleEmitterInfo
#include "search.hpp"           // for get_address, find_inst
#include "state.hpp"            // for State, get_state_ptr, enum_to_layer
#include "state_structs.hpp"    // for ShopRestrictedItem, Illumination
#include "thread_utils.hpp"     // for OnHeapPointer
#include "virtual_table.hpp"    // for get_virtual_function_address, VIRT_FUNC

uint32_t setflag(uint32_t flags, int bit) // shouldn't we change those to #define ?
{
    return flags | (1U << (bit - 1));
}
uint32_t clrflag(uint32_t flags, int bit)
{
    return flags & ~(1U << (bit - 1));
}
bool testflag(uint32_t flags, int bit)
{
    return (flags & (1U << (bit - 1))) > 0;
}
uint32_t flipflag(uint32_t flags, int bit)
{
    return (flags ^ (1U << (bit - 1)));
}

void attach_entity(Entity* overlay, Entity* attachee)
{
    if (attachee->overlay)
    {
        overlay->remove_item_ptr(attachee);
    }

    auto [x, y] = overlay->position();
    attachee->x -= x;
    attachee->y -= y;
    attachee->special_offsetx = attachee->x;
    attachee->special_offsety = attachee->y;
    attachee->overlay = overlay;

    using AddItemPtr = void(EntityList*, Entity*, bool);
    static AddItemPtr* add_item_ptr = (AddItemPtr*)get_address("add_item_ptr");
    add_item_ptr(&overlay->items, attachee, false);
}

void attach_entity_by_uid(uint32_t overlay_uid, uint32_t attachee_uid)
{
    if (Entity* overlay = get_entity_ptr(overlay_uid))
    {
        if (Entity* attachee = get_entity_ptr(attachee_uid))
        {
            attach_entity(overlay, attachee);
        }
    }
}

int32_t attach_ball_and_chain(uint32_t uid, float off_x, float off_y)
{
    if (Entity* entity = get_entity_ptr(uid))
    {
        static const auto ball_entity_type = to_id("ENT_TYPE_ITEM_PUNISHBALL");
        static const auto chain_entity_type = to_id("ENT_TYPE_ITEM_PUNISHCHAIN");

        auto [x, y, l] = get_position(uid);
        auto* layer_ptr = State::get().layer(l);

        PunishBall* ball = (PunishBall*)layer_ptr->spawn_entity(ball_entity_type, x + off_x, y + off_y, false, 0.0f, 0.0f, false);

        ball->attached_to_uid = uid;

        const uint8_t chain_length = 15;
        for (uint8_t i = 0; i < chain_length; i++)
        {
            StretchChain* chain = (StretchChain*)layer_ptr->spawn_entity(chain_entity_type, x, y, false, 0.0f, 0.0f, false);
            chain->animation_frame -= (i % 2);

            chain->at_end_of_chain_uid = ball->uid;
            chain->dot_offset = (float)i / chain_length;
            chain->position_in_chain = i;
            chain->inverse_doubled_position_in_chain = (chain_length - i) * 2;
        }
        return ball->uid;
    }
    return -1;
}

void stack_entities(uint32_t bottom_uid, uint32_t top_uid, const float (&offset)[2])
{
    if (Entity* bottom = get_entity_ptr(bottom_uid))
    {
        if (Entity* top = get_entity_ptr(top_uid))
        {
            if (top->overlay)
            {
                top->overlay->remove_item_ptr(top);
            }
            attach_entity(bottom, top);
            top->x = offset[0];
            top->y = offset[1];
            top->special_offsetx = offset[0];
            top->special_offsety = offset[1];
            if ((bottom->flags >> 0x10) & 0x1) // facing left
            {
                top->x *= -1.0f;
            }
        }
    }
}

int32_t get_grid_entity_at(float x, float y, LAYER layer)
{
    auto state = State::get();
    uint8_t actual_layer = enum_to_layer(layer);

    if (Entity* ent = state.layer(actual_layer)->get_grid_entity_at(x, y))
        return ent->uid;

    return -1;
}

void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
    {
        if (ent->is_liquid())
        {
            move_liquid_abs(uid, x, y, vx, vy);
        }
        else
        {
            ent->teleport_abs(x, y, vx, vy);
        }
    }
}

void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy, LAYER layer)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
    {
        std::pair<float, float> offset;
        enum_to_layer(layer, offset);
        if (ent->is_liquid())
        {
            move_liquid_abs(uid, offset.first + x, offset.second + y, vx, vy);
        }
        else
        {
            ent->teleport_abs(offset.first + x, offset.second + y, vx, vy);
            ent->set_layer(layer);
        }
    }
}

void move_liquid_abs(uint32_t uid, float x, float y, float vx, float vy)
{
    auto entity = get_entity_ptr(uid)->as<Liquid>();
    if (entity)
    {
        auto liquid_engine = State::get().get_correct_liquid_engine(entity->type->id);
        if (liquid_engine)
        {
            liquid_engine->entity_coordinates[*entity->liquid_id] = {x, y};
            liquid_engine->entity_velocities[*entity->liquid_id] = {vx, vy};
        }
    }
}

uint32_t get_entity_flags(uint32_t uid)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        return ent->flags;
    return 0;
}

void set_entity_flags(uint32_t uid, uint32_t flags)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        ent->flags = flags;
}

uint32_t get_entity_flags2(uint32_t uid)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        return ent->more_flags;
    return 0;
}

void set_entity_flags2(uint32_t uid, uint32_t flags)
{
    auto ent = get_entity_ptr(uid);
    if (ent)
        ent->more_flags = flags;
}

int get_entity_ai_state(uint32_t uid)
{
    auto ent = get_entity_ptr(uid)->as<Movable>();
    if (ent && ent->is_movable())
        return ent->move_state;
    return 0;
}

uint32_t get_level_flags()
{
    auto state = State::get();
    return state.flags();
}

void set_level_flags(uint32_t flags)
{
    auto state = State::get();
    state.set_flags(flags);
}

ENT_TYPE get_entity_type(uint32_t uid)
{
    auto entity = get_entity_ptr(uid);
    if (entity)
        return entity->type->id;

    return UINT32_MAX; // TODO: shouldn't this be 0?
}

std::vector<Player*> get_players(StateMemory* state)
{
    state = state != nullptr
                ? state
                : State::get().ptr();

    std::vector<Player*> found;
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state->items->player(i);
        if (player)
            found.push_back((Player*)player);
    }
    return found;
}

std::tuple<float, float, float, float> screen_aabb(float left, float top, float right, float bottom)
{
    auto [sx1, sy1] = State::screen_position(left, top);
    auto [sx2, sy2] = State::screen_position(right, bottom);
    return std::tuple{sx1, sy1, sx2, sy2};
}

float screen_distance(float x)
{
    auto a = State::screen_position(0, 0);
    auto b = State::screen_position(x, 0);
    return b.first - a.first;
}

std::vector<uint32_t> filter_entities(std::vector<uint32_t> entities, std::function<bool(Entity*)> predicate)
{
    std::vector<uint32_t> filtered_entities{std::move(entities)};
    auto filter_fun = [&](uint32_t uid)
    {
        if (Entity* entity = get_entity_ptr(uid))
        {
            return !predicate(entity);
        }
        return false;
    };
    std::erase_if(filtered_entities, filter_fun);
    return filtered_entities;
}

std::vector<uint32_t> get_entities()
{
    return get_entities_by({}, 0, LAYER::BOTH);
}

std::vector<uint32_t> get_entities_by_layer(LAYER layer)
{
    return get_entities_by({}, 0, layer);
}

std::vector<uint32_t> get_entities_by_type(std::vector<ENT_TYPE> entity_types)
{
    return get_entities_by(std::move(entity_types), 0, LAYER::BOTH);
}
std::vector<uint32_t> get_entities_by_type(ENT_TYPE entity_type)
{
    return get_entities_by(std::vector<ENT_TYPE>{entity_type}, 0, LAYER::BOTH);
}

std::vector<uint32_t> get_entities_by_mask(uint32_t mask)
{
    return get_entities_by({}, mask, LAYER::BOTH);
}

template <class FunT>
requires std::is_invocable_v<FunT, const EntityList&>
void foreach_mask(uint32_t mask, Layer* l, FunT&& fun)
{
    if (mask == 0)
    {
        fun(l->all_entities);
    }
    else
    {
        for (uint32_t test_flag = 1U; test_flag < 0x8000; test_flag <<= 1U)
        {
            if (mask & test_flag)
            {
                const auto& it = l->entities_by_mask.find(test_flag);
                if (it != l->entities_by_mask.end())
                {
                    fun(it->second);
                }
            }
        }
    }
}

std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));

    auto push_matching_types = [&proper_types, &found](const EntityList& entities)
    {
        for (auto& item : entities.entities())
        {
            if (entity_type_check(proper_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    };
    auto insert_all_uids = [&found](const EntityList& entities)
    {
        const auto uids = entities.uids();
        found.insert(found.end(), uids.begin(), uids.end());
    };

    if (layer == LAYER::BOTH)
    {
        if (proper_types.empty() || proper_types[0] == 0)
        {
            if (mask == 0) // all entities
            {
                // this exception for small improvments with calling reserve once
                found.reserve(found.size() + (size_t)state.layer(0)->all_entities.size + (size_t)state.layer(1)->all_entities.size);
                found.insert(found.end(), state.layer(0)->all_entities.uids().begin(), state.layer(0)->all_entities.uids().end());
                found.insert(found.end(), state.layer(1)->all_entities.uids().begin(), state.layer(1)->all_entities.uids().end());
            }
            else // all types
            {
                foreach_mask(mask, state.layer(0), insert_all_uids);
                foreach_mask(mask, state.layer(1), insert_all_uids);
            }
        }
        else
        {
            foreach_mask(mask, state.layer(0), push_matching_types);
            foreach_mask(mask, state.layer(1), push_matching_types);
        }
    }
    else
    {
        uint8_t correct_layer = enum_to_layer(layer);
        if (proper_types.empty() || proper_types[0] == 0) // all types
        {
            foreach_mask(mask, state.layer(correct_layer), insert_all_uids);
        }
        else
        {
            foreach_mask(mask, state.layer(correct_layer), push_matching_types);
        }
    }
    return found;
}
std::vector<uint32_t> get_entities_by(ENT_TYPE entity_type, uint32_t mask, LAYER layer)
{
    return get_entities_by(std::vector<ENT_TYPE>{entity_type}, mask, layer);
}

std::vector<uint32_t> get_entities_at(std::vector<ENT_TYPE> entity_types, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
    auto push_entities_at = [&x, &y, &radius, &proper_types, &found](const EntityList& entities)
    {
        for (auto& item : entities.entities())
        {
            auto [ix, iy] = item->position();
            float distance = sqrt(pow(x - ix, 2.0f) + pow(y - iy, 2.0f));
            if (distance < radius && entity_type_check(proper_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    };
    if (layer == LAYER::BOTH)
    {
        foreach_mask(mask, state.layer(0), push_entities_at);
        foreach_mask(mask, state.layer(1), push_entities_at);
    }
    else
    {
        foreach_mask(mask, state.layer(enum_to_layer(layer)), push_entities_at);
    }
    return found;
}
std::vector<uint32_t> get_entities_at(ENT_TYPE entity_type, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    return get_entities_at(std::vector<ENT_TYPE>{entity_type}, mask, x, y, layer, radius);
}

std::vector<uint32_t> get_entities_overlapping_hitbox(std::vector<ENT_TYPE> entity_types, uint32_t mask, AABB hitbox, LAYER layer)
{
    auto state = State::get();
    std::vector<uint32_t> result;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
    if (layer == LAYER::BOTH)
    {
        std::vector<uint32_t> result2;
        result = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(0));
        result2 = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(1));
        result.insert(result.end(), result2.begin(), result2.end());
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);
        result = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(actual_layer));
    }
    return result;
}
std::vector<uint32_t> get_entities_overlapping_hitbox(ENT_TYPE entity_type, uint32_t mask, AABB hitbox, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, hitbox, layer);
}

std::vector<uint32_t> get_entities_overlapping(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::move(entity_types), mask, {sx, sy2, sx2, sy}, layer);
}
std::vector<uint32_t> get_entities_overlapping(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, {sx, sy2, sx2, sy}, layer);
}

std::vector<uint32_t> get_entities_overlapping_by_pointer(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    std::vector<uint32_t> found;
    foreach_mask(mask, layer, [&entity_types, &found, &sx, &sy, &sx2, &sy2](const EntityList& entities)
                 {
                     for (auto& item : entities.entities())
                     {
                         if (entity_type_check(std::move(entity_types), item->type->id) && item->overlaps_with(sx, sy, sx2, sy2))
                         {
                             found.push_back(item->uid);
                         }
                     } });

    return found;
}
std::vector<uint32_t> get_entities_overlapping_by_pointer(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    return get_entities_overlapping_by_pointer(std::vector<ENT_TYPE>{entity_type}, mask, sx, sy, sx2, sy2, layer);
}

void set_door_target(uint32_t uid, uint8_t w, uint8_t l, uint8_t t)
{
    if (auto door = get_entity_ptr(uid)->as<ExitDoor>())
    {
        door->world = w;
        door->level = l;
        door->theme = t;
        door->special_door = true;
    }
}

std::tuple<uint8_t, uint8_t, uint8_t> get_door_target(uint32_t uid)
{
    auto door = get_entity_ptr(uid)->as<ExitDoor>();
    if (door == nullptr || !door->special_door)
        return std::make_tuple((uint8_t)0, (uint8_t)0, (uint8_t)0);

    return std::make_tuple(door->world, door->level, door->theme);
}

void set_contents(uint32_t uid, ENT_TYPE item_entity_type)
{
    Entity* container = get_entity_ptr(uid);
    if (container == nullptr)
        return;
    uint32_t type = container->type->id;
    static auto compatible_entities = {
        to_id("ENT_TYPE_ITEM_COFFIN"),
        to_id("ENT_TYPE_ITEM_CRATE"),
        to_id("ENT_TYPE_ITEM_DMCRATE"),
        to_id("ENT_TYPE_ITEM_PRESENT"),
        to_id("ENT_TYPE_ITEM_GHIST_PRESENT"),
        to_id("ENT_TYPE_ITEM_POT"),
        to_id("ENT_TYPE_ALIVE_EMBEDDED_ON_ICE")};

    if (std::find(compatible_entities.begin(), compatible_entities.end(), type) == compatible_entities.end())
        return;

    container->as<Container>()->inside = item_entity_type;
}

void entity_remove_item(uint32_t uid, uint32_t item_uid)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return;
    entity->remove_item(item_uid);
}

bool entity_has_item_uid(uint32_t uid, uint32_t item_uid)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;

    return entity->items.contains(item_uid);
};

bool entity_has_item_type(uint32_t uid, std::vector<ENT_TYPE> entity_types)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;
    if (entity->items.size > 0)
    {
        const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
        for (auto item : entity->items.entities())
        {
            if (entity_type_check(proper_types, item->type->id))
                return true;
        }
    }
    return false;
}
bool entity_has_item_type(uint32_t uid, ENT_TYPE entity_type)
{
    return entity_has_item_type(uid, std::vector<ENT_TYPE>{entity_type});
}

std::vector<uint32_t> entity_get_items_by(uint32_t uid, std::vector<ENT_TYPE> entity_types, uint32_t mask)
{
    std::vector<uint32_t> found;
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return found;
    if (entity->items.size > 0)
    {
        const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
        if ((!proper_types.size() || !proper_types[0]) && !mask) // all items
        {
            const auto uids = entity->items.uids();
            found.insert(found.end(), uids.begin(), uids.end());
        }
        else
        {
            for (auto item : entity->items.entities())
            {
                if ((mask == 0 || (item->type->search_flags & mask)) && entity_type_check(proper_types, item->type->id))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    return found;
}
std::vector<uint32_t> entity_get_items_by(uint32_t uid, ENT_TYPE entity_type, uint32_t mask)
{
    return entity_get_items_by(uid, std::vector<ENT_TYPE>{entity_type}, mask);
}

void lock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at({}, 0, x, y, LAYER::FRONT, 1);
    for (auto id : items)
    {
        Entity* door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags &= ~(1U << 19);
            door->flags |= 1U << 21;
        }
        else if (
            door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") ||
            door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
        {
            door->animation_frame &= ~1U;
        }
    }
}

void unlock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at({}, 0, x, y, LAYER::FRONT, 1);
    for (auto id : items)
    {
        Entity* door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags |= 1U << 19;
            door->flags &= ~(1U << 21);
        }
        else if (
            door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") ||
            door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
        {
            door->animation_frame |= 1U;
        }
    }
}

uint32_t get_frame_count_main()
{
    auto state = State::get();
    return state.get_frame_count_main();
}
uint32_t get_frame_count()
{
    auto state = State::get();
    return state.get_frame_count();
}

void carry(uint32_t mount_uid, uint32_t rider_uid)
{
    auto mount = get_entity_ptr(mount_uid)->as<Mount>();
    auto rider = get_entity_ptr(rider_uid)->as<Movable>();
    if (mount == nullptr || rider == nullptr)
        return;
    mount->carry(rider);
}

void kill_entity(uint32_t uid, std::optional<bool> destroy_corpse)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->kill(destroy_corpse.value_or(true), nullptr);
}

void destroy_entity(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->destroy();
}

void apply_entity_db(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->apply_db();
}

void flip_entity(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent == nullptr)
        return;
    ent->flags = flipflag(ent->flags, 17);
    if (ent->items.size > 0)
    {
        for (auto item : ent->items.entities())
        {
            item->flags = flipflag(item->flags, 17);
        }
    }
}

void set_camera_position(float cx, float cy)
{
    auto state = State::get();
    state.set_camera_position(cx, cy);
}

void warp(uint8_t world, uint8_t level, uint8_t theme)
{
    auto state = State::get();
    state.warp(world, level, theme);
}

void set_seed(uint32_t seed)
{
    auto state = State::get();
    state.set_seed(seed);
}

void set_arrowtrap_projectile(ENT_TYPE regular_entity_type, ENT_TYPE poison_entity_type)
{
    static const auto arrowtrap = get_address("arrowtrap_projectile");
    static const auto poison_arrowtrap = get_address("poison_arrowtrap_projectile");
    write_mem_prot(arrowtrap, regular_entity_type, true);
    write_mem_prot(poison_arrowtrap, poison_entity_type, true);
}

float* g_sparktrap_parameters{nullptr};
void modify_sparktraps(float angle_increment, float distance)
{
    if (g_sparktrap_parameters == nullptr)
    {
        static const auto offset = get_address("sparktrap_angle_increment") + 4;

        if (memory_read<uint8_t>(offset - 1) == 0x89) // check if sparktraps_hack is active
            return;

        const int32_t distance_offset = 0xF1;
        g_sparktrap_parameters = (float*)alloc_mem_rel32(offset + 4, sizeof(float) * 2);
        if (!g_sparktrap_parameters)
            return;

        int32_t rel = static_cast<int32_t>((size_t)g_sparktrap_parameters - (offset + 4));
        write_mem_prot(offset, rel, true);
        write_mem_prot(offset + distance_offset, (int32_t)(rel - distance_offset + sizeof(float)), true);
    }
    *g_sparktrap_parameters = angle_increment;
    *(g_sparktrap_parameters + 1) = distance;
}

float* get_sparktraps_parameters_ptr() // only for the UI
{
    return g_sparktrap_parameters;
}

void activate_sparktraps_hack(bool activate)
{
    if (activate)
    {
        static const auto offset = get_address("sparktrap_angle_increment");
        const int32_t distance_offset = 0xF1;

        write_mem_recoverable("sparktraps_hack", offset, "\xF3\x0F\x58\x89\x6C\x01\x00\x00"sv, true);
        write_mem_recoverable("sparktraps_hack", offset + distance_offset, "\xF3\x0F\x10\xB9\x70\x01\x00\x00"sv, true);
    }
    else
    {
        recover_mem("sparktraps_hack");
    }
}

void set_storage_layer(LAYER layer)
{
    static const auto storage_layer = get_address("storage_layer");
    if (layer == LAYER::FRONT || layer == LAYER::BACK)
        write_mem_prot(storage_layer, 0x1300 + 8 * (uint8_t)layer, true);
}

void set_kapala_blood_threshold(uint8_t threshold)
{
    static const auto kapala_blood_threshold = get_address("kapala_blood_threshold");
    write_mem_prot(kapala_blood_threshold, threshold, true);
}

void set_kapala_hud_icon(int8_t icon_index)
{
    static size_t instruction_offset = 0;
    static size_t icon_index_offset = 0;
    static uint32_t distance = 0;

    if (instruction_offset == 0)
    {
        instruction_offset = get_address("kapala_hud_icon");
        icon_index_offset = instruction_offset + 0x12;
        distance = static_cast<uint32_t>(icon_index_offset - (instruction_offset + 7));
    }

    if (icon_index < 0) // reset to original
    {
        write_mem_prot(instruction_offset + 2, 0x00013089, true);
    }
    else
    {
        // Instead of loading the value from KapalaPowerup:amount_of_blood (the instruction pointed at by instruction_offset)
        // we overwrite this with an instruction that loads a byte located a bit after the current function.
        // So you need to assemble `movzx  <relevant register>,BYTE PTR [rip+<distance>]`
        write_mem_prot(instruction_offset + 2, {0x0d}, true);
        write_mem_prot(instruction_offset + 3, distance, true);
        if (icon_index > 6)
        {
            icon_index = 6;
        }
        write_mem_prot(icon_index_offset, icon_index, true);
    }
}

void set_blood_multiplication(uint32_t /*default_multiplier*/, uint32_t vladscape_multiplier)
{
    // Due to changes in 1.23.x, the default multiplier is automatically vlads - 1.
    static const auto blood_multiplication = get_address("blood_multiplication");
    write_mem_prot(blood_multiplication, vladscape_multiplier, true);
}

std::vector<int64_t> read_prng()
{
    auto state = State::get();
    return state.read_prng();
}

void pick_up(uint32_t who_uid, uint32_t what_uid)
{
    Movable* ent = (Movable*)get_entity_ptr(who_uid);
    Movable* item = (Movable*)get_entity_ptr(what_uid);
    if (ent != nullptr && item != nullptr)
    {
        ent->pick_up(item);
    }
}

void drop(uint32_t who_uid, uint32_t what_uid)
{
    Movable* ent = (Movable*)get_entity_ptr(who_uid);
    Movable* item = (Movable*)get_entity_ptr(what_uid);
    if (ent != nullptr && item != nullptr)
    {
        ent->drop(item);
    }
}

void unequip_backitem(uint32_t who_uid)
{
    static size_t offset = get_address("unequip");

    if (offset != 0)
    {
        auto backitem_uid = worn_backitem(who_uid);
        if (backitem_uid != -1)
        {
            Movable* ent = (Movable*)get_entity_ptr(who_uid);
            Entity* backitem_ent = get_entity_ptr(backitem_uid);
            if (ent != nullptr && backitem_ent != nullptr)
            {
                typedef size_t unequip_func(Entity*, uint32_t);
                static unequip_func* uf = (unequip_func*)(offset);
                uf(ent, backitem_ent->type->id);
            }
        }
    }
}

int32_t worn_backitem(uint32_t who_uid)
{
    static const std::unordered_set<uint32_t> backitem_types = {
        to_id("ENT_TYPE_ITEM_JETPACK"),
        to_id("ENT_TYPE_ITEM_HOVERPACK"),
        to_id("ENT_TYPE_ITEM_POWERPACK"),
        to_id("ENT_TYPE_ITEM_TELEPORTER_BACKPACK"),
        to_id("ENT_TYPE_ITEM_CAPE"),
        to_id("ENT_TYPE_ITEM_VLADS_CAPE"),
    };

    auto ent = get_entity_ptr(who_uid)->as<PowerupCapable>();
    if (ent != nullptr)
    {
        for (const auto& [powerup_type, powerup_entity] : ent->powerups)
        {
            if (backitem_types.count(powerup_type) > 0)
            {
                return powerup_entity->uid;
            }
        }
    }
    return -1;
}

void set_olmec_phase_y_level(uint8_t phase, float y)
{
    // Sets the Y-level Olmec changes phases. The defaults are :
    // - phase 1 (bombs) = 100
    // - phase 2 (ufos) = 83
    // Olmecs checks phases in order! The means if you want ufo's from the start
    // you have to put both phase 1 and 2 at e.g. level 199
    // If you want to make Olmec stay in phase 0 (stomping) all the time, you can just set
    // the phase 1 y level to 70. Don't set it too low, from 1.25.0 onwards, Olmec's stomp
    // activation distance seems to be related to the y-level trigger point.
    static size_t phase1_offset;
    if (phase1_offset == 0)
    {
        // from 1.23.x onwards, there are now two instructions per phase that reference the y-level float
        const size_t phase_1_instruction_a = get_address("olmec_transition_phase_1_y_level");
        const size_t phase_1_instruction_b = phase_1_instruction_a + 0xd;

        const size_t phase_2_instruction_a = get_address("olmec_transition_phase_2_y_level");
        const size_t phase_2_instruction_b = phase_2_instruction_a + 0x11;
        phase1_offset = (size_t)alloc_mem_rel32(phase_2_instruction_b + 4, sizeof(float) * 2);
        if (!phase1_offset)
            return;

        auto phase2_offset = phase1_offset + 0x4;

        // write the default values to our new floats
        write_mem_prot(phase1_offset, 100.0f, true);
        write_mem_prot(phase2_offset, 83.0f, true);

        // calculate the distances between our floats and the movss instructions
        auto distance_1_a = static_cast<int32_t>(phase1_offset - phase_1_instruction_a);
        auto distance_1_b = static_cast<int32_t>(phase1_offset - phase_1_instruction_b);
        auto distance_2_a = static_cast<int32_t>(phase2_offset - phase_2_instruction_a);
        auto distance_2_b = static_cast<int32_t>(phase2_offset - phase_2_instruction_b);

        // overwrite the movss instructions to load our floats
        write_mem_prot(phase_1_instruction_a - 4, distance_1_a, true);
        write_mem_prot(phase_1_instruction_b - 4, distance_1_b, true);
        write_mem_prot(phase_2_instruction_a - 4, distance_2_a, true);
        write_mem_prot(phase_2_instruction_b - 4, distance_2_b, true);
    }

    if (phase == 1)
    {
        *(float*)phase1_offset = y;
    }
    else if (phase == 2)
    {
        *(float*)(phase1_offset + sizeof(float)) = y;
    }
}

void force_olmec_phase_0(bool b)
{
    static size_t offset = get_address("olmec_transition_phase_1");

    if (b)
    {
        write_mem_recoverable("force_olmec_phase_0", offset, "\xEB\x2E"s, true); // jbe -> jmp
    }
    else
    {
        recover_mem("force_olmec_phase_0");
    }
}

void set_ghost_spawn_times(uint32_t normal, uint32_t cursed)
{
    static const auto ghost_spawn_time = get_address("ghost_spawn_time");
    static const auto ghost_spawn_time_cursed_p1 = get_address("ghost_spawn_time_cursed_player1");
    static const auto ghost_spawn_time_cursed_p2 = get_address("ghost_spawn_time_cursed_player2");
    static const auto ghost_spawn_time_cursed_p3 = get_address("ghost_spawn_time_cursed_player3");
    static const auto ghost_spawn_time_cursed_p4 = get_address("ghost_spawn_time_cursed_player4");

    write_mem_prot(ghost_spawn_time, normal, true);
    write_mem_prot(ghost_spawn_time_cursed_p1, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p2, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p3, cursed, true);
    write_mem_prot(ghost_spawn_time_cursed_p4, cursed, true);
}

void set_time_ghost_enabled(bool b)
{
    static size_t offset_trigger = 0;
    static size_t offset_toast_trigger = 0;
    if (offset_trigger == 0)
    {
        auto memory = Memory::get();
        offset_trigger = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TRIGGER, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
        offset_toast_trigger = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_GHOST_TOAST_TRIGGER, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
    }
    if (b)
    {
        recover_mem("set_time_ghost_enabled");
    }
    else
    {
        write_mem_recoverable("set_time_ghost_enabled", offset_trigger, "\xC3\x90\x90\x90"s, true);
        write_mem_recoverable("set_time_ghost_enabled", offset_toast_trigger, "\xC3\x90\x90\x90"s, true);
    }
}

void set_time_jelly_enabled(bool b)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        offset = memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::LOGIC_COSMIC_OCEAN, static_cast<uint32_t>(VIRT_FUNC::LOGIC_PERFORM)));
    }
    if (b)
    {
        recover_mem("set_time_jelly_enabled");
    }
    else
    {
        write_mem_recoverable("set_time_jelly_enabled", offset, "\xC3\x90\x90\x90"s, true);
    }
}

bool is_inside_active_shop_room(float x, float y, LAYER layer)
{
    static size_t offset = get_address("coord_inside_active_shop_room");
    if (offset != 0)
    {
        typedef bool coord_inside_shop_func(StateMemory*, uint32_t layer, float x, float y);
        static coord_inside_shop_func* cisf = (coord_inside_shop_func*)(offset);
        return cisf(get_state_ptr(), enum_to_layer(layer), x, y);
    }
    return false;
}

bool is_inside_shop_zone(float x, float y, LAYER layer)
{
    static size_t offset = 0;
    static void* rcx = nullptr;
    if (offset == 0)
    {
        offset = get_address("coord_inside_shop_zone");
        size_t* tmp = (size_t*)get_address("coord_inside_shop_zone_rcx");
        auto heap_ptr = OnHeapPointer<void*>(*tmp);
        rcx = heap_ptr.decode();
    }
    if (offset != 0)
    {
        typedef bool coord_inside_shop_zone_func(void*, uint32_t layer, float x, float y);
        static coord_inside_shop_zone_func* ciszf = (coord_inside_shop_zone_func*)(offset);
        return ciszf(rcx, enum_to_layer(layer), x, y);
    }
    return false;
}

ParticleEmitterInfo* generate_world_particles(uint32_t particle_emitter_id, uint32_t uid)
{
    static size_t offset = get_address("generate_world_particles");

    if (offset != 0)
    {
        auto entity = get_entity_ptr(uid);
        if (entity != nullptr)
        {
            auto state = get_state_ptr();
            typedef ParticleEmitterInfo* generate_particles_func(std::vector<ParticleEmitterInfo*>*, uint32_t, Entity*);
            static generate_particles_func* gpf = (generate_particles_func*)(offset);
            return gpf(state->particle_emitters, particle_emitter_id, entity);
        }
    }
    return nullptr;
}

ParticleEmitterInfo* generate_screen_particles(uint32_t particle_emitter_id, float x, float y)
{
    static size_t offset = get_address("generate_screen_particles");

    if (offset != 0)
    {
        typedef ParticleEmitterInfo* generate_particles_func(uint32_t, float, float, size_t);
        static generate_particles_func* gpf = (generate_particles_func*)(offset);
        return gpf(particle_emitter_id, x, y, 0);
    }
    return nullptr;
}

void advance_screen_particles(ParticleEmitterInfo* particle_emitter)
{
    static size_t offset = get_address("advance_screen_particles");

    if (offset != 0)
    {
        typedef void advance_particles_func(ParticleEmitterInfo*);
        static advance_particles_func* apf = (advance_particles_func*)(offset);
        apf(particle_emitter);
    }
}

void render_screen_particles(ParticleEmitterInfo* particle_emitter)
{
    static size_t offset = get_address("render_screen_particles");

    if (offset != 0)
    {
        typedef void render_particles_func(ParticleEmitterInfo*, size_t, size_t, size_t);
        static render_particles_func* rpf = (render_particles_func*)(offset);
        rpf(particle_emitter, 0, 0, 0);
    }
}

void extinguish_particles(ParticleEmitterInfo* particle_emitter)
{
    // removing from state only applies to world emitters, but it just won't find the screen one in the vector, so no big deal
    auto state = get_state_ptr();
    std::erase(*state->particle_emitters, particle_emitter);

    using generic_free_func = void(void*);
    static generic_free_func* generic_free = (generic_free_func*)get_address("generic_free");

    if (particle_emitter != nullptr)
    {
        generic_free(particle_emitter->emitted_particles.memory);
        generic_free(particle_emitter->emitted_particles_back_layer.memory);
        generic_free(particle_emitter);
    }
}

Illumination* create_illumination_internal(Color color, float size, float x, float y, int32_t uid)
{
    static size_t offset = get_address("generate_illumination");

    if (offset != 0)
    {
        auto state = get_state_ptr();

        float position[] = {x, y};

        typedef Illumination* create_illumination_func(std::vector<Illumination*>*, float*, Color*, uint32_t r9, float size, uint8_t flags, uint32_t uid, uint8_t);
        static create_illumination_func* cif = (create_illumination_func*)(offset);
        auto emitted_light = cif(state->lightsources, position, &color, 0x2, size, 32, uid, 0x0);

        // turn on Enabled flag
        emitted_light->flags = emitted_light->flags | (1U << (25 - 1));

        return emitted_light;
    }
    return nullptr;
}

Illumination* create_illumination(Color color, float size, float x, float y)
{
    return create_illumination_internal(color, size, x, y, -1);
}

Illumination* create_illumination(Color color, float size, uint32_t uid)
{
    auto entity = get_entity_ptr(uid);
    if (entity != nullptr)
    {
        return create_illumination_internal(color, size, entity->abs_x, entity->abs_y, uid);
    }
    return nullptr;
}

void refresh_illumination(Illumination* illumination)
{
    static uint32_t* offset = 0;
    if (offset == 0)
    {
        size_t** heap_offset = (size_t**)get_address("refresh_illumination_heap_offset");
        auto illumination_counter = OnHeapPointer<uint32_t>(**heap_offset);
        offset = illumination_counter.decode();
    }
    illumination->timer = *offset;
}

void set_journal_enabled(bool b)
{
    get_journal_enabled() = b;
}

void set_camp_camera_bounds_enabled(bool b)
{
    static const size_t offset = get_address("enforce_camp_camera_bounds");
    if (b)
    {
        recover_mem("camp_camera_bounds");
    }
    else
    {
        write_mem_recoverable("camp_camera_bounds", offset, "\xC3\x90\x90"s, true);
    }
}

void set_explosion_mask(int32_t mask)
{
    static const size_t addr = get_address("explosion_mask");
    if (mask == -1)
    {
        recover_mem("explosion_mask");
    }
    else
    {
        write_mem_recoverable("explosion_mask", addr, mask, true);
    }
}

void set_max_rope_length(uint8_t length)
{
    uint32_t length_32 = length;
    static const auto attach_thrown_rope = get_address("attach_thrown_rope_to_background");
    static const auto process_ropes_one = get_address("process_ropes_one");
    static const auto process_ropes_two = get_address("process_ropes_two");
    static const auto process_ropes_three = get_address("process_ropes_three");

    // there's four instances where the max (default=6) is used

    // 1) When throwing a rope and it attaches to the background, the initial entity is
    // given a start value in its segment_nr_inverse variable
    write_mem_prot(attach_thrown_rope, length_32, true);

    // 2) and 3) at the top of the rope processing function are two comparisons to the max
    write_mem_prot(process_ropes_one, length, true);
    write_mem_prot(process_ropes_two, length, true);

    // 4) in the same function at the end of the little loop of process_ropes_two is a comparison to n-1
    uint8_t length_minus_one_8 = length - 1;
    write_mem_prot(process_ropes_three, length_minus_one_8, true);
}

uint8_t get_max_rope_length()
{
    static const auto address = get_address("attach_thrown_rope_to_background");
    return static_cast<uint8_t>(memory_read<uint32_t>(address));
}

uint8_t waddler_count_entity(ENT_TYPE entity_type)
{
    auto state = get_state_ptr();
    uint8_t count = 0;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (state->waddler_storage[x] == entity_type)
        {
            count++;
        }
    }
    return count;
}

int8_t waddler_store_entity(ENT_TYPE entity_type)
{
    auto state = get_state_ptr();
    int8_t item_stored_in_slot = -1;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (state->waddler_storage[x] == 0)
        {
            state->waddler_storage[x] = entity_type;
            item_stored_in_slot = x;
            break;
        }
    }
    return item_stored_in_slot;
}

void waddler_remove_entity(ENT_TYPE entity_type, uint8_t amount_to_remove)
{
    auto state = get_state_ptr();

    uint8_t remove_count = 0;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (amount_to_remove == remove_count)
        {
            break;
        }

        if (state->waddler_storage[x] == entity_type)
        {
            state->waddler_storage[x] = 0;
            remove_count++;
        }
    }

    if (remove_count > 0)
    {
        uint32_t tmp[99] = {0};
        uint8_t tmp_x = 0;
        for (uint8_t x = 0; x < 99; ++x)
        {
            if (state->waddler_storage[x] != 0)
            {
                tmp[tmp_x++] = state->waddler_storage[x];
            }
        }
        memcpy(&(state->waddler_storage[0]), tmp, 99 * sizeof(uint32_t));
    }
}

int16_t waddler_get_entity_meta(uint8_t slot)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        return state->waddler_storage_meta[slot];
    }
    return 0;
}

void waddler_set_entity_meta(uint8_t slot, int16_t meta)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        state->waddler_storage_meta[slot] = meta;
    }
}

uint32_t waddler_entity_type_in_slot(uint8_t slot)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        return state->waddler_storage[slot];
    }
    return 0;
}

bool entity_type_check(const std::vector<ENT_TYPE>& types_array, const ENT_TYPE find)
{
    if (types_array.empty() || types_array[0] == 0 || std::find(types_array.begin(), types_array.end(), find) != types_array.end())
        return true;

    return false;
}

std::vector<ENT_TYPE> get_proper_types(std::vector<ENT_TYPE> ent_types)
{
    for (size_t i = 0; i < ent_types.size(); i++)
    {
        if (ent_types[i] >= (uint32_t)CUSTOM_TYPE::ACIDBUBBLE)
        {
            auto extra_types = get_custom_entity_types(static_cast<CUSTOM_TYPE>(ent_types[i]));
            if (extra_types.size() == 1)
            {
                ent_types[i] = extra_types[0];
            }
            else if (!extra_types.empty())
            {
                auto it = ent_types.begin() + i;
                it = ent_types.erase(it);
                ent_types.insert(it, extra_types.begin(), extra_types.end());
                i += extra_types.size() - 1;
            }
        }
    }
    return ent_types;
}

void enter_door(int32_t player_uid, int32_t door_uid)
{
    auto player = get_entity_ptr(player_uid);
    auto door = get_entity_ptr(door_uid)->as<Door>();
    if (player == nullptr || door == nullptr)
        return;

    door->enter(player);
}

void change_sunchallenge_spawns(std::vector<ENT_TYPE> ent_types)
{
    static bool modified = false;

    uint32_t ent_types_size = static_cast<uint32_t>(ent_types.size());
    static const auto offset = get_address("sun_chalenge_generator_ent_types");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(offset) + offset + 4);

    if (ent_types_size == 0)
    {
        if (modified)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        recover_mem("sunchallenge_spawn");
        modified = false;
        return;
    }

    const uint8_t old_size = ((memory_read<uint8_t>(offset - 4)) >> 2) + 1;

    if (ent_types_size >= 32)
        ent_types_size = 32;
    else if ((ent_types_size & (ent_types_size - 1))) // if the size is not power of 2
    {
        auto get_previous_power_of_two = [](uint32_t x)
        {
            x = x | (x >> 1);
            x = x | (x >> 2);
            x = x | (x >> 4);
            x = x | (x >> 8);
            x = x | (x >> 16);
            return x ^ (x >> 1);
        };
        ent_types_size = get_previous_power_of_two(ent_types_size);
    }

    if (old_size == ent_types_size)
    {
        for (uint32_t i = 0; i < ent_types_size; ++i)
            write_mem_recoverable("sunchallenge_spawn", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }

    const auto data_size = ent_types_size * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(offset + 4, data_size);
    if (new_array)
    {
        if (modified)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (offset + 4));
        write_mem_recoverable("sunchallenge_spawn", offset, rel, true);

        // the game does bitwise "and" with value 12 (0xC), so it would get 0, 4, 8 or 12 (4 positions in table)
        int8_t new_value = static_cast<int8_t>((ent_types_size - 1) << 2);
        write_mem_recoverable("sunchallenge_spawn", offset - 4, new_value, true);
        modified = true;
    }
}

void change_diceshop_prizes(std::vector<ENT_TYPE> ent_types)
{
    static const auto offset = get_address("dice_shop_prizes_id_roll");
    static const auto array_offset = get_address("dice_shop_prizes");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);
    bool original_instr = (memory_read<uint8_t>(offset) == 0x89);

    if (ent_types.size() > 255 || ent_types.size() < 6) // has to be min 6 as the game needs 6 uniqe item ids for prize_dispenser
    {
        if (!ent_types.size())
        {
            if (!original_instr)
                VirtualFree(old_types_array, 0, MEM_RELEASE);

            recover_mem("diceshop_prizes");
        }
        return;
    }

    if ((original_instr && ent_types.size() == 25) ||                              // if it's the unchanged instruction and we set the same number of ent_type's
        (!original_instr && memory_read<uint8_t>(offset + 5) == ent_types.size())) // or new instruction but the same size
    {
        for (unsigned int i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("diceshop_prizes", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }

    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);

    if (new_array)
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("diceshop_prizes", array_offset, rel, true);

        if (original_instr)
        {
            std::string new_code = fmt::format("\x50\x31\xC0\x41\xB3{}\x88\xD0\x41\xF6\xF3\x88\xE2\x58"sv, to_le_bytes((uint8_t)ent_types.size()));
            // push rax
            // xor eax, eax
            // mov r11b, (size)
            // mov al, dl
            // divb r11b
            // mov dl, ah
            // pop rax
            write_mem_recoverable("diceshop_prizes", offset, new_code, true);
        }
        else
        {
            write_mem_recoverable("diceshop_prizes", offset + 5, (uint8_t)ent_types.size(), true);
        }
    }
}

void change_altar_damage_spawns(std::vector<ENT_TYPE> ent_types)
{
    if (ent_types.size() > 255)
        return;

    static const auto array_offset = get_address("altar_break_ent_types");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);
    const auto code_offset = array_offset + 0xDD;
    const auto instruction_shr = array_offset + 0x13D;
    const auto instruction_to_modifiy = array_offset + 0x204;
    const auto original_instr = (memory_read<uint8_t>(instruction_shr) == 0x41);
    if (ent_types.empty())
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        recover_mem("altar_damage_spawn");
        return;
    }
    if (!original_instr && memory_read<uint8_t>(code_offset + 2) == ent_types.size())
    {
        // original array is used for something else as well, so i never edit that content
        for (uint32_t i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("altar_damage_spawn", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }
    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);
    if (new_array)
    {
        if (!original_instr)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("altar_damage_spawn", array_offset, rel, true);

        if (original_instr)
        {
            std::string new_code = fmt::format("\x41\xB1{}\x48\xC1\xE8\x38\x41\xF6\xF1\x49\x89\xC1"sv, to_le_bytes((uint8_t)ent_types.size()));
            // mov R9b, (size)
            // shr RAX, 0x38
            // divb R9b
            // mov R9, RAX
            write_mem_recoverable("altar_damage_spawn", code_offset, new_code, true);
            write_mem_recoverable("altar_damage_spawn", instruction_shr, "\x49\xC1\xE9\x08"sv, true); // shr r9,0x8
            write_mem_recoverable("altar_damage_spawn", instruction_to_modifiy, (uint8_t)0x8C, true); // r9+r12 => r12+r9*4
        }
        else
        {
            write_mem_recoverable("altar_damage_spawn", code_offset + 2, (uint8_t)ent_types.size(), true);
        }
    }
}

void change_waddler_drop(std::vector<ENT_TYPE> ent_types)
{
    static bool modified = false;

    static const auto offset = get_address("waddler_drop_size");
    static const auto array_offset = get_address("waddler_drop_array");
    ENT_TYPE* old_types_array = (ENT_TYPE*)(memory_read<int32_t>(array_offset) + array_offset + 4);

    if (ent_types.size() > 255 || ent_types.size() < 1)
    {
        if (!ent_types.size())
        {
            if (modified)
                VirtualFree(old_types_array, 0, MEM_RELEASE);

            recover_mem("waddler_drop");
            modified = false;
        }
        return;
    }

    if ((!modified && ent_types.size() == 3) ||                         // if it's the unchanged instruction and we set the same number of ent_type's
        (modified && memory_read<uint8_t>(offset) == ent_types.size())) // or new instruction but the same size
    {
        for (unsigned int i = 0; i < ent_types.size(); ++i)
            write_mem_recoverable("waddler_drop", (size_t)&old_types_array[i], ent_types[i], true);

        return;
    }

    const auto data_size = ent_types.size() * sizeof(ENT_TYPE);
    ENT_TYPE* new_array = (ENT_TYPE*)alloc_mem_rel32(array_offset + 4, data_size);

    if (new_array)
    {
        if (modified)
            VirtualFree(old_types_array, 0, MEM_RELEASE);

        memcpy(new_array, ent_types.data(), data_size);
        int32_t rel = static_cast<int32_t>((size_t)new_array - (array_offset + 4));
        write_mem_recoverable("waddler_drop", array_offset, rel, true);
        write_mem_recoverable("waddler_drop", offset, (uint8_t)ent_types.size(), true);
        modified = true;
    }
}

void poison_entity(int32_t entity_uid)
{
    auto ent = get_entity_ptr(entity_uid);
    if (ent)
    {
        using PoisonEntity_fun = void(Entity*, bool);
        static auto poison_entity = (PoisonEntity_fun*)get_address("poison_entity");
        poison_entity(ent, true);
    }
}

void modify_ankh_health_gain(uint8_t health, uint8_t beat_add_health)
{
    static size_t offsets[4];
    static auto size_minus_one = get_address("ankh_health");
    if (!health)
    {
        recover_mem("ankh_health");
        return;
    }
    if (size_minus_one && beat_add_health)
    {
        if (!offsets[0])
        {
            auto memory = Memory::get();
            size_t offset = size_minus_one - memory.exe_ptr;
            const auto limit_size = offset + 0x200;

            offsets[0] = find_inst(memory.exe(), "\x41\x80\xBF\x17\x01\x00\x00"sv, offset, limit_size, "ankh_health_gain_1");
            offsets[1] = find_inst(memory.exe(), "\x41\x80\xBF\x17\x01\x00\x00"sv, offsets[0] + 7, limit_size, "ankh_health_gain_2");
            offsets[2] = find_inst(memory.exe(), "\x0F\x42\xCA\x83\xC0"sv, offset, limit_size, "ankh_health_gain_3");
            offsets[3] = find_inst(memory.exe(), "\x8A\x83\x17\x01\x00\x00\x3C"sv, offset, std::nullopt, "ankh_health_gain_4"); // this is some bs
            if (!offsets[0] || !offsets[1] || !offsets[2] || !offsets[3])
            {
                offsets[0] = 0;
                return;
            }
            offsets[0] = memory.at_exe(offsets[0] + 7); // add pattern size
            offsets[1] = memory.at_exe(offsets[1] + 7);
            offsets[2] = memory.at_exe(offsets[2] + 5);
            offsets[3] = memory.at_exe(offsets[3] + 7);
        }
        const uint8_t game_maxhp = memory_read<uint8_t>(offsets[2] - 14);
        if (health > game_maxhp)
            health = game_maxhp;

        if (health % beat_add_health == 0)
        {
            write_mem_recoverable("ankh_health", size_minus_one, (uint8_t)(health - 1), true);
            write_mem_recoverable("ankh_health", offsets[0], health, true);
            write_mem_recoverable("ankh_health", offsets[1], health, true);
            write_mem_recoverable("ankh_health", offsets[2], beat_add_health, true);
            if (health < 4)
            {
                write_mem_recoverable("ankh_health", offsets[3], (uint8_t)0, true);
            }
            else
            {
                if (memory_read<uint8_t>(offsets[3]) != 3)
                    recover_mem("ankh_health", offsets[3]);
            }
        }
    }
}

void move_grid_entity(int32_t uid, float x, float y, LAYER layer)
{
    if (auto entity = get_entity_ptr(uid))
    {
        auto state = State::get();
        std::pair<float, float> offset;
        const auto actual_layer = enum_to_layer(layer, offset);
        state.layer(entity->layer)->move_grid_entity(entity, offset.first + x, offset.first + y, state.layer(actual_layer));
        entity->teleport_abs(offset.first + x, offset.first + y, 0, 0);
        entity->set_layer(layer);
    }
}

void destroy_grid(int32_t uid)
{
    if (auto entity = get_entity_ptr(uid))
    {
        auto state = State::get();
        state.layer(entity->layer)->destroy_grid_entity(entity);
    }
}

void destroy_grid(float x, float y, LAYER layer)
{
    auto state = State::get();
    uint8_t actual_layer = enum_to_layer(layer);

    if (Entity* entity = state.layer(actual_layer)->get_grid_entity_at(x, y))
    {
        state.layer(entity->layer)->destroy_grid_entity(entity);
    }
}

void add_item_to_shop(int32_t item_uid, int32_t shop_owner_uid)
{
    Movable* item = get_entity_ptr(item_uid)->as<Movable>();
    Entity* owner = get_entity_ptr(shop_owner_uid);
    if (item && owner && item->is_movable())
    {
        const static auto room_owners = {
            to_id("ENT_TYPE_MONS_SHOPKEEPER"),
            to_id("ENT_TYPE_MONS_MERCHANT"),
            to_id("ENT_TYPE_MONS_YANG"),
            to_id("ENT_TYPE_MONS_MADAMETUSK"),
            to_id("ENT_TYPE_MONS_STORAGEGUY"),
            to_id("ENT_TYPE_MONS_CAVEMAN_SHOPKEEPER"), // exception: not actually room owner
            to_id("ENT_TYPE_MONS_GHIST_SHOPKEEPE"),    // exception: not actually room owner
        };
        for (auto& it : room_owners)
        {
            if (owner->type->id == it) // TODO: check what happens if it's not room owner/shopkeeper
            {
                auto state = State::get();
                item->flags = setflag(item->flags, 23); // shop item
                item->flags = setflag(item->flags, 20); // Enable button prompt (flag is problably: show dialogs and other fx)
                state.layer_local(item->layer)->spawn_entity_over(to_id("ENT_TYPE_FX_SALEICON"), item, 0, 0);
                state.layer_local(item->layer)->spawn_entity_over(to_id("ENT_TYPE_FX_SALEDIALOG_CONTAINER"), item, 0, 0.5);

                ItemOwnerDetails iod{shop_owner_uid, owner->type->id};
                state.ptr()->room_owners.owned_items.insert({item->uid, iod});
                return;
            }
        }
    }
}

void change_poison_timer(int16_t frames)
{
    static const size_t offset_first = get_address("first_poison_tick_timer_default");
    static const size_t offset_subsequent = get_address("subsequent_poison_tick_timer_default");

    if (frames == -1)
    {
        recover_mem("change_poison_timer");
    }
    else
    {
        write_mem_recoverable("change_poison_timer", offset_first, frames, true);
        write_mem_recoverable("change_poison_timer", offset_subsequent, frames, true);
    }
}

void set_adventure_seed(int64_t first, int64_t second)
{
    static const size_t offset = get_address("adventure_seed");
    if (offset != 0)
    {
        write_mem_prot(offset, first, true);
        write_mem_prot(offset + 8, second, true);
    }
}

std::pair<int64_t, int64_t> get_adventure_seed()
{
    static const size_t offset = get_address("adventure_seed");
    if (offset != 0)
    {
        return {memory_read<int64_t>(offset), memory_read<int64_t>(offset + 8)};
    }
    return {0, 0};
}

void update_liquid_collision_at(float x, float y, bool add)
{
    using UpdateLiquidCollision = void(LiquidPhysics*, int32_t, int32_t, bool); // setting last parameter to true just skips the whole function
    static UpdateLiquidCollision* RemoveLiquidCollision_fun = (UpdateLiquidCollision*)get_address("remove_from_liquid_collision_map");
    static UpdateLiquidCollision* AddLiquidCollision_fun = (UpdateLiquidCollision*)get_address("add_from_liquid_collision_map");
    auto state = get_state_ptr();

    if (add)
        AddLiquidCollision_fun(state->liquid_physics, static_cast<int32_t>(std::round(x)), static_cast<int32_t>(std::round(y)), false);
    else
        RemoveLiquidCollision_fun(state->liquid_physics, static_cast<int32_t>(std::round(x)), static_cast<int32_t>(std::round(y)), false);
}

bool disable_floor_embeds(bool disable)
{
    static const auto address = get_address("spawn_floor_embeds");
    const bool current_value = memory_read<uint8_t>(address) == 0xc3;
    if (disable)
    {
        write_mem_recoverable("disable_floor_embeds", address, "\xC3"sv, true);
    }
    else
    {
        recover_mem("disable_floor_embeds");
    }
    return current_value;
}

void set_cursepot_ghost_enabled(bool enable)
{
    static const auto address = get_address("ghost_jar_ghost_spawn");
    if (!enable)
    {
        write_mem_recoverable("ghost_jar_ghost_spawn", address, "\x90\x90\x90\x90\x90"sv, true);
    }
    else
    {
        recover_mem("ghost_jar_ghost_spawn");
    }
}

void game_log(std::string message)
{
    using GameLogFun = void(std::ofstream*, const char*, void*, LogLevel);
    static const auto game_log_fun = (GameLogFun*)get_address("game_log_function");
    static const auto log_stream = (std::ofstream*)memory_read<int64_t>(get_address("game_log_stream"));
    game_log_fun(log_stream, message.c_str(), nullptr, LogLevel::Info);
}

void call_death_screen()
{
    using DeathScreen = void();
    static auto death_screen = (DeathScreen*)get_address("death_screen");
    death_screen();
}

void save_progress()
{
    using SaveProgress = void(SaveRelated*);
    static auto save_game_to_file = (SaveProgress*)get_address("save_progress");
    static auto gm = get_game_manager();
    save_game_to_file(gm->save_related);
}

void set_level_string(std::u16string_view text)
{
    static const auto hud_text_address = get_address("hud_level_text");
    static const auto journal_text_address = get_address("journal_level_text");
    static const auto journal_map_text_address = get_address("journal_map_level_text");
    static char16_t* data;
    static size_t text_data_length = 0;

    if (text_data_length == 0 || text_data_length < text.length())
    {
        if (text_data_length != 0)
        {
            VirtualFree(data, 0, MEM_RELEASE);
        }
        text_data_length = text.length() == 0 ? 1 : text.length(); // just to make sure it's not set to 0

        auto new_array_offset = hud_text_address;
        if (journal_text_address > new_array_offset)
        {
            new_array_offset = journal_text_address;
        }
        if (journal_map_text_address > new_array_offset)
        {
            new_array_offset = journal_map_text_address;
        }

        data = (char16_t*)alloc_mem_rel32(new_array_offset + 4, (text_data_length + 5) * sizeof(char16_t));
        *data = 0x25; // for the theme name in the journal map
        *(data + 1) = 0x6C;
        *(data + 2) = 0x73;
        *(data + 3) = 0x0A;

        const int32_t hud_rel = static_cast<int32_t>((size_t)(data + 4) - (hud_text_address + 4));
        const int32_t journal_rel = static_cast<int32_t>((size_t)(data + 4) - (journal_text_address + 4));
        const int32_t journal_map_rel = static_cast<int32_t>((size_t)(data) - (journal_map_text_address + 4));

        write_mem_prot(hud_text_address, hud_rel, true);
        write_mem_prot(journal_text_address, journal_rel, true);
        write_mem_prot(journal_map_text_address, journal_map_rel, true);
    }
    memcpy(data + 4, text.data(), text.length() * sizeof(char16_t));
    *(data + 4 + text.length()) = NULL;
}

void set_ending_unlock(ENT_TYPE type)
{
    static const ENT_TYPE first = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
    static const ENT_TYPE last = to_id("ENT_TYPE_CHAR_CLASSIC_GUY");
    if (type >= first && type <= last)
    {
        static const auto offset = get_address("ending_unlock");
        const int32_t char_offset = 10;

        write_mem_recoverable("ending_unlock", offset, "\x90\x90\x90\x90\x90\x90\x90\x90"sv, true);
        write_mem_recoverable("ending_unlock", offset + char_offset, type, true);
    }
    else
    {
        recover_mem("ending_unlock");
    }
}

void set_olmec_cutscene_enabled(bool enable)
{
    set_skip_olmec_cutscene(!enable);
}

void set_tiamat_cutscene_enabled(bool enable)
{
    set_skip_tiamat_cutscene(!enable);
}

void activate_tiamat_position_hack(bool activate)
{
    static const auto code_addr = get_address("tiamat_attack_position");

    static const std::string_view code{"\xF3\x0F\x5C\xBE\x78\x01\x00\x00"sv   // subss  xmm7,DWORD PTR [rsi+0x178]
                                       "\xF3\x0F\x5C\xB6\x7C\x01\x00\x00"sv}; // subss  xmm6,DWORD PTR [rsi+0x17C]

    if (activate)
        write_mem_recoverable("activate_tiamat_position_hack", code_addr, code, true);
    else
        recover_mem("activate_tiamat_position_hack");
}

void activate_crush_elevator_hack(bool activate)
{
    auto memory = Memory::get();
    static size_t offsets[3];
    if (offsets[0] == 0)
    {
        auto func_offset = get_virtual_function_address(VTABLE_OFFSET::ACTIVEFLOOR_CRUSHING_ELEVATOR, 78);

        offsets[0] = find_inst(memory.exe(), "\xF3\x0F\x58\xD0"sv, func_offset, func_offset + 0x80, "activate_crush_elevator_hack");
        if (offsets[0] == 0)
            return;

        offsets[0] += 4; // pattern size
        offsets[1] = find_inst(memory.exe(), "\xEB*\x0F\x57\xD2"sv, offsets[0], offsets[0] + 0xF0, "activate_crush_elevator_hack");
        if (offsets[1] == 0)
            return;

        offsets[1] += 5; // pattern size
        offsets[2] = find_inst(memory.exe(), "\xF3\x0F\x58\xC1"sv, offsets[1], offsets[1] + 0x40, "activate_crush_elevator_hack");
        if (offsets[2] == 0)
            return;

        offsets[2] += 4; // pattern size
    }

    if (activate)
    {
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[0]), "\x0f\x2e\x90\x30\x01\x00\x00"sv, true); // ucomiss xmm2,DWORD PTR [rax+0x130] // limit
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[1]), "\xf3\x0f\x10\x9b\x30\x01\x00"sv, true); // movss  xmm3,DWORD PTR [rbx+0x130]  // limit
        write_mem_recoverable("activate_crush_elevator_hack", memory.at_exe(offsets[2]), "\xf3\x0f\x58\x83\x34\x01\x00"sv, true); // addss  xmm0,DWORD PTR [rbx+0x134]  // speed
    }
    else
        recover_mem("activate_crush_elevator_hack");
}

void activate_hundun_hack(bool activate)
{
    /*
     * Pointer to Hundun entity is stored in r13 register. which means we need 8 bytes for ucomiss instruction
     * but we have 7 available, that's why we jump out to new code with the instruction and back
     */
    static size_t offsets[6]; // y_limit, y_limit, bird_head, sneak_head, speed, speed
    static char new_code[3][8];

    if (offsets[0] == 0)
    {
        auto memory = Memory::get();
        auto func_offset = get_virtual_function_address(VTABLE_OFFSET::MONS_HUNDUN, 78);
        offsets[0] = find_inst(memory.exe(), "\x41\xF6\x85\x61\x01\x00\x00\x08"sv, func_offset, func_offset + 0x1420, "activate_hundun_hack");
        if (offsets[0] == 0)
            return;

        offsets[0] -= 13; // offset, no good pattern above
        offsets[1] = find_inst(memory.exe(), "\x41\x80\x8D\x61\x01\x00\x00\x04"sv, offsets[0], offsets[0] + 0xF40, "activate_hundun_hack");
        if (offsets[1] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[1] += 8; // pattern size

        offsets[2] = find_inst(memory.exe(), "\xF3\x41\x0F\x58\x45\x7C"sv, offsets[0], offsets[1], "activate_hundun_hack");
        if (offsets[2] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[2] += 6; // pattern size

        offsets[3] = find_inst(memory.exe(), "\xF3\x41\x0F\x58\x45\x7C"sv, offsets[2], offsets[1], "activate_hundun_hack");
        if (offsets[3] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[3] += 6; // pattern size

        offsets[4] = find_inst(memory.exe(), "\x83\x7A\x0C\x0E"sv, offsets[1], offsets[1] + 0xC0, "activate_hundun_hack");
        if (offsets[4] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[4] += 6; // pattern size plus jump

        offsets[5] = find_inst(memory.exe(), "\xF3\x41\x0F"sv, offsets[4], offsets[4] + 0x58, "activate_hundun_hack");
        if (offsets[5] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[5] += 9; // instruction size (din't include the whole thing in pattern, very short distance from previous pattern)

        offsets[0] = memory.at_exe(offsets[0]);
        offsets[1] = memory.at_exe(offsets[1]);
        offsets[2] = memory.at_exe(offsets[2]);
        offsets[3] = memory.at_exe(offsets[3]);
        offsets[4] = memory.at_exe(offsets[4]);
        offsets[5] = memory.at_exe(offsets[5]);

        char old_code[3][8];

        std::memcpy(old_code[0], (void*)offsets[0], 7);
        std::memcpy(old_code[1], (void*)offsets[1], 7);
        std::memcpy(old_code[2], (void*)offsets[5], 8);

        const std::string_view patch_code{"\x41\x0F\x2E\xBD\x64\x01\x00\x00"sv};      // ucomiss xmm7,DWORD PTR [r13+0x164]
        const std::string_view speed_patch{"\xF3\x41\x0F\x58\x85\x6C\x01\x00\x00"sv}; // addss  xmm0,DWORD PTR [r13+0x16C]

        patch_and_redirect(offsets[0], 7, patch_code, true);
        patch_and_redirect(offsets[1], 7, patch_code, true);
        patch_and_redirect(offsets[5], 8, speed_patch, true);

        std::memcpy(new_code[0], (void*)offsets[0], 7);
        std::memcpy(new_code[1], (void*)offsets[1], 7);
        std::memcpy(new_code[2], (void*)offsets[5], 8);

        // writing back the old code so we can just use write_mem_recoverable for going from vanilla to the patch
        write_mem_prot(offsets[0], std::string_view{&old_code[0][0], &old_code[0][7]}, true);
        write_mem_prot(offsets[1], std::string_view{&old_code[1][0], &old_code[1][7]}, true);
        write_mem_prot(offsets[5], std::string_view{&old_code[2][0], &old_code[2][8]}, true);
    }

    if (activate)
    {
        static const std::string_view speed_code{"\x49\x8D\x95\x68\x01\x00\x00"sv                           // lea    rdx,[r13+0x168]
                                                 "\x66\x2E\x0F\x1F\x84\x00\x00\x00\x00\x00\x90\x90\x90"sv}; //  spoiled with space, all nop

        write_mem_recoverable("activate_hundun_hack", offsets[0], std::string_view{&new_code[0][0], &new_code[0][7]}, true); // limit
        write_mem_recoverable("activate_hundun_hack", offsets[1], std::string_view{&new_code[1][0], &new_code[1][7]}, true); // limit
        write_mem_recoverable("activate_hundun_hack", offsets[5], std::string_view{&new_code[2][0], &new_code[2][8]}, true); // speed for adding to the y_limit

        write_mem_recoverable("activate_hundun_hack", offsets[4], speed_code, true); // speed (for adding to the x position)

        write_mem_recoverable("activate_hundun_hack", offsets[2], "\x0F\x2E\xB8\x70\x01\x00\x00"sv, true); // ucomiss xmm7,DWORD PTR [rax+0x170] // bird_head
        write_mem_recoverable("activate_hundun_hack", offsets[3], "\x0F\x2E\xB8\x74\x01\x00\x00"sv, true); // ucomiss xmm7,DWORD PTR [rax+0x174] // snake head
    }
    else
        recover_mem("activate_hundun_hack");
}

void set_boss_door_control_enabled(bool enable)
{
    static size_t offsets[2];
    if (offsets[0] == 0)
    {
        auto memory = Memory::get();
        offsets[0] = get_address("hundun_door_control");
        if (offsets[0] == 0)
            return;
        // find tiamat door control (the same pattern)
        offsets[1] = find_inst(memory.exe(), "\x4A\x8B\xB4\xC8\x80\xF4\x00\x00"sv, offsets[0] - memory.exe_ptr + 0x777, std::nullopt, "set_boss_door_control_enabled");
        if (offsets[1] == 0)
        {
            offsets[0] = 0;
            return;
        }
        offsets[1] = function_start(memory.at_exe(offsets[1]));
    }
    if (!enable)
    {
        write_mem_recoverable("set_boss_door_control_enabled", offsets[0], "\xC3\x90"sv, true);
        write_mem_recoverable("set_boss_door_control_enabled", offsets[1], "\xC3\x90"sv, true);
    }
    else
        recover_mem("set_boss_door_control_enabled");
}

void update_state()
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("state_refresh");
    }
    if (offset != 0)
    {
        auto state = State::get().ptr();
        typedef void refresh_func(StateMemory*);
        static refresh_func* rf = (refresh_func*)(offset);
        rf(state);
    }
}

void set_frametime(std::optional<double> frametime)
{
    static size_t offset = 0;
    if (offset == 0)
        offset = get_address("engine_frametime");
    if (offset != 0)
    {
        if (frametime.has_value())
            write_mem_recoverable("engine_frametime", offset, frametime.value(), true);
        else
            recover_mem("engine_frametime");
    }
}

std::optional<double> get_frametime()
{
    static size_t offset = 0;
    if (offset == 0)
        offset = get_address("engine_frametime");
    if (offset != 0)
        return memory_read<double>(offset);
    return std::nullopt;
}

void set_frametime_inactive(std::optional<double> frametime)
{
    static size_t offset = 0;
    if (offset == 0)
        offset = get_address("engine_frametime") + 0x10;
    if (offset != 0)
    {
        if (frametime.has_value())
            write_mem_recoverable("engine_frametime_inactive", offset, frametime.value(), true);
        else
            recover_mem("engine_frametime_inactive");
    }
}

std::optional<double> get_frametime_inactive()
{
    static size_t offset = 0;
    if (offset == 0)
        offset = get_address("engine_frametime") + 0x10;
    if (offset != 0)
        return memory_read<double>(offset);
    return std::nullopt;
}

void destroy_layer(uint8_t layer)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("unload_layer");
    }
    if (offset != 0)
    {
        auto* layer_ptr = State::get().layer(layer);
        typedef void destroy_func(Layer*);
        static destroy_func* df = (destroy_func*)(offset);
        df(layer_ptr);
    }
}

void destroy_level()
{
    destroy_layer(0);
    destroy_layer(1);
}

void create_layer(uint8_t layer)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("init_layer");
    }
    if (offset != 0)
    {
        auto* layer_ptr = State::get().layer(layer);
        typedef void init_func(Layer*);
        static init_func* ilf = (init_func*)(offset);
        ilf(layer_ptr);
    }
}

void create_level()
{
    create_layer(0);
    create_layer(1);
}

void set_death_enabled(bool enable)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("dead_players");
    }
    if (offset != 0)
    {
        if (!enable)
            write_mem_recoverable("death_disable", offset, "\xC3\x90"sv, true);
        else
            recover_mem("death_disable");
    }
}
