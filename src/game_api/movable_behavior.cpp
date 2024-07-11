#include "movable_behavior.hpp"

#include <detours.h> // for DetourTransactionBegin, DetourUpdateThread, ...
#include <list>      // for _List_iterator, _List_const_ite...
#include <map>       // for _Tree_iterator, _Tree_const_ite...
#include <string>    // for operator""sv
#include <utility>   // for min, max, pair

#include "containers/custom_map.hpp" // for custom_map
#include "containers/custom_set.hpp" // for custom_set
#include "memory.hpp"                // for Memory
#include "movable.hpp"               // for Movable
#include "search.hpp"                // for get_address
#include "util.hpp"                  // for OnScopeExit
#include "virtual_table.hpp"         // for get_virtual_function_address, ...
#include "vtable_hook.hpp"           // for register_hook_function

class Entity;

bool SortMovableBehavior::operator()(const MovableBehavior* lhs, const MovableBehavior* rhs) const
{
    const auto lhs_id = lhs->get_state_id();
    const auto rhs_id = rhs->get_state_id();
    return lhs_id == rhs_id
               ? lhs->secondary_sort_id() > rhs->secondary_sort_id()
               : lhs_id > rhs_id;
}

CustomMovableBehavior::~CustomMovableBehavior()
{
    for (auto& [movable, hook] : using_movables)
    {
        // Reset to default behaviors, as soon as one custom behavior is removed this
        // whole state machine can not possibly be valid anymore
        clear_behaviors(movable);
        movable->apply_db();
        movable->clean_on_dtor(hook);
    }
}

template <auto OriginalFun>
void call_custom_or_original(const auto& custom, VanillaMovableBehavior* base, Movable* movable)
{
    if (custom)
    {
        if (base)
        {
            custom(movable, [=](Movable* _movable)
                   { (base->*OriginalFun)(_movable); });
        }
        else
        {
            custom(movable, nullptr);
        }
    }
    else if (base)
    {
        (base->*OriginalFun)(movable);
    }
}

template <auto OriginalFun, class T>
T call_custom_or_original(const auto& custom, VanillaMovableBehavior* base, T fallback_return, Movable* movable)
{
    if (custom)
    {
        if (base)
        {
            return custom(movable, [=](Movable* _movable)
                          { return (base->*OriginalFun)(_movable); });
        }
        else
        {
            return custom(movable, nullptr);
        }
    }
    else if (base)
    {
        return (base->*OriginalFun)(movable);
    }

    return fallback_return;
}

bool CustomMovableBehavior::force_state(Movable* movable)
{
    return call_custom_or_original<&VanillaMovableBehavior::force_state>(custom_force_state, base_behavior, false, movable);
}
void CustomMovableBehavior::on_enter(Movable* movable)
{
    call_custom_or_original<&VanillaMovableBehavior::on_enter>(custom_on_enter, base_behavior, movable);
}
void CustomMovableBehavior::on_exit(Movable* movable)
{
    call_custom_or_original<&VanillaMovableBehavior::on_exit>(custom_on_exit, base_behavior, movable);
}
void CustomMovableBehavior::update_logic(Movable* movable)
{
    call_custom_or_original<&VanillaMovableBehavior::update_logic>(custom_update_logic, base_behavior, movable);
}
void CustomMovableBehavior::update_world(Movable* movable)
{
    call_custom_or_original<&VanillaMovableBehavior::update_world>(custom_update_world, base_behavior, movable);
}
uint8_t CustomMovableBehavior::get_next_state_id(Movable* movable)
{
    return call_custom_or_original<&VanillaMovableBehavior::get_next_state_id>(custom_get_next_state_id, base_behavior, state_id, movable);
}

void CustomMovableBehavior::hook_movable(Movable* movable)
{
    using_movables[movable] =
        movable->set_on_dtor([=, this](Entity*)
                             { using_movables.erase(movable); });
}

VanillaMovableBehavior* get_base_behavior(Movable* movable, uint32_t state_id)
{
    auto it = movable->behaviors_map.find(state_id);
    if (it != movable->behaviors_map.end() && it->second->secondary_sort_id() != 255)
    {
        return static_cast<VanillaMovableBehavior*>(it->second);
    }
    return nullptr;
}
void add_behavior(Movable* movable, MovableBehavior* behavior)
{
    using AddBehaviorFun = void(Movable*, uint32_t, MovableBehavior*);
    static AddBehaviorFun* add_behavior_impl = (AddBehaviorFun*)get_address("add_behavior"sv);
    add_behavior_impl(movable, behavior->get_state_id(), behavior);

    if (behavior->secondary_sort_id() == 255)
    {
        CustomMovableBehavior* custom_behavior = static_cast<CustomMovableBehavior*>(behavior);
        custom_behavior->hook_movable(movable);
    }
}
void clear_behavior(Movable* movable, MovableBehavior* behavior)
{
    if (behavior == nullptr)
    {
        return;
    }

    movable->behaviors_map.erase(behavior->get_state_id());
    movable->behaviors.erase(behavior);
    if (movable->current_behavior == behavior)
    {
        movable->current_behavior->on_exit(movable);
        if (movable->behaviors.empty())
        {
            movable->current_behavior = nullptr;
        }
        else
        {
            movable->current_behavior = *movable->behaviors.begin();
            movable->current_behavior->on_enter(movable);
        }
        movable->stand_counter = 0;
    }
}
void clear_behaviors(Movable* movable)
{
    if (movable->current_behavior != nullptr)
    {
        movable->current_behavior->on_exit(movable);
    }

    movable->current_behavior = nullptr;
    movable->idle_counter = 0;

    movable->behaviors_map.clear();
    movable->behaviors.clear();
}

using UpdateMovable = void(Movable&, const Vec2&, float, bool, bool, bool, bool);

void update_movable(Movable* movable, bool disable_gravity)
{
    Vec2 null{};
    update_movable(movable, null, 1.0f, disable_gravity, false);
}
void update_movable(Movable* movable, Vec2 move, float sprint_factor, bool disable_gravity, bool on_rope)
{
    static UpdateMovable* update_movable_impl = (UpdateMovable*)get_address("update_movable"sv);
    update_movable_impl(*movable, move, sprint_factor, false, disable_gravity, on_rope, false);
}

struct TurningEntityInfo
{
    Entity* entity;
    uint32_t dtor_hook;
};
std::vector<TurningEntityInfo> g_entities_disabled_turning{};
void set_entity_turning(class Entity* entity, bool enabled)
{
    auto find_entity_pred = [=](const TurningEntityInfo& turning_info)
    { return turning_info.entity == entity; };

    if (!enabled)
    {
        g_entities_disabled_turning.push_back({
            entity,
            entity->set_on_dtor([=](Entity*)
                                { std::erase_if(g_entities_disabled_turning, find_entity_pred); }),
        });
    }
    else
    {
        auto it = std::find_if(g_entities_disabled_turning.begin(), g_entities_disabled_turning.end(), find_entity_pred);
        if (it == g_entities_disabled_turning.end())
        {
            entity->clean_on_dtor(it->dtor_hook);
            g_entities_disabled_turning.erase(it);
        }
    }
}

using EntityTurn = void(Entity*, bool);
EntityTurn* g_entity_turn_trampoline{nullptr};
void entity_turn(Entity* self, bool apply)
{
    auto find_entity_pred = [=](const TurningEntityInfo& turning_info)
    { return turning_info.entity == self; };

    if (std::find_if(g_entities_disabled_turning.begin(), g_entities_disabled_turning.end(), find_entity_pred) == g_entities_disabled_turning.end())
    {
        g_entity_turn_trampoline(self, apply);
    }
}

#ifdef HOOK_MOVE_ENTITY
UpdateMovable* g_update_movable_trampoline{nullptr};
void update_movable(Movable& movable, const Vec2& move_xy, float sprint_factor, bool apply_move, bool disable_gravity, bool on_ladder, bool param_7)
{
    g_update_movable_trampoline(movable, move_xy, sprint_factor, apply_move, disable_gravity, on_ladder, param_7);
}
#endif

void init_behavior_hooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    auto& memory = Memory::get();

    g_entity_turn_trampoline = (EntityTurn*)memory.at_exe(get_virtual_function_address(VTABLE_OFFSET::MONS_SNAKE, 0x10));
    DetourAttach((void**)&g_entity_turn_trampoline, &entity_turn);

#ifdef HOOK_MOVE_ENTITY
    g_update_movable_trampoline = (UpdateMovable*)memory.at_exe(0x228e3580);
    DetourAttach((void**)&g_update_movable_trampoline, &update_movable);
#endif

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking behavior hooks: {}\n", error);
    }
}
