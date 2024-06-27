#pragma once

#include <cstdint>       // for uint32_t, uint8_t
#include <functional>    // for equal_to, function
#include <memory>        // for allocator
#include <new>           // for operator new
#include <type_traits>   // for hash
#include <unordered_map> // for _Umap_traits<>::allocator_type, unordered_map

#include "math.hpp" // for Vec2

class Movable;
struct MovableBehavior;

struct SortMovableBehavior
{
    bool operator()(const MovableBehavior* lhs, const MovableBehavior* rhs) const;
};

struct MovableBehavior
{
    virtual uint8_t get_state_id() const = 0;
    virtual uint8_t secondary_sort_id() const = 0;
    virtual bool force_state(Movable* movable) = 0;
    virtual void on_enter(Movable* movable) = 0;
    virtual void on_exit(Movable* movable) = 0;
    virtual void update_logic(Movable* movable) = 0;
    virtual void update_world(Movable* movable) = 0;
    virtual uint8_t get_next_state_id(Movable* movable) = 0;

    // To shut up clang
    virtual ~MovableBehavior(){};
};

struct VanillaMovableBehavior : MovableBehavior
{
};

struct CustomMovableBehavior final : MovableBehavior
{
    uint8_t state_id{};
    std::function<bool(Movable*, std::function<bool(Movable*)>)> custom_force_state;
    std::function<void(Movable*, std::function<void(Movable*)>)> custom_on_enter;
    std::function<void(Movable*, std::function<void(Movable*)>)> custom_on_exit;
    std::function<void(Movable*, std::function<void(Movable*)>)> custom_update_logic;
    std::function<void(Movable*, std::function<void(Movable*)>)> custom_update_world;
    std::function<uint8_t(Movable*, std::function<uint8_t(Movable*)>)> custom_get_next_state_id;
    VanillaMovableBehavior* base_behavior;

    std::unordered_map<Movable*, uint32_t> using_movables;

    ~CustomMovableBehavior();

    virtual uint8_t get_state_id() const override
    {
        return state_id;
    }
    virtual uint8_t secondary_sort_id() const override
    {
        return 255;
    }
    virtual bool force_state(Movable* movable) override;
    virtual void on_enter(Movable* movable) override;
    virtual void on_exit(Movable* movable) override;
    virtual void update_logic(Movable* movable) override;
    virtual void update_world(Movable* movable) override;
    virtual uint8_t get_next_state_id(Movable* movable) override;

    void hook_movable(Movable* movable);
};

/// Gets a vanilla behavior from this movable, needs to be called before `clear_behaviors`
/// but the returned values are still valid after a call to `clear_behaviors`
VanillaMovableBehavior* get_base_behavior(Movable* movable, uint32_t state_id);
/// Add a behavior to this movable, can be either a `VanillaMovableBehavior` or a
/// `CustomMovableBehavior`
void add_behavior(Movable* movable, MovableBehavior* behavior);
/// Clear a specific behavior of this movable, can be either a `VanillaMovableBehavior` or a
/// `CustomMovableBehavior`, a behavior with this behaviors `state_id` may be required to
/// run this movables statemachine without crashing, so add a new one if you are not sure
void clear_behavior(Movable* movable, MovableBehavior* behavior);
/// Clears all behaviors of this movable, need to call `add_behavior` to avoid crashing
void clear_behaviors(Movable* movable);

/// Move a movable according to its velocity, can disable gravity
/// Will also update `movable.animation_frame` and various timers and counters
void update_movable(Movable* movable, bool disable_gravity);
/// Move a movable according to its velocity, update physics, gravity, etc.
/// Will also update `movable.animation_frame` and various timers and counters
inline void update_movable(Movable* movable)
{
    update_movable(movable, false);
}
/// Move a movable according to its velocity and `move`, if the movables `BUTTON.RUN` is
/// held apply `sprint_factor` on `move.x`, can disable gravity or lock its horizontal
/// movement via `on_rope`. Use this for example to update a custom enemy type.
/// Will also update `movable.animation_frame` and various timers and counters
void update_movable(Movable* movable, Vec2 move, float sprint_factor, bool disable_gravity, bool on_rope);

void set_entity_turning(class Entity* entity, bool enabled);

void init_behavior_hooks();
