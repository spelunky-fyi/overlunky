#pragma once

#include <cstdint>     // for int32_t
#include <type_traits> // for is_function, move, forward, ...

#include "lua_backend.hpp" // for LuaBackend
#include "util.hpp"        // for ON_SCOPE_EXIT, is_instantiation_of

// Generate a thread-safe callback wrapper that is bound to the calling LuaBackend.
// Use inside a C++ function bound to Lua as following examples:
//
// Lua side pseudo-signature:
//      function() -> bool
// C++ wrapper generation:
//      make_safe_cb<bool()>(cb);
//
// Lua side pseudo-signature:
//      function(a: number, b: number, c: Entity) -> integer?
// C++ wrapper generation:
//      make_safe_cb<sol::optional<bool>(float, float, Entity*)>(cb);
//
// Lua side pseudo-signature:
//      function(render_ctx: VanillaRenderCtx, e: Entity) -> nil
// C++ wrapper generation:
//      make_safe_cb<void(Entity*)>(cb, FrontBinder{ VanillaRenderContext{} });
//
// Lua side pseudo-signature:
//      function(e: Entity, render_ctx: VanillaRenderCtx) -> nil
// C++ wrapper generation:
//      make_safe_clearable_cb<void(Entity*)>(cb, uid, cb_id, FrontBinder{}, BackBinder{ VanillaRenderContext{} });
//
// Notes:
//      - `cb` in these examples can also be a lambda
//      - which may or may not take a `LuaBackend&` as its first argument,
//      - you may also pass as objects to `FrontBinder` and `BackBinder` as you like
//      - and these objects may also be functors to lazily evaluate the bound values
//      - and said functors allow the same signatures as `cb` or just `auto()`
//
// Crazy example...
// Lua side pseudo-signature:
//      function(ent_ctx: EntityContext,
//               ent: Entity,
//               state_less_ctx: Stateless,
//               state_full_ctx: StateFullContext) -> AABB
// C++ wrapper generation that is passed as a `std::function<AABB(Entity*)>`:
//      make_safe_cb<AABB(Entity*)>(
//          cb,
//          FrontBinder{
//              [](Entity* e) { return EntityContext{ e }; }
//          },
//          BackBinder{
//              StateLessContext{},
//              [](LuaBackend& backend, Entity* e) {
//                  return backend.make_stateful_context(e);
//              }
//          });
// This code is equivalent to the following lambda:
//    [cb, state_less = StateLessContext{}](Entity* ent) -> AABB
//    {
//        auto backend = LuaBackend::get_calling_backend(); // acquire mutex
//        return backend->handle_function_with_return<AABB>(
//            cb,
//            EntityContext{ ent },
//            ent,
//            state_less,
//            backend->make_stateful_context(ent)
//        );
//    }
// So clearly for complex cases like this it is more readable to make a lambda.
// The benefit of these wrappers is just that you don't have to think about different
// types of callables (e.g. sol::function vs lambda vs function pointer, etc.) and
// that you don't need to manually worry about thread-safety.

template <class... T>
struct FrontBinder;
template <>
struct FrontBinder<> {};
template <class... T>
struct BackBinder;
template <>
struct BackBinder<> {};

template <class FunT, class CallableT, class... FrontTs, class... BackTs>
requires(std::is_function_v<FunT>)
static auto make_safe_cb(
    CallableT&& cb,
    FrontBinder<FrontTs...> front_binder = FrontBinder<>{},
    BackBinder<BackTs...> back_binder = BackBinder<>{});

template <class FunT, CallbackType CbType, class CallableT, class... FrontTs, class... BackTs>
requires(std::is_function_v<FunT>)
static auto make_safe_clearable_cb(
    CallableT&& cb,
    int32_t id,
    int32_t aux_id,
    FrontBinder<FrontTs...> front_binder = FrontBinder<>{},
    BackBinder<BackTs...> back_binder = BackBinder<>{});

#include "safe_cb.inl"
