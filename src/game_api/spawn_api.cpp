#include "spawn_api.hpp"

#include "entities_liquids.hpp"
#include "entity.hpp"
#include "layer.hpp"
#include "level_api.hpp"
#include "logger.h"
#include "memory.hpp"
#include "prng.hpp"
#include "rpc.hpp"
#include "util.hpp"

#include "script/events.hpp"

#include <Windows.h>
#include <detours.h>

#include <array>
#include <functional>
#include <ranges>

std::uint32_t g_SpawnNonReplacable;
SpawnType g_SpawnTypeFlags;
std::array<std::uint32_t, SPAWN_TYPE_NUM_FLAGS> g_SpawnTypes{};
std::function<void(Entity*)> g_temp_entity_spawn_hook;

void spawn_liquid(ENT_TYPE entity_type, float x, float y)
{
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
            float position[2] = {lava->x, lava->y};
            float color[4] = {1.782f, 0.575262f, 0.0f, 0.0f}; // green value is randomized!
            float light_size = 1.0f;
            uint32_t flags = 0x63;

            using construct_illumination_ptr_fun_t = Illumination*(std::vector<Illumination*>*, float*, float*, uint8_t, float, uint32_t, uint32_t, uint8_t);
            static auto construct_illumination_ptr_call = (construct_illumination_ptr_fun_t*)get_address("generate_illumination");

            auto ill_ptr = construct_illumination_ptr_call(state->lightsources, position, color, 2, light_size, flags, lava->uid, lava->layer);
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
        auto some_value = get_address("spawn_liquid_amount");
        const uint8_t tmp_value = read_u8(some_value);
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
        if (blobs_separation != INFINITY)
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

int32_t spawn_entity(ENT_TYPE entity_type, float x, float y, bool s, float vx, float vy, bool snap) // ui only
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();
    Player* player = nullptr;

    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
    {
        if (state.items()->player(i) != nullptr)
        {
            player = state.items()->player(i); // maybe spawn offset to camera focus uid then the player itself?
            break;
        }
    }
    if (player == nullptr)
        return -1;

    std::pair<float, float> offset_position;
    if (!s)
        offset_position = player->position();

    DEBUG("Spawning {} on {}, {}", entity_type, x + offset_position.first, y + offset_position.second);
    return state.layer_local(player->layer)->spawn_entity(entity_type, x + offset_position.first, y + offset_position.second, s, vx, vy, snap)->uid;
}

int32_t spawn_entity_abs(ENT_TYPE entity_type, float x, float y, LAYER layer, float vx, float vy)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    std::pair<float, float> offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return State::get().layer_local(actual_layer)->spawn_entity(entity_type, x + offset_position.first, y + offset_position.second, false, vx, vy, false)->uid;
}

int32_t spawn_entity_snap_to_floor(ENT_TYPE entity_type, float x, float y, LAYER layer)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    std::pair<float, float> offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return State::get().layer_local(actual_layer)->spawn_entity_snap_to_floor(entity_type, x + offset_position.first, y + offset_position.second)->uid;
}

int32_t spawn_entity_snap_to_grid(ENT_TYPE entity_type, float x, float y, LAYER layer)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    std::pair<float, float> offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return State::get().layer_local(actual_layer)->spawn_entity(entity_type, x + offset_position.first, y + offset_position.second, false, 0.0f, 0.0f, true)->uid;
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

    auto state = State::get();
    Entity* overlay = get_entity_ptr(over_uid);
    if (overlay == nullptr)
        return -1;
    uint8_t layer = overlay->layer;
    return state.layer_local(layer)->spawn_entity_over(entity_type, overlay, x, y)->uid;
}

int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t) // ui only
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();

    auto player = state.items()->player(0); // do the same stuff as in spawn_entity?
    if (player == nullptr)
        return -1;
    auto [_x, _y] = player->position();
    DEBUG("Spawning door on {}, {}", x + _x, y + _y);
    Layer* layer = state.layer_local(player->layer);
    layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    return layer->spawn_door(x + _x, y + _y, w, l, t)->uid;
}

int32_t spawn_door_abs(float x, float y, LAYER layer, uint8_t w, uint8_t l, uint8_t t)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    std::pair<float, float> offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return State::get().layer_local(actual_layer)->spawn_door(x + offset_position.first, y + offset_position.second, w, l, t)->uid;
}

void spawn_backdoor(float x, float y) // ui only
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto [_x, _y] = player->position();
    DEBUG("Spawning backdoor on {}, {}", x + _x, y + _y);
    Layer* front_layer = state.layer_local(0);
    Layer* back_layer = state.layer_local(1);
    front_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    front_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0f, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0f, false, 0.0, 0.0, true);
    front_layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    back_layer->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
}

void spawn_backdoor_abs(float x, float y)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();
    DEBUG("Spawning backdoor on {}, {}", x, y);
    Layer* front_layer = state.layer_local(0);
    Layer* back_layer = state.layer_local(1);
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

    std::pair<float, float> offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);

    return State::get().layer_local(actual_layer)->spawn_apep(x + offset_position.first, y + offset_position.second, right)->uid;
}

void spawn_tree(float x, float y, LAYER layer)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    std::pair<float, float> offset_position;
    uint8_t actual_layer = enum_to_layer(layer, offset_position);
    x += offset_position.first;
    y += offset_position.second;

    x = std::roundf(x);
    y = std::roundf(y);

    Layer* layer_ptr = State::get().layer_local(actual_layer);

    // Needs some space on top
    if (layer_ptr->get_grid_entity_at(x, y + 2.0f) == nullptr)
    {
        static const auto tree_base = to_id("ENT_TYPE_FLOOR_TREE_BASE");
        static const auto tree_trunk = to_id("ENT_TYPE_FLOOR_TREE_TRUNK");
        static const auto tree_top = to_id("ENT_TYPE_FLOOR_TREE_TOP");
        static const auto tree_branch = to_id("ENT_TYPE_FLOOR_TREE_BRANCH");
        static const auto tree_deco = to_id("ENT_TYPE_DECORATION_TREE");

        PRNG& prng = PRNG::get_local();

        // spawn the base
        Entity* base = layer_ptr->spawn_entity(tree_base, x, y, false, 0.0f, 0.0f, true);

        // spawn segments
        if (layer_ptr->get_grid_entity_at(x, y + 3.0f) == nullptr)
        {
            size_t i = 5;
            while (i > 0)
            {
                i--;
                y = y + 1;
                base = layer_ptr->spawn_entity_over(tree_trunk, base, 0.0f, 1.0f);
                if (layer_ptr->get_grid_entity_at(x, y + 2.0f) != nullptr)
                {
                    break;
                }
                if (prng.random_chance(2, PRNG::PRNG_CLASS::ENTITY_VARIATION))
                {
                    break;
                }
            }
        }

        // spawn the top
        base = layer_ptr->spawn_entity_over(tree_top, base, 0.0f, 1.0f);

        // spawn branches
        do
        {
            auto spawn_deco = [&](Entity* branch, float w)
            {
                Entity* deco = layer_ptr->spawn_entity_over(tree_deco, branch, 0.0f, 0.49f);
                deco->animation_frame = 7 * 12 + 3 + static_cast<uint16_t>(prng.random_int(0, 2, PRNG::PRNG_CLASS::ENTITY_VARIATION).value_or(0)) * 12;
                deco->w *= w;
            };
            if (prng.random_chance(2, PRNG::PRNG_CLASS::ENTITY_VARIATION))
            {
                Entity* branch = layer_ptr->spawn_entity_over(tree_branch, base, 1.02f, 0.0f);
                spawn_deco(branch, 1.0f);
            }
            if (prng.random_chance(2, PRNG::PRNG_CLASS::ENTITY_VARIATION))
            {
                Entity* branch = layer_ptr->spawn_entity_over(tree_branch, base, -1.02f, 0.0f);
                branch->w = -1.0f;
                spawn_deco(branch, -1.0f);
            }
            base = base->overlay;
        } while (base->overlay);
    }
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

        std::pair<float, float> offset_position;
        uint8_t actual_layer = enum_to_layer(layer, offset_position);

        aabb.left += offset_position.first;
        aabb.right += offset_position.first;
        aabb.top += offset_position.second;
        aabb.bottom += offset_position.second;

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

struct EntityFactory;
using SpawnEntityFun = Entity*(EntityFactory*, std::uint32_t, float, float, bool, Entity*, bool);
SpawnEntityFun* g_spawn_entity_trampoline{nullptr};
Entity* spawn_entity(EntityFactory* entity_factory, std::uint32_t entity_type, float x, float y, bool layer, Entity* overlay, bool some_bool)
{
    const auto theme_floor = State::get().ptr_local()->current_theme->get_floor_spreading_type();
    const bool is_floor_spreading = (entity_type == theme_floor) && (g_SpawnTypeFlags & SPAWN_TYPE_LEVEL_GEN) && !(g_SpawnTypeFlags & SPAWN_TYPE_LEVEL_GEN_TILE_CODE);
    if (is_floor_spreading)
    {
        push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING);
    }

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

void spawn_player(int8_t player_slot, float x, float y)
{
    if (player_slot < 1 || player_slot > 4)
        return;

    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    using spawn_player_fun = void(Items*, uint8_t ps, float pos_x, float pos_y);
    static auto spawn_player = (spawn_player_fun*)get_address("spawn_player");
    spawn_player(get_state_ptr()->items, player_slot - 1, x, y);
}

int32_t spawn_companion(ENT_TYPE companion_type, float x, float y, LAYER layer)
{
    auto offset = get_address("spawn_companion");
    if (offset != 0)
    {
        push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
        OnScopeExit pop{[]
                        { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

        auto state = get_state_ptr();
        typedef Player* spawn_companion_func(StateMemory*, float x, float y, size_t layer, uint32_t entity_type);
        static spawn_companion_func* sc = (spawn_companion_func*)(offset);
        Player* spawned = sc(state, x, y, enum_to_layer(layer), companion_type);
        return spawned->uid;
    }
    return -1;
}
