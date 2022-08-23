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
        movable->unhook(hook);
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

uint8_t CustomMovableBehavior::get_state_id() const
{
    return state_id;
}
uint8_t CustomMovableBehavior::secondary_sort_id() const
{
    return 255;
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
        movable->set_on_dtor([=](Entity*)
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

void update_movable(Movable* movable)
{
    update_movable(movable, false);
}
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

struct ClimbableEntityInfo
{
    Entity* entity;
    uint32_t dtor_hook;
    bool climbable;
};
std::vector<ClimbableEntityInfo> g_climbable_entities;
void set_entity_climbable(Entity* entity, bool climbable)
{
    auto find_entity_pred = [=](const ClimbableEntityInfo& climbable_info)
    { return climbable_info.entity == entity; };

    auto it = std::find_if(g_climbable_entities.begin(), g_climbable_entities.end(), find_entity_pred);
    bool climbable_before = (entity->type->properties_flags & 0x20) == 0x20;
    if (it != g_climbable_entities.end())
    {
        if (climbable_before != climbable)
        {
            it->climbable = climbable;
        }
        else
        {
            g_climbable_entities.erase(it);
        }
    }
    else if (climbable_before != climbable)
    {
        g_climbable_entities.push_back({
            entity,
            entity->set_on_dtor([=](Entity*)
                                { std::erase_if(g_climbable_entities, find_entity_pred); }),
            climbable,
        });
    }
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
            entity->unhook(it->dtor_hook);
            g_entities_disabled_turning.erase(it);
        }
    }
}

bool g_expecting_ledge_hang_test{false};

struct ClimbableEntityRecoveryInfo
{
    Entity* entity;
    bool climbable;
};
std::vector<ClimbableEntityRecoveryInfo> g_temporary_climbable_entities;

using GetEntityCloseTo = Entity*(Layer&, float, float, size_t, size_t, size_t);
GetEntityCloseTo* g_get_entity_close_to_trampoline{nullptr};
Entity* get_entity_close_to(Layer& layer, float x, float y, size_t search_flags, size_t include_flags, size_t exclude_flags)
{
    auto test_ent = [](Entity* entity) -> std::optional<bool>
    {
        if (!std::count_if(g_temporary_climbable_entities.begin(), g_temporary_climbable_entities.end(), [=](const ClimbableEntityRecoveryInfo& ent_info)
                           { return ent_info.entity == entity; }))
        {
            auto find_entity_pred = [=](const ClimbableEntityInfo& _info)
            { return _info.entity == entity; };
            auto it = std::find_if(g_climbable_entities.begin(), g_climbable_entities.end(), find_entity_pred);
            if (it != g_climbable_entities.end())
            {
                return it->climbable;
            }
        }
        return std::nullopt;
    };

    Entity* close_ent = g_get_entity_close_to_trampoline(layer, x, y, search_flags, include_flags, exclude_flags);
    const std::optional<bool> climbable = test_ent(close_ent);
    if (g_expecting_ledge_hang_test && close_ent && climbable.has_value())
    {
        bool climbable_before = (close_ent->type->properties_flags & 0x20) == 0x20;
        if (climbable_before != climbable.value())
        {
            if (climbable.value())
            {
                close_ent->type->properties_flags |= 0x20;
            }
            else
            {
                close_ent->type->properties_flags &= ~0x20;
            }
            g_temporary_climbable_entities.push_back({close_ent, climbable_before});
        }
    }
    return close_ent;
}
void reset_clingy_ents()
{
    for (const ClimbableEntityRecoveryInfo& recovery_info : g_temporary_climbable_entities)
    {
        if (recovery_info.climbable)
        {
            recovery_info.entity->type->properties_flags |= 0x20;
        }
        else
        {
            recovery_info.entity->type->properties_flags &= ~0x20;
        }
    }
    g_temporary_climbable_entities.clear();
}

using LedgeGrabForceState = bool(MovableBehavior*, Movable*);
LedgeGrabForceState* g_original_ledge_grab_force_state{nullptr};
bool ledge_grab_force_state(MovableBehavior* behavior, Movable* player)
{
    g_expecting_ledge_hang_test = true;
    ON_SCOPE_EXIT(g_expecting_ledge_hang_test = false; reset_clingy_ents());
    return g_original_ledge_grab_force_state(behavior, player);
}
using LedgeGrabOnEnter = bool(MovableBehavior*, Movable*);
LedgeGrabOnEnter* g_original_ledge_grab_on_enter{nullptr};
void ledge_grab_on_enter(MovableBehavior* behavior, Movable* player)
{
    g_expecting_ledge_hang_test = true;
    ON_SCOPE_EXIT(g_expecting_ledge_hang_test = false; reset_clingy_ents());
    g_original_ledge_grab_on_enter(behavior, player);
}
using LedgeGrabGetNextStateId = uint8_t(MovableBehavior*, Movable*);
LedgeGrabGetNextStateId* g_original_ledge_grab_get_next_state_id{nullptr};
uint8_t ledge_grab_get_next_state_id(MovableBehavior* behavior, Movable* player)
{
    g_expecting_ledge_hang_test = true;
    ON_SCOPE_EXIT(g_expecting_ledge_hang_test = false; reset_clingy_ents());
    return g_original_ledge_grab_get_next_state_id(behavior, player);
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

#define HOOK_MOVE_ENTITY
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

    auto memory = Memory::get();

    g_get_entity_close_to_trampoline = (GetEntityCloseTo*)get_address("layer_get_entity_close_to"sv);
    DetourAttach((void**)&g_get_entity_close_to_trampoline, (GetEntityCloseTo*)get_entity_close_to);

    g_entity_turn_trampoline = (EntityTurn*)get_address("entity_turn"sv);
    DetourAttach((void**)&g_entity_turn_trampoline, (GetEntityCloseTo*)entity_turn);

#ifdef HOOK_MOVE_ENTITY
    g_update_movable_trampoline = (UpdateMovable*)memory.at_exe(0x228e3580);
    DetourAttach((void**)&g_update_movable_trampoline, (UpdateMovable*)update_movable);
#endif

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking behavior hooks: {}\n", error);
    }

    void*** ledge_grab_behavior_vtable = (void***)get_address("player_ledge_hang_behavior_vtable"sv);
    g_original_ledge_grab_force_state = (LedgeGrabForceState*)register_hook_function(ledge_grab_behavior_vtable, 0x2, &ledge_grab_force_state);
    g_original_ledge_grab_on_enter = (LedgeGrabOnEnter*)register_hook_function(ledge_grab_behavior_vtable, 0x3, &ledge_grab_on_enter);
    g_original_ledge_grab_get_next_state_id = (LedgeGrabGetNextStateId*)register_hook_function(ledge_grab_behavior_vtable, 0x7, &ledge_grab_get_next_state_id);
}
