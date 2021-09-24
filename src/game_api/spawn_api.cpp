#include "spawn_api.hpp"

#include "entity.hpp"
#include "layer.hpp"
#include "logger.h"
#include "memory.hpp"
#include "rpc.hpp"
#include "util.hpp"

#include "script/events.hpp"

#include <Windows.h>
#include <detours.h>

std::uint32_t g_SpawnNonReplacable;
SPAWN_TYPE g_SpawnTypeFlags;
std::array<std::uint32_t, SPAWN_TYPE_NUM_FLAGS> g_SpawnTypes{};

void spawn_liquid(ENT_TYPE entity_type, float x, float y)
{
    using spawn_liquid_fun_t = void(void*, float, float, std::uint32_t, bool);
    static auto spawn_liquid_call = (spawn_liquid_fun_t*)[]()
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto start = memory.after_bundle;
        auto location = find_inst(exe, "\x41\xb9\x8b\x03\x00\x00\x0f\x28\xd7"s, start) - 0x1;
        location = find_inst(exe, "\xe8"s, location);
        location = decode_pc(exe, location, 1);
        return memory.at_exe(location);
    }
    ();

    auto state = State::get().ptr();
    spawn_liquid_call(state->liquid_physics, x, y, entity_type, false);
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

    auto player = state.items()->player(0); //do the same stuff as in spawn_entity?
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

    using spawn_tree_fun_t = void(void*, int, float, float);
    static auto spawn_tree_call = (spawn_tree_fun_t*)[]()
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto start = memory.after_bundle;
        auto location = find_inst(exe, "\x0f\x28\xd7\x40\x0f\xb6\xd7"s, start);
        location = find_inst(exe, "\xe8"s, location);
        location = decode_pc(exe, location, 1);
        return memory.at_exe(location);
    }
    ();
    spawn_tree_call(nullptr, actual_layer, x + offset_position.first, y + offset_position.second);
}

Entity* spawn_impostor_lake(AABB aabb, LAYER layer, float top_threshold)
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

    static auto impostor_lake_id = to_id("ENT_TYPE_LIQUID_IMPOSTOR_LAKE");
    Entity* impostor_lake = get_entity_ptr(spawn_entity_abs(impostor_lake_id, x, y, (LAYER)actual_layer, 0.0f, 0.0f));
    setup_impostor_lake(impostor_lake, aabb, top_threshold);
    return impostor_lake;
}
void setup_impostor_lake(Entity* lake_impostor, AABB aabb, float top_threshold)
{
    auto layer = State::get().layer(0);
    (void)layer;

    using setup_lake_impostor_fun_t = void(Entity*, float half_width, float half_heigth, float);
    static auto setup_lake_impostor = (setup_lake_impostor_fun_t*)[]()
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto start = memory.after_bundle;
        auto location = find_inst(exe, "\x0f\x28\xcf\xf3\x0f\x59\xcd\x0f\x2e\xc1\x7a\x02"s, start);
        return function_start(memory.at_exe(location));
    }
    ();
    setup_lake_impostor(lake_impostor, aabb.width() / 2.0f, aabb.height() / 2.0f, top_threshold);
}

void update_spawn_type_flags()
{
    g_SpawnTypeFlags = 0;

    g_SpawnTypeFlags |= g_SpawnTypes[0] ? SPAWN_TYPE_LEVEL_GEN_TILE_CODE : 0;
    g_SpawnTypeFlags |= g_SpawnTypes[1] ? SPAWN_TYPE_LEVEL_GEN_PROCEDURAL : 0;
    g_SpawnTypeFlags |= g_SpawnTypes[2] ? SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING : 0;

    // LEVEL_GEN_GENERAL only covers level gen spawns not covered by the others
    if ((g_SpawnTypeFlags & SPAWN_TYPE_LEVEL_GEN) == 0)
    {
        g_SpawnTypeFlags |= g_SpawnTypes[3] ? SPAWN_TYPE_LEVEL_GEN_GENERAL : 0;
    }

    g_SpawnTypeFlags |= g_SpawnTypes[4] ? SPAWN_TYPE_SCRIPT : 0;

    // SYSTEMIC covers everything that isn't covered above
    if (g_SpawnTypeFlags == 0)
    {
        g_SpawnTypeFlags |= SPAWN_TYPE_SYSTEMIC;
    }
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

struct EntityStore;
using SpawnEntityFun = Entity*(EntityStore*, std::uint32_t, float, float, bool, Entity*, bool);
SpawnEntityFun* g_spawn_entity_trampoline{nullptr};
Entity* spawn_entity(EntityStore* entity_store, std::uint32_t entity_type, float x, float y, bool layer, Entity* overlay, bool some_bool)
{
    Entity* spawned_ent{nullptr};
    if (g_SpawnNonReplacable == 0)
    {
        spawned_ent = pre_entity_spawn(entity_type, x, y, layer, overlay, g_SpawnTypeFlags);
    }

    if (spawned_ent == nullptr)
    {
        spawned_ent = g_spawn_entity_trampoline(entity_store, entity_type, x, y, layer, overlay, some_bool);
    }

    post_entity_spawn(spawned_ent, g_SpawnTypeFlags);
    return spawned_ent;
}

using FloorSpreadingFun = Entity*();
FloorSpreadingFun* g_floor_spreading_trampoline{nullptr};
Entity* floor_spreading()
{
    push_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_LEVEL_GEN_FLOOR_SPREADING); }};
    return g_floor_spreading_trampoline();
}

void init_spawn_hooks()
{
    {
        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        g_spawn_entity_trampoline = (SpawnEntityFun*)get_address("spawn_entity");

        // TODO: 1.23.3
        //{
        //    auto floor_spreading_off = find_inst(exe, " 45 0f 57 e4 44 88 94 24 20 01 00 00 44 89 4c 24 44"s, after_bundle);
        //    auto floor_spreading_start = function_start(memory.at_exe(floor_spreading_off));
        //    g_floor_spreading_trampoline = (FloorSpreadingFun*)floor_spreading_start;
        //}

        DetourAttach((void**)&g_spawn_entity_trampoline, (SpawnEntityFun*)spawn_entity);
        //DetourAttach((void**)&g_floor_spreading_trampoline, floor_spreading);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking SpawnEntity: {}\n", error);
        }
    }
}
