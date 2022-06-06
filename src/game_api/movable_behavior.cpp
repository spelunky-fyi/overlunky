#include "movable_behavior.hpp"

#include "movable.hpp"
#include "search.hpp"


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
    for (Movable* movable : using_movables)
    {
        // Reset to default behaviors, as soon as one custom behavior is removed this
        // whole state machine can not possibly be valid anymore
        clear_behaviors(movable);
        movable->apply_db();
    }
}

template<auto OriginalFun>
void call_custom_or_original(const auto& custom, VanillaMovableBehavior* base, Movable* movable)
{
    if (custom)
    {
        if (base)
        {
            custom(movable, [=](Movable* movable) {
                (base->*OriginalFun)(movable);
            });
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

template<auto OriginalFun, class T>
T call_custom_or_original(const auto& custom, VanillaMovableBehavior* base, T fallback_return, Movable* movable)
{
    if (custom)
    {
        if (base)
        {
            return custom(movable, [=](Movable* movable) {
                return (base->*OriginalFun)(movable);
            });
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

uint8_t CustomMovableBehavior::get_state_id() const {
    return state_id;
}
uint8_t CustomMovableBehavior::secondary_sort_id() const {
    return 255;
}
bool CustomMovableBehavior::force_state(Movable* movable) {
    return call_custom_or_original<&VanillaMovableBehavior::force_state>(custom_force_state, base_behavior, false, movable);
}
void CustomMovableBehavior::on_enter(Movable* movable) {
    call_custom_or_original<&VanillaMovableBehavior::on_enter>(custom_on_enter, base_behavior, movable);
}
void CustomMovableBehavior::on_exit(Movable* movable) {
    call_custom_or_original<&VanillaMovableBehavior::on_exit>(custom_on_exit, base_behavior, movable);
}
void CustomMovableBehavior::update_render(Movable* movable) {
    call_custom_or_original<&VanillaMovableBehavior::update_render>(custom_update_render, base_behavior, movable);
}
void CustomMovableBehavior::update_physics(Movable* movable) {
    call_custom_or_original<&VanillaMovableBehavior::update_physics>(custom_update_physics, base_behavior, movable);
}
uint8_t CustomMovableBehavior::get_next_state_id(Movable* movable) {
    return call_custom_or_original<&VanillaMovableBehavior::get_next_state_id>(custom_get_next_state_id, base_behavior, state_id, movable);
}

void CustomMovableBehavior::hook_movable(Movable* movable)
{
    using_movables.push_back(movable);
    movable->set_on_dtor([=](Entity*) {
        std::erase(using_movables, movable);
    });
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
    update_movable_impl(*movable, move, sprint_factor, true, disable_gravity, on_rope, false);
}

#ifdef HOOK_MOVE_ENTITY
UpdateMovable* g_update_movable_trampoline{nullptr};
void update_movable(Movable& movable, const Vec2& move_xy, float sprint_factor, bool apply_move, bool disable_gravity, bool on_ladder, bool param_7)
{
	g_update_movable_trampoline(movable, move_xy, sprint_factor, apply_move, disable_gravity, on_ladder, param_7);
}

#include <detours.h>

void init_behavior_hooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    auto memory = Memory::get();
    g_update_movable_trampoline = (UpdateMovable*)memory.at_exe(0x228e3580);

    DetourAttach((void**)&g_update_movable_trampoline, (UpdateMovable*)update_movable);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking MoveEntity: {}\n", error);
    }
}
#else
void init_behavior_hooks() {}
#endif
