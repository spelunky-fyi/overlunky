#include "spawn_api.hpp"

#include <Windows.h>   // for GetCurrentThread, LONG, NO_ERROR
#include <array>       // for array
#include <cmath>       // for roundf, INFINITY
#include <cstddef>     // for size_t
#include <detours.h>   // for DetourAttach, DetourTransactionBegin
#include <functional>  // for function, _Func_impl_no_alloc<>::_M...
#include <new>         // for operator new
#include <optional>    // for optional
#include <type_traits> // for move
#include <utility>     // for pair, identity, min, _Find_fn, find
#include <vector>      // for vector, allocator, _Vector_iterator

#include "containers/custom_vector.hpp" //
#include "entities_chars.hpp"           // for Player
#include "entities_items.hpp"           // for ClimbableRope
#include "entities_liquids.hpp"         // for Lava
#include "entities_monsters.hpp"        // for Shopkeeper, RoomOwner
#include "entity.hpp"                   // for to_id, Entity, get_entity_ptr, Enti...
#include "entity_db.hpp"                // for EntityFactory
#include "illumination.hpp"             //
#include "items.hpp"                    //
#include "layer.hpp"                    // for Layer, g_level_max_y, g_level_max_x
#include "level_api.hpp"                // for LevelGenSystem, ThemeInfo
#include "logger.h"                     // for DEBUG
#include "math.hpp"                     // for AABB
#include "memory.hpp"                   // for write_mem_prot, memory_read
#include "prng.hpp"                     // for PRNG, PRNG::PRNG_CLASS, PRNG::ENTIT...
#include "script/events.hpp"            // for post_entity_spawn, pre_entity_spawn
#include "search.hpp"                   // for get_address
#include "state.hpp"                    // for StateMemory
#include "state_structs.hpp"            // for LiquidTileSpawnData, LiquidPhysics
#include "util.hpp"                     // for OnScopeExit

struct Items;

thread_local std::uint32_t g_SpawnNonReplacable;
thread_local SpawnType g_SpawnTypeFlags{SPAWN_TYPE_SYSTEMIC};
thread_local std::array<std::uint32_t, SPAWN_TYPE_NUM_FLAGS> g_SpawnTypes{};
thread_local std::function<void(Entity*)> g_temp_entity_spawn_hook;

void spawn_liquid(ENT_TYPE entity_type, float x, float y)
{
    if (y < 0.1) // spawning below 0 crashes the game
        return;

    using spawn_liquid_fun_t = void(void*, float, float, std::uint32_t, bool);
    static auto spawn_liquid_call = (spawn_liquid_fun_t*)get_address("spawn_liquid");

    // In case of lava, construct an Illumination* and assign it to each new lava entity
    // Do this only outside of level-gen, otherwise the game will create these objects
    static const auto lava_types = std::vector{
        to_id("ENT_TYPE_LIQUID_LAVA"),
        to_id("ENT_TYPE_LIQUID_STAGNANT_LAVA"),
        to_id("ENT_TYPE_LIQUID_COARSE_LAVA"),
    };

    auto state = get_state_ptr();
    if (state->loading != 2 && std::ranges::find(lava_types, entity_type) != lava_types.end())
    {
        std::vector<Lava*> lavas{};

        g_temp_entity_spawn_hook = [&lavas, entity_type](Entity* ent)
        {
            if (ent->type->id == entity_type)
            {
                lavas.push_back(ent->as<Lava>());
            }
        };
        spawn_liquid_call(state->liquid_physics, x, y, entity_type, false);
        g_temp_entity_spawn_hook = nullptr;

        for (Lava* lava : lavas)
        {
            Vec2 position{lava->x, lava->y};
            Color color{1.782f, 0.575262f, 0.0f, 0.0f};
            // green value is randomized in game
            // uses PRNG_CLASS::LEVEL_DECO
            // green = (PRNG::random_float / 1000 * 0.3 + 0.2) / 1.62

            float light_size = 0.6f; // should be 0.8f for dark level
            uint8_t flags = 0x63;

            // using construct_illumination_ptr_fun_t = Illumination*(custom_vector<Illumination*>*, float*, float*, uint8_t, float, uint32_t, uint32_t, uint8_t);
            // static auto construct_illumination_ptr_call = (construct_illumination_ptr_fun_t*)get_address("generate_illumination");

            // auto ill_ptr = construct_illumination_ptr_call(state->lightsources, position, color, 2, light_size, flags, lava->uid, lava->layer);
            auto ill_ptr = create_illumination(position, color, LIGHT_TYPE::FOLLOW_ENTITY, light_size, flags, lava->uid, (LAYER)lava->layer);
            lava->emitted_light = ill_ptr;
        }
    }
    else
    {
        spawn_liquid_call(state->liquid_physics, x, y, entity_type, false);
    }
}

void spawn_liquid(ENT_TYPE entity_type, float x, float y, float velocityx, float velocityy, uint32_t liquid_flags, uint32_t amount, float blobs_separation = INFINITY)
{
    LiquidTileSpawnData* liquid_spawn_info = nullptr;
    auto state_ptr = get_state_ptr();
    static const ENT_TYPE liquid_water = to_id("ENT_TYPE_LIQUID_WATER");
    static const ENT_TYPE liquid_coarse_water = to_id("ENT_TYPE_LIQUID_COARSE_WATER");
    static const ENT_TYPE liquid_lava = to_id("ENT_TYPE_LIQUID_LAVA");
    static const ENT_TYPE liquid_stagnant_lava = to_id("ENT_TYPE_LIQUID_STAGNANT_LAVA");
    static const ENT_TYPE liquid_coarse_lava = to_id("ENT_TYPE_LIQUID_COARSE_LAVA");

    if (entity_type == liquid_water)
    {
        liquid_spawn_info = &state_ptr->liquid_physics->water_tile_spawn_data;
    }
    else if (entity_type == liquid_coarse_water)
    {
        liquid_spawn_info = &state_ptr->liquid_physics->coarse_water_tile_spawn_data;
    }
    else if (entity_type == liquid_lava)
    {
        liquid_spawn_info = &state_ptr->liquid_physics->lava_tile_spawn_data;
    }
    else if (entity_type == liquid_stagnant_lava)
    {
        liquid_spawn_info = &state_ptr->liquid_physics->stagnant_lava_tile_spawn_data;
    }
    else if (entity_type == liquid_coarse_lava)
    {
        liquid_spawn_info = &state_ptr->liquid_physics->coarse_lava_tile_spawn_data;
    }

    if (liquid_spawn_info != nullptr)
    {
        static auto some_value = get_address("spawn_liquid_amount");
        const uint8_t tmp_value = memory_read<uint8_t>(some_value);
        write_mem_prot(some_value, (uint8_t)0, true);

        // Save default liquid physics spawn values
        float tmp_spawn_velocity_x = liquid_spawn_info->spawn_velocity_x,
              tmp_spawn_velocity_y = liquid_spawn_info->spawn_velocity_y,
              tmp_blobs_separation = liquid_spawn_info->blobs_separation;
        uint32_t tmp_liquid_flags = liquid_spawn_info->liquid_flags,
                 tmp_liquidtile_liquid_amount = liquid_spawn_info->liquidtile_liquid_amount;

        liquid_spawn_info->liquid_flags = liquid_flags;
        liquid_spawn_info->spawn_velocity_x = velocityx;
        liquid_spawn_info->spawn_velocity_y = velocityy;
        liquid_spawn_info->liquidtile_liquid_amount = amount;

        if (!std::isinf(blobs_separation))
            liquid_spawn_info->blobs_separation = blobs_separation;

        spawn_liquid(entity_type, x, y);

        // Reset to default values
        liquid_spawn_info->liquid_flags = tmp_liquid_flags;
        liquid_spawn_info->spawn_velocity_x = tmp_spawn_velocity_x;
        liquid_spawn_info->spawn_velocity_y = tmp_spawn_velocity_y;
        liquid_spawn_info->liquidtile_liquid_amount = tmp_liquidtile_liquid_amount;
        liquid_spawn_info->blobs_separation = tmp_blobs_separation;

        write_mem_prot(some_value, tmp_value, true);
    }
}

void spawn_liquid_ex(ENT_TYPE entity_type, float x, float y, float velocityx, float velocityy, uint32_t liquid_flags, uint32_t amount)
{
    return spawn_liquid(entity_type, x, y, velocityx, velocityy, liquid_flags, amount, INFINITY);
}

int32_t spawn_entity_abs(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Vec2 offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return HeapBase::get().state()->layers[actual_layer]->spawn_entity(entity_type, x + offset_position.x, y + offset_position.y, false, vx, vy, false)->uid;
}

int32_t spawn_entity_snap_to_floor(ENT_TYPE entity_type, float x, float y, LAYER layer)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Vec2 offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return HeapBase::get().state()->layers[actual_layer]->spawn_entity_snap_to_floor(entity_type, x + offset_position.x, y + offset_position.y)->uid;
}

int32_t spawn_entity_snap_to_grid(ENT_TYPE entity_type, float x, float y, LAYER layer)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Vec2 offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return HeapBase::get().state()->layers[actual_layer]->spawn_entity(entity_type, x + offset_position.x, y + offset_position.y, false, 0.0f, 0.0f, true)->uid;
}

int32_t spawn_entity_abs_nonreplaceable(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy)
{
    g_SpawnNonReplacable++;
    OnScopeExit pop{[]
                    { g_SpawnNonReplacable--; }};
    return spawn_entity_abs(entity_type, x, y, layer, vx, vy);
}

int32_t spawn_entity_over(ENT_TYPE entity_type, uint32_t over_uid, float x, float y)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Entity* overlay = get_entity_ptr(over_uid);
    if (overlay == nullptr)
        return -1;
    uint8_t layer = overlay->layer;
    if (layer > 1)
        return -1;

    return HeapBase::get().state()->layers[layer]->spawn_entity_over(entity_type, overlay, x, y)->uid;
}

int32_t spawn_door_abs(float x, float y, LAYER layer, uint8_t w, uint8_t l, uint8_t t)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Vec2 offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return HeapBase::get().state()->layers[actual_layer]->spawn_door(x + offset_position.x, y + offset_position.y, w, l, t)->uid;
}

void spawn_backdoor_abs(float x, float y)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = HeapBase::get().state();
    DEBUG("Spawning backdoor on {}, {}", x, y);
    Layer* front_layer = state->layers[0];
    Layer* back_layer = state->layers[1];
    front_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    front_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0f, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0f, false, 0.0, 0.0, true);
}

int32_t spawn_apep(float x, float y, LAYER layer, bool right)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Vec2 offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return HeapBase::get().state()->layers[actual_layer]->spawn_apep(x + offset_position.x, y + offset_position.y, right)->uid;
}

int32_t spawn_tree(float x, float y, LAYER layer, uint16_t height)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Vec2 offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    x = std::roundf(x + offset_position.x);
    y = std::roundf(y + offset_position.y);

    Layer* layer_ptr = HeapBase::get().state()->layers[actual_layer];

    // Needs some space on top
    if (x < 0 || static_cast<int>(x) >= g_level_max_x || y < 1 || static_cast<int>(y) + 2 >= g_level_max_y || height == 1 ||
        layer_ptr->get_grid_entity_at(x, y) != nullptr ||
        layer_ptr->get_grid_entity_at(x, y + 1.0f) != nullptr ||
        layer_ptr->get_grid_entity_at(x, y + 2.0f) != nullptr)
        return -1;

    static const auto tree_base = to_id("ENT_TYPE_FLOOR_TREE_BASE");
    static const auto tree_trunk = to_id("ENT_TYPE_FLOOR_TREE_TRUNK");
    static const auto tree_top = to_id("ENT_TYPE_FLOOR_TREE_TOP");
    static const auto tree_branch = to_id("ENT_TYPE_FLOOR_TREE_BRANCH");
    static const auto tree_deco = to_id("ENT_TYPE_DECORATION_TREE");

    PRNG* prng = HeapBase::get().prng();

    // spawn the base
    Entity* current_piece = layer_ptr->spawn_entity(tree_base, x, y, false, 0.0f, 0.0f, true);
    Entity* base_piece = current_piece;

    // spawn segments
    if (layer_ptr->get_grid_entity_at(x, y + 3.0f) == nullptr)
    {
        size_t i = height == 0 ? 5 : (height - 2); // -2 to remove the base and top
        for (; i > 0; --i)
        {
            y += 1.0f;
            if (static_cast<int>(y) + 2 >= g_level_max_y || layer_ptr->get_grid_entity_at(x, y + 2.0f) != nullptr)
            {
                break;
            }
            current_piece = layer_ptr->spawn_entity_over(tree_trunk, current_piece, 0.0f, 1.0f);
            if (height == 0 && prng->random_chance(2, PRNG::PRNG_CLASS::ENTITY_VARIATION))
            {
                break;
            }
        }
    }
    // spawn the top
    current_piece = layer_ptr->spawn_entity_over(tree_top, current_piece, 0.0f, 1.0f);

    do // spawn branches
    {
        auto spawn_deco = [&](Entity* branch, bool left)
        {
            Entity* deco = layer_ptr->spawn_entity_over(tree_deco, branch, 0.0f, 0.49f);
            deco->animation_frame = 7 * 12 + 3 + static_cast<uint16_t>(prng->random_int(0, 2, PRNG::PRNG_CLASS::ENTITY_VARIATION)) * 12;
            if (left)
                deco->flags |= 1U << 16; // flag 17: facing left
        };
        auto test_pos = current_piece->abs_position();

        if (static_cast<int>(test_pos.x) + 1 < g_level_max_x && layer_ptr->get_grid_entity_at(test_pos.x + 1, test_pos.y) == nullptr &&
            prng->random_chance(2, PRNG::PRNG_CLASS::ENTITY_VARIATION))
        {
            Entity* branch = layer_ptr->spawn_entity_over(tree_branch, current_piece, 1.02f, 0.0f);
            spawn_deco(branch, false);
        }
        if (static_cast<int>(test_pos.x) - 1 > 0 && layer_ptr->get_grid_entity_at(test_pos.x - 1, test_pos.y) == nullptr &&
            prng->random_chance(2, PRNG::PRNG_CLASS::ENTITY_VARIATION))
        {
            Entity* branch = layer_ptr->spawn_entity_over(tree_branch, current_piece, -1.02f, 0.0f);
            branch->flags |= 1U << 16; // flag 17: facing left
            spawn_deco(branch, true);
        }
        current_piece = current_piece->overlay;
    } while (current_piece->overlay);

    return base_piece->uid;
}

int32_t spawn_mushroom(float x, float y, LAYER l, uint16_t height) // height relates to trunk
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    Vec2 offset(0.0f, 0.0f);
    const auto actual_layer = enum_to_layer(l, offset);
    const auto layer_ptr = HeapBase::get().state()->layers[actual_layer];
    const uint32_t i_x = static_cast<uint32_t>(x + offset.x + 0.5f);
    uint32_t i_y = static_cast<uint32_t>(y + offset.y + 0.5f);
    static const auto base = to_id("ENT_TYPE_FLOOR_MUSHROOM_BASE");
    static const auto trunk = to_id("ENT_TYPE_FLOOR_MUSHROOM_TRUNK");
    static const auto top = to_id("ENT_TYPE_FLOOR_MUSHROOM_TOP");
    static const auto platform = to_id("ENT_TYPE_FLOOR_MUSHROOM_HAT_PLATFORM");
    static const auto deco = to_id("ENT_TYPE_DECORATION_MUSHROOM_HAT");

    if (height == 1 || i_y == 0 || i_x >= g_level_max_x || i_y >= g_level_max_y - 2 || // check parameters
        layer_ptr->grid_entities[i_y][i_x] != nullptr ||                               // check spaces above etc.
        layer_ptr->grid_entities[i_y + 1][i_x] != nullptr ||
        layer_ptr->grid_entities[i_y + 2][i_x] != nullptr)
        return -1;

    Entity* current_ent = layer_ptr->spawn_entity(base, x, y, false, 0, 0, true);
    const auto base_uid = current_ent->uid;
    if (layer_ptr->grid_entities[i_y + 3][i_x] == nullptr)
    {
        if (height != 0)
        {
            height -= 2; // remove the base and top of the trunk
        }
        else
        {
            auto prng = HeapBase::get().prng();
            height = static_cast<uint16_t>(prng->random_int(1, 3, PRNG::PRNG_CLASS::PROCEDURAL_SPAWNS));
        }

        i_y += 3;
        for (uint32_t test_y = 0; test_y < height; ++test_y)
        {
            if (i_y + test_y >= g_level_max_y - 2 || layer_ptr->grid_entities[i_y + test_y][i_x] != nullptr)
                break;

            current_ent = layer_ptr->spawn_entity_over(trunk, current_ent, 0, 1);
        }
    }
    current_ent = layer_ptr->spawn_entity_over(top, current_ent, 0, 1);
    layer_ptr->spawn_entity_over(deco, current_ent, 0, 1);

    Entity* platform_left = layer_ptr->spawn_entity_over(platform, current_ent, -1, 0);
    layer_ptr->spawn_entity_over(deco, platform_left, 0, 1)->animation_frame -= 1;

    Entity* platform_right = layer_ptr->spawn_entity_over(platform, current_ent, 1, 0);
    layer_ptr->spawn_entity_over(deco, platform_right, 0, 1)->animation_frame += 1;
    platform_right->animation_frame = 119;
    return base_uid;
}

int32_t spawn_unrolled_player_rope(float x, float y, LAYER layer, TEXTURE texture)
{
    static const auto rope_offset = get_address("attach_thrown_rope_to_background");
    return spawn_unrolled_player_rope(x, y, layer, texture, static_cast<uint16_t>(memory_read<uint32_t>(rope_offset)));
}
int32_t spawn_unrolled_player_rope(float x, float y, LAYER layer, TEXTURE texture, uint16_t max_length)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    using setup_top_rope_rendering_info_one_fun = void(RenderInfo*);
    using setup_top_rope_rendering_info_two_fun = void(RenderInfo*, int32_t, int32_t);
    static const auto setup_top_rope_rendering_info_one = (setup_top_rope_rendering_info_one_fun*)get_address("setup_top_rope_rendering_info_one"sv);
    static const auto setup_top_rope_rendering_info_two = (setup_top_rope_rendering_info_two_fun*)get_address("setup_top_rope_rendering_info_two"sv);
    static const auto rope_ent = to_id("ENT_TYPE_ITEM_CLIMBABLE_ROPE");

    Vec2 offset(0.0f, 0.0f);
    const auto actual_layer = enum_to_layer(layer, offset);
    const auto layer_ptr = HeapBase::get().state()->layers[actual_layer];
    const uint32_t i_x = static_cast<uint32_t>(x + offset.x + 0.5f);
    const uint32_t i_y = static_cast<uint32_t>(y + offset.y + 0.5f);
    const float g_x = static_cast<float>(i_x);
    const float g_y = static_cast<float>(i_y);

    auto has_solid_ent = [=](float gx, float gy) -> bool
    {
        {
            Movable* ent = static_cast<Movable*>(layer_ptr->get_entity_at(gx, gy, 0x180, 0x4, 0x8, 0));
            if (ent)
            {
                return ent->type->search_flags == 0x100 || (ent->velocityx == 0.0 && ent->velocityy == 0.0); // see 0x2299c90f
            }
        }

        {
            Entity* grid_ent = layer_ptr->get_grid_entity_at(gx, gy);
            if (grid_ent)
            {
                return (grid_ent->flags & 0x1c) == 0x4; // see 0x2299cd7c
            }
        }

        return false;
    };

    if (has_solid_ent(g_x, g_y))
    {
        return -1;
    }

    constexpr uint16_t anim_frame_top = 157;
    constexpr uint16_t anim_frame_single = 158;
    constexpr uint16_t anim_frame_middle = 192;
    constexpr uint16_t anim_frame_bottom = 197;

    ClimbableRope* top_part = static_cast<ClimbableRope*>(layer_ptr->spawn_entity(rope_ent, g_x, g_y, false, 0, 0, true));
    top_part->set_texture(texture);
    top_part->animation_frame = anim_frame_single;
    top_part->idle_counter = 5;
    top_part->segment_nr = 0;
    top_part->segment_nr_inverse = max_length;
    setup_top_rope_rendering_info_one(top_part->rendering_info);
    setup_top_rope_rendering_info_two(top_part->rendering_info, 7, 2);

    ClimbableRope* above_part = top_part;
    for (size_t i = 1; i <= max_length; i++)
    {
        if (has_solid_ent(g_x, g_y - static_cast<float>(i)))
        {
            break;
        }

        ClimbableRope* next_part = static_cast<ClimbableRope*>(layer_ptr->spawn_entity_over(rope_ent, above_part, 0, -1));
        next_part->set_texture(texture);
        next_part->animation_frame = anim_frame_bottom;
        next_part->idle_counter = 5;
        next_part->segment_nr = static_cast<uint32_t>(i);
        next_part->segment_nr_inverse = max_length - static_cast<uint32_t>(i);
        next_part->above_part = above_part;
        setup_top_rope_rendering_info_one(next_part->rendering_info);

        above_part->below_part = next_part;
        if (i == 1)
        {
            above_part->animation_frame = anim_frame_top;
        }
        else
        {
            above_part->animation_frame = anim_frame_middle;
        }
        above_part = next_part;
    }

    return top_part->uid;
}

Entity* spawn_impostor_lake(AABB aabb, LAYER layer, ENT_TYPE impostor_type, float top_threshold)
{
    static const auto impostor_lake_id = to_id("ENT_TYPE_LIQUID_IMPOSTOR_LAKE");
    static const auto impostor_lava_id = to_id("ENT_TYPE_LIQUID_IMPOSTOR_LAVA");
    if (impostor_type == impostor_lake_id || impostor_type == impostor_lava_id)
    {
        push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
        OnScopeExit pop{[]
                        { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

        Vec2 offset_position;
        uint8_t actual_layer = enum_to_layer(layer, offset_position);

        aabb.offset(offset_position.x, offset_position.y);

        auto [x, y] = aabb.center();

        Entity* impostor_lake = get_entity_ptr(spawn_entity_abs(impostor_type, x, y, (LAYER)actual_layer, 0.0f, 0.0f));
        setup_impostor_lake(impostor_lake, aabb, top_threshold);
        return impostor_lake;
    }

    return nullptr;
}
void setup_impostor_lake(Entity* lake_impostor, AABB aabb, float top_threshold)
{
    using setup_lake_impostor_fun_t = void(Entity*, float half_width, float half_heigth, float top_threshold);
    static auto setup_lake_impostor = (setup_lake_impostor_fun_t*)get_address("setup_lake_impostor");
    setup_lake_impostor(lake_impostor, aabb.width() / 2.0f, aabb.height() / 2.0f, top_threshold);
}

void fix_impostor_lake_positions()
{
    auto state = get_state_ptr();
    for (auto& lake : state->liquid_physics->impostor_lakes)
    {
        Entity* impostor_lake = lake.impostor_lake;
        auto [x_pos, y_pos] = impostor_lake->abs_position();
        x_pos += impostor_lake->offsetx;
        y_pos += impostor_lake->offsety;
        float hitboxx = impostor_lake->hitboxx;
        float hitboxy = impostor_lake->hitboxy;
        // Match the calculations that the game does when initializing these impostor objects.
        int32_t x1 = (int32_t)((y_pos - hitboxy + .5) * 3) * 0x102;
        int32_t x2 = (int32_t)((x_pos - hitboxx + .5) * 3) + x1;
        int32_t x3 = (int32_t)((x_pos + hitboxx + .5) * 3);
        x1 += x3;
        x3 = (int32_t)((y_pos + hitboxy + .5) * 3) * 0x102 + x3;
        lake.position1 = x2;
        lake.position2 = x1;
        lake.position3 = x3;
    }
}

void update_spawn_type_flags()
{
    SPAWN_TYPE flags = 0;

    flags |= g_SpawnTypes[0] ? SPAWN_TYPE_LEVEL_GEN_TILE_CODE : 0;
    flags |= g_SpawnTypes[1] ? SPAWN_TYPE_LEVEL_GEN_PROCEDURAL : 0;
    flags |= g_SpawnTypes[2] ? SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING : 0;

    // LEVEL_GEN_GENERAL only covers level gen spawns not covered by the others
    if ((flags & SPAWN_TYPE_LEVEL_GEN) == 0)
    {
        flags |= g_SpawnTypes[3] ? SPAWN_TYPE_LEVEL_GEN_GENERAL : 0;
    }

    flags |= g_SpawnTypes[4] ? SPAWN_TYPE_SCRIPT : 0;

    // SYSTEMIC covers everything that isn't covered above
    if (flags == 0)
    {
        flags |= SPAWN_TYPE_SYSTEMIC;
    }

    g_SpawnTypeFlags = SpawnType(flags);
}
void push_spawn_type_flags(SPAWN_TYPE flags)
{
    for (size_t i = 0; i < g_SpawnTypes.size(); i++)
    {
        if (flags & (1 << i))
        {
            g_SpawnTypes[i]++;
        }
    }
    update_spawn_type_flags();
}
void pop_spawn_type_flags(SPAWN_TYPE flags)
{
    for (size_t i = 0; i < g_SpawnTypes.size(); i++)
    {
        if (flags & (1 << i))
        {
            g_SpawnTypes[i]--;
        }
    }
    update_spawn_type_flags();
}

using SpawnEntityFun = Entity*(EntityFactory*, std::uint32_t, float, float, bool, Entity*, bool);
SpawnEntityFun* g_spawn_entity_trampoline{nullptr};
Entity* spawn_entity(EntityFactory* entity_factory, std::uint32_t entity_type, float x, float y, bool layer, Entity* overlay, bool some_bool)
{
    // TODO: This still might not work very well and corner fill isn't actually floor spreading per level config definition, and should have a different SPAWN_TYPE (corner fill still happens when floor spreading chance is set to 0)
    // const auto theme_floor = HeapBase::get().state()->current_theme->get_floor_spreading_type();
    // const auto theme_floor2 = HeapBase::get().state()->current_theme->get_floor_spreading_type2();
    auto state = HeapBase::get().state();
    auto [ax, ay, bx, by] = std::make_tuple(2.5f, 122.5f, state->w * 10.0f + 2.5f, 122.5f - state->h * 8.0f);
    static const auto border_octo = to_id("ENT_TYPE_FLOOR_BORDERTILE_OCTOPUS");
    static const auto border_dust = to_id("ENT_TYPE_FLOOR_DUSTWALL");
    const bool is_decorated = (entity_factory->types[entity_type].properties_flags & 0x1) == 0x1;
    const bool is_styled = (entity_factory->types[entity_type].properties_flags & 0x2) == 0x2;
    const bool is_border = entity_type <= border_octo || entity_type == border_dust || x < ax || x > bx || y > ay || y < by;
    const bool is_floor_spreading = (is_decorated || is_styled) && !is_border && (g_SpawnTypeFlags & SPAWN_TYPE_LEVEL_GEN) && !(g_SpawnTypeFlags & SPAWN_TYPE_LEVEL_GEN_TILE_CODE);
    if (is_floor_spreading)
    {
        push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING);
    }

    /* testing
    auto entity_db = entity_factory->types[entity_type];
    // solid floor that's not from a tilecode
    const bool is_floor_replaced = (entity_db.search_flags & 0x100 && entity_db.default_flags & 0x4) && (g_SpawnTypeFlags & SPAWN_TYPE_LEVEL_GEN) && !(g_SpawnTypeFlags & SPAWN_TYPE_LEVEL_GEN_TILE_CODE);
    if (is_floor_replaced)
    {
        push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_FLOOR_REPLACED);
    }*/

    Entity* spawned_ent{nullptr};
    if (g_SpawnNonReplacable == 0)
    {
        spawned_ent = pre_entity_spawn(entity_type, x, y, layer, overlay, g_SpawnTypeFlags);
    }

    if (spawned_ent == nullptr)
    {
        spawned_ent = g_spawn_entity_trampoline(entity_factory, entity_type, x, y, layer, overlay, some_bool);
    }

    post_entity_spawn(spawned_ent, g_SpawnTypeFlags);
    if (g_temp_entity_spawn_hook)
    {
        g_temp_entity_spawn_hook(spawned_ent);
    }

    if (is_floor_spreading)
    {
        pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING);
    }

    return spawned_ent;
}

void init_spawn_hooks()
{
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        g_spawn_entity_trampoline = (SpawnEntityFun*)get_address("spawn_entity");

        DetourAttach((void**)&g_spawn_entity_trampoline, (SpawnEntityFun*)spawn_entity);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking SpawnEntity: {}\n", error);
        }
    }
}

int32_t spawn_player(int8_t player_slot, std::optional<float> x, std::optional<float> y, std::optional<LAYER> layer)
{
    if (player_slot < 1 || player_slot > 4)
        return -1;
    auto state = HeapBase::get().state();
    auto& slot = state->items->player_select_slots[player_slot - 1];
    if (slot.character < to_id("ENT_TYPE_CHAR_ANA_SPELUNKY") || slot.character > to_id("ENT_TYPE_CHAR_CLASSIC_GUY"))
        return -1;
    if (state->items->player_count < player_slot)
        state->items->player_count = player_slot;
    slot.activated = true;

    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto old_x = state->level_gen->spawn_x;
    auto old_y = state->level_gen->spawn_y;
    state->level_gen->spawn_x = x.value_or(old_x);
    state->level_gen->spawn_y = y.value_or(old_y);
    using spawn_player_fun = void(Items*, uint8_t ps);
    static auto spawn_player = (spawn_player_fun*)get_address("spawn_player");
    // move the back layer to front layer offset if spawning in back layer
    if (layer.has_value() && layer.value() == LAYER::BACK)
        std::swap(state->layers[0], state->layers[1]);
    spawn_player(get_state_ptr()->items, player_slot - 1);
    if (layer.has_value() && layer.value() == LAYER::BACK)
        std::swap(state->layers[0], state->layers[1]);
    state->level_gen->spawn_x = old_x;
    state->level_gen->spawn_y = old_y;
    auto player = state->items->player(player_slot - 1);
    if (player)
        return player->uid;
    return -1;
}

int32_t spawn_companion(ENT_TYPE companion_type, float x, float y, LAYER layer)
{
    static auto offset = get_address("spawn_companion");
    if (offset != 0)
    {
        push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
        OnScopeExit pop{[]
                        { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

        auto state = get_state_ptr();
        typedef Player* spawn_companion_func(StateMemory*, float x, float y, size_t layer, uint32_t entity_type);
        static spawn_companion_func* sc = (spawn_companion_func*)(offset);

        Vec2 pos_offset;
        const auto actual_layer = enum_to_layer(layer, pos_offset);

        Player* spawned = sc(state, x + pos_offset.x, y + pos_offset.y, actual_layer, companion_type);
        return spawned->uid;
    }
    return -1;
}

int32_t spawn_shopkeeper(float x, float y, LAYER layer, ROOM_TEMPLATE room_template)
{
    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    auto level_gen = HeapBase::get().level_gen();
    auto [ix, iy] = level_gen->get_room_index(x, y);
    uint32_t room_index = ix + iy * 8;
    level_gen->set_room_template(ix, iy, real_layer, room_template);
    uint32_t keeper_uid = spawn_entity_abs_nonreplaceable(to_id("ENT_TYPE_MONS_SHOPKEEPER"), x, y, layer, 0, 0);
    auto keeper = get_entity_ptr(keeper_uid)->as<Shopkeeper>();
    keeper->shop_owner = true;
    keeper->room_index = room_index;
    // ShopOwnerDetails owner = {.layer = (uint8_t)layer, .room_index = room_index, .shop_owner_uid = keeper_uid};
    // state_ptr->shops.shop_owners.push_back(owner);
    return keeper_uid;
}

int32_t spawn_roomowner(ENT_TYPE owner_type, float x, float y, LAYER layer, int16_t room_template)
{
    static const auto waddler_id = to_id("ENT_TYPE_MONS_STORAGEGUY");
    static const auto shoppie_id = to_id("ENT_TYPE_MONS_SHOPKEEPER");
    static const auto yang_id = to_id("ENT_TYPE_MONS_YANG");
    static const auto tun_id = to_id("ENT_TYPE_MONS_MERCHANT");

    const uint8_t real_layer = static_cast<int32_t>(layer) < 0 ? 0 : static_cast<uint8_t>(layer);
    auto level_gen = HeapBase::get().level_gen();
    auto [ix, iy] = level_gen->get_room_index(x, y);
    uint32_t room_index = ix + iy * 8;
    if (room_template >= 0)
        level_gen->set_room_template(ix, iy, real_layer, (uint16_t)room_template);
    uint32_t keeper_uid = spawn_entity_abs_nonreplaceable(owner_type, x, y, layer, 0, 0);
    if (owner_type == waddler_id || owner_type == yang_id || owner_type == tun_id)
    {
        auto keeper = get_entity_ptr(keeper_uid)->as<RoomOwner>();
        keeper->room_index = room_index;
    }
    else if (owner_type == shoppie_id)
    {
        auto keeper = get_entity_ptr(keeper_uid)->as<Shopkeeper>();
        keeper->shop_owner = true;
        keeper->room_index = room_index;
    }
    // ShopOwnerDetails owner = {.layer = (uint8_t)layer, .room_index = room_index, .shop_owner_uid = keeper_uid};
    // state_ptr->shops.shop_owners.push_back(owner);
    return keeper_uid;
}

int32_t spawn_playerghost(ENT_TYPE char_type, float x, float y, LAYER layer)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    static const auto player_ghost = to_id("ENT_TYPE_ITEM_PLAYERGHOST");
    static const auto ana = to_id("ENT_TYPE_CHAR_ANA_SPELUNKY");
    static const auto egg_child = to_id("ENT_TYPE_CHAR_EGGPLANT_CHILD");
    static PlayerSlot dummy_player_controls;
    dummy_player_controls.player_slot = -1;

    if (char_type < ana || char_type > egg_child)
        return -1;

    Vec2 offset;
    const auto l = enum_to_layer(layer, offset);
    auto level_layer = HeapBase::get().state()->layers[l];
    auto player_ghost_entity = level_layer->spawn_entity(player_ghost, x + offset.x, y + offset.y, false, 0, 0, false)->as<PlayerGhost>();
    if (player_ghost_entity)
    {
        player_ghost_entity->player_inputs = &dummy_player_controls;
        player_ghost_entity->set_texture(get_type(char_type)->texture_id);
        return player_ghost_entity->uid;
    }
    return -1;
}

MagmamanSpawnPosition::MagmamanSpawnPosition(uint32_t x_, uint32_t y_)
{
    x = x_;
    y = y_;
    timer = static_cast<uint32_t>(HeapBase::get().prng()->random_int(2700, 27000, PRNG::PRNG_CLASS::PROCEDURAL_SPAWNS));
}
