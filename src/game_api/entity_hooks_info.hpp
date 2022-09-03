#pragma once

#include <cstdint>    // for int8_t, uint16_t, ...
#include <functional> // for function
#include <vector>     // for vector

class Entity;
class Movable;
class Container;

template <class FunT>
struct HookWithId
{
    uint32_t id;
    std::function<FunT> fun;
};

struct EntityHooksInfo
{
    int32_t entity;
    uint32_t cbcount;
    std::vector<HookWithId<void(Entity*)>> on_dtor;
    std::vector<HookWithId<void(Entity*)>> on_destroy;
    std::vector<HookWithId<void(Entity*, Entity*)>> on_kill;
    std::vector<HookWithId<bool(Entity*)>> pre_floor_update;
    std::vector<HookWithId<void(Entity*)>> post_floor_update;
    std::vector<HookWithId<bool(Entity*)>> on_player_instagib;
    std::vector<HookWithId<bool(Entity*, Entity*, int8_t, float, float, uint16_t, uint8_t)>> on_damage;
    std::vector<HookWithId<bool(Movable*)>> pre_statemachine;
    std::vector<HookWithId<void(Movable*)>> post_statemachine;
    std::vector<HookWithId<void(Container*, Movable*)>> on_open;
    std::vector<HookWithId<bool(Entity*, Entity*)>> pre_collision1;
    std::vector<HookWithId<bool(Entity*, Entity*)>> pre_collision2;
    std::vector<HookWithId<bool(Entity*)>> pre_render;
    std::vector<HookWithId<void(Entity*)>> post_render;
};
