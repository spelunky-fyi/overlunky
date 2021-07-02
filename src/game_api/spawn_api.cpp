#include "spawn_api.hpp"

#include "entity.hpp"
#include "layer.hpp"
#include "logger.h"
#include "memory.hpp"
#include "rpc.hpp"
#include "script.hpp"
#include "util.hpp"

#include <Windows.h>
#include <detours.h>

std::uint32_t g_SpawnNonReplacable;
SpawnTypeFlags g_SpawnTypeFlags;
std::array<std::uint32_t, SPAWN_TYPE_NUM_FLAGS> g_SpawnTypes{};

void spawn_liquid(uint32_t entity_type, float x, float y)
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

int32_t spawn_entity(uint32_t entity_type, float x, float y, bool s, float vx, float vy, bool snap)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    auto [_x, _y] = player->position();
    if (!s)
    {
        DEBUG("Spawning {} on {}, {}", entity_type, x + _x, y + _y);
        return state.layer(player->layer)->spawn_entity(entity_type, x + _x, y + _y, s, vx, vy, snap)->uid;
    }
    else
    {
        DEBUG("Spawning {} on screen {}, {}", entity_type, x, y);
        return state.layer(player->layer)->spawn_entity(entity_type, x, y, s, vx, vy, snap)->uid;
    }
}

int32_t spawn_entity_abs(uint32_t entity_type, float x, float y, int layer, float vx, float vy)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();
    if (layer == 0 || layer == 1)
    {
        return state.layer(layer)->spawn_entity(entity_type, x, y, false, vx, vy, false)->uid;
    }
    else if (layer < 0)
    {
        auto player = state.items()->player(abs(layer) - 1);
        if (player == nullptr)
            return -1;
        auto [_x, _y] = player->position();
        DEBUG("Spawning {} on {}, {}", entity_type, x + _x, y + _y);
        return state.layer(player->layer)->spawn_entity(entity_type, x + _x, y + _y, false, vx, vy, false)->uid;
    }
    return -1;
}

int32_t spawn_entity_abs_nonreplaceable(uint32_t entity_type, float x, float y, int layer, float vx, float vy)
{
    g_SpawnNonReplacable++;
    OnScopeExit pop{[]
                    { g_SpawnNonReplacable--; }};
    return spawn_entity_abs(entity_type, x, y, layer, vx, vy);
}

int32_t spawn_entity_over(uint32_t entity_type, uint32_t over_uid, float x, float y)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();
    Entity* overlay = get_entity_ptr(over_uid);
    if (overlay == nullptr)
        return -1;
    int layer = overlay->layer;
    return state.layer(layer)->spawn_entity_over(entity_type, overlay, x, y)->uid;
}

int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    auto [_x, _y] = player->position();
    DEBUG("Spawning door on {}, {}", x + _x, y + _y);
    state.layer(player->layer)->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    return state.layer(player->layer)->spawn_door(x + _x, y + _y, w, l, t)->uid;
}

int32_t spawn_door_abs(float x, float y, int layer, uint8_t w, uint8_t l, uint8_t t)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();
    if (layer == 0 || layer == 1)
    {
        return state.layer(layer)->spawn_door(x, y, w, l, t)->uid;
    }
    else if (layer < 0)
    {
        auto player = state.items()->player(abs(layer) - 1);
        if (player == nullptr)
            return -1;
        auto [_x, _y] = player->position();
        DEBUG("Spawning door on {}, {}", x + _x, y + _y);
        return state.layer(player->layer)->spawn_door(x + _x, y + _y, w, l, t)->uid;
    }
    return -1;
}

void spawn_backdoor(float x, float y)
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
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
}

void spawn_backdoor_abs(float x, float y)
{
    push_spawn_type_flags(SPAWN_TYPE_SCRIPT);
    OnScopeExit pop{[]
                    { pop_spawn_type_flags(SPAWN_TYPE_SCRIPT); }};

    auto state = State::get();
    DEBUG("Spawning backdoor on {}, {}", x, y);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0, false, 0.0, 0.0, true);
}

void push_spawn_type_flags(SpawnTypeFlags flags)
{
    for (size_t i = 0; i < g_SpawnTypes.size(); i++)
    {
        if (flags & (1 << i))
        {
            g_SpawnTypes[i]++;
        }
    }

    g_SpawnTypeFlags = 0;
    g_SpawnTypeFlags |= g_SpawnTypes[SPAWN_TYPE_LEVEL_GEN] ? SPAWN_TYPE_LEVEL_GEN : 0;
    g_SpawnTypeFlags |= g_SpawnTypes[SPAWN_TYPE_SCRIPT] ? SPAWN_TYPE_SCRIPT : 0;
    g_SpawnTypeFlags |= g_SpawnTypeFlags == 0 ? SPAWN_TYPE_SYSTEMIC : 0;
}
void pop_spawn_type_flags(SpawnTypeFlags flags)
{
    for (size_t i = 0; i < g_SpawnTypes.size(); i++)
    {
        if (flags & (1 << i))
        {
            g_SpawnTypes[i]--;
        }
    }

    g_SpawnTypeFlags = 0;
    g_SpawnTypeFlags |= g_SpawnTypes[SPAWN_TYPE_LEVEL_GEN] ? SPAWN_TYPE_LEVEL_GEN : 0;
    g_SpawnTypeFlags |= g_SpawnTypes[SPAWN_TYPE_SCRIPT] ? SPAWN_TYPE_SCRIPT : 0;
    g_SpawnTypeFlags |= g_SpawnTypeFlags == 0 ? SPAWN_TYPE_SYSTEMIC : 0;
}

//#define HOOK_LOAD_ITEM
#ifdef HOOK_LOAD_ITEM
using LoadItemFun = void*(Layer*, std::uint32_t, float, float);
LoadItemFun* g_load_item_trampoline{nullptr};
void* load_item(Layer* _this, std::uint32_t entity_id, float x, float y)
{
    return g_load_item_trampoline(_this, entity_id, x, y);
}
#endif

struct EntityStore;
using SpawnEntityFun = Entity*(EntityStore*, std::uint32_t, float, float, bool, Entity*, bool);
SpawnEntityFun* g_spawn_entity_trampoline{nullptr};
Entity* spawn_entity(EntityStore* entity_store, std::uint32_t entity_type, float x, float y, bool layer, Entity* overlay, bool some_bool)
{
    Entity* spawned_ent{nullptr};
    if (g_SpawnNonReplacable == 0)
    {
        SpelunkyScript::for_each_script([=, &spawned_ent](SpelunkyScript& script)
                                        {
                                            spawned_ent = script.pre_entity_spawn(entity_type, x, y, layer, overlay, g_SpawnTypeFlags);
                                            return spawned_ent == nullptr;
                                        });
    }

    if (spawned_ent == nullptr)
    {
        spawned_ent = g_spawn_entity_trampoline(entity_store, entity_type, x, y, layer, overlay, some_bool);
    }

    SpelunkyScript::for_each_script([spawned_ent](SpelunkyScript& script)
                                    {
                                        script.post_entity_spawn(spawned_ent, g_SpawnTypeFlags);
                                        return true;
                                    });
    return spawned_ent;
}

void init_spawn_hooks()
{
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

#ifdef HOOK_LOAD_ITEM
        auto load_item_off = find_inst(exe, "\x48\x89\x5c\x24\x10\x48\x89\x6c\x24\x18\x56\x57\x41\x56\x48\x83\xec\x60\x48\x8b\xf1\x0f\xb6\x01\xc6\x44\x24\x30\x00\x48\xc7\x44\x24\x28\x00\x00\x00\x00\x88\x44\x24\x20"s, after_bundle);
        g_load_item_trampoline = (LoadItemFun*)memory.at_exe(load_item_off);
        DetourAttach((void**)&g_load_item_trampoline, load_item);
#endif

        auto spawn_entity_off = find_inst(memory.exe(), "\xba\x79\x00\x00\x00\x41\x0f\xb6\x06\x88\x44\x24\x20"s, memory.after_bundle);
        auto spawn_entity_call = find_inst(memory.exe(), "\xE8"s, spawn_entity_off);
        auto spawn_entity_start = decode_call(spawn_entity_call);
        g_spawn_entity_trampoline = (SpawnEntityFun*)memory.at_exe(spawn_entity_start);
        DetourAttach((void**)&g_spawn_entity_trampoline, (SpawnEntityFun*)spawn_entity);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking SpawnEntity: {}\n", error);
        }
    }
}
