#pragma once

#include <functional>  // for invoke
#include <sol/sol.hpp> // for object, function

#include "safe_cb.hpp"

#include "handle_lua_function.hpp" // for handle_function_raw, handle_function

template <class T>
struct Binder
{
    T val;

    template <class... ArgsT>
    decltype(auto) get(LuaBackend& backend, ArgsT&&... args);
};
template <class... T>
struct FrontBinder : Binder<T>...
{
};
template <class... T>
struct BackBinder : Binder<T>...
{
};
template <class... Ts>
FrontBinder(Ts&&...) -> FrontBinder<Ts...>;
template <class... Ts>
BackBinder(Ts&&...) -> BackBinder<Ts...>;

namespace detail
{
// clang-format off
template<class T, class... ArgsT>
inline constexpr auto is_auto_invocable_v =
        std::is_same_v<std::decay_t<T>, sol::function>
    || std::is_invocable_v<T, LuaBackend&, ArgsT...>
    || std::is_invocable_v<T, ArgsT...>;
template<class T, class RetT, class... ArgsT>
inline constexpr auto is_invocable_v =
        std::is_same_v<std::decay_t<T>, sol::function>
    || std::is_invocable_r_v<RetT, T, LuaBackend&, ArgsT...>
    || std::is_invocable_r_v<RetT, T, ArgsT...>;

struct auto_tag_t {};
// clang-format on

template <
    class RetT,
    class CallableT,
    class... FrontTs,
    class... BackTs,
    class... ArgsT>
decltype(auto) invoke(
    CallableT&& callable,
    [[maybe_unused]] LuaBackend& backend,
    FrontBinder<FrontTs...> front_binder,
    BackBinder<BackTs...> back_binder,
    ArgsT&&... args)
{
    using FrontBinderT = FrontBinder<FrontTs...>;
    using BackBinderT = BackBinder<BackTs...>;
#define COMMON_ARGS_TS                                                                                                     \
    decltype(std::declval<FrontBinderT>().Binder<FrontTs>::get(std::declval<LuaBackend&>(), std::declval<ArgsT>()...))..., \
        ArgsT...,                                                                                                          \
        decltype(std::declval<BackBinderT>().Binder<BackTs>::get(std::declval<LuaBackend&>(), std::declval<ArgsT>()...))...
    static constexpr auto invocable{is_invocable_v<
        CallableT,
        RetT,
        COMMON_ARGS_TS>};
    static constexpr auto auto_invocable{is_auto_invocable_v<
        CallableT,
        COMMON_ARGS_TS>};
    static_assert(invocable || auto_invocable, "Can't call this cb...");
#undef COMMON_ARGS_TS

#define COMMON_ARGS front_binder.Binder<FrontTs>::get(backend, args...)..., \
                    std::move(args)...,                                     \
                    back_binder.Binder<BackTs>::get(backend, args...)...
    if constexpr (std::is_same_v<std::decay_t<CallableT>, sol::function>)
    {
        if constexpr (std::is_same_v<RetT, auto_tag_t>)
        {
            return handle_function_raw(&backend, callable, COMMON_ARGS);
        }
        else if constexpr (std::is_void_v<RetT>)
        {
            (void)handle_function<void>(&backend, callable, COMMON_ARGS);
            return;
        }
        else if constexpr (is_optional_v<RetT>)
        {
            return handle_function<unwrap_optional_t<RetT>>(&backend, callable, COMMON_ARGS);
        }
        else
        {
            return handle_function<unwrap_optional_t<RetT>>(&backend, callable, COMMON_ARGS).value_or(RetT{});
        }
    }
    else if constexpr (std::is_invocable_r_v<RetT, CallableT, LuaBackend&, ArgsT...>)
    {
        return std::invoke(callable, backend, COMMON_ARGS);
    }
    else
    {
        return std::invoke(callable, COMMON_ARGS);
    }
#undef COMMON_ARGS
}

template <class FunT>
struct make_safe_cb_impl;
template <class RetT, class... ArgsT>
struct make_safe_cb_impl<RetT(ArgsT...)>
{
    template <
        class CallableT,
        class SetCurrentCb,
        class... FrontTs,
        class... BackTs,
        class... TestCallbacksT>
    static auto build(
        CallableT&& cb,
        SetCurrentCb&& set_current_cb,
        FrontBinder<FrontTs...> front_binder,
        BackBinder<BackTs...> back_binder,
        TestCallbacksT&&... tests)
    {
        return [backend_id = LuaBackend::get_calling_backend_id(),
                cb = std::forward<CallableT>(cb),
                set_current_cb = std::forward<SetCurrentCb>(set_current_cb),
                front_binder = std::move(front_binder),
                back_binder = std::move(back_binder),
                tests...](ArgsT&&... args) -> RetT
        {
            auto backend = LuaBackend::get_backend(backend_id);
            if ((tests(*backend) && ...))
            {
                if constexpr (std::is_invocable_v<SetCurrentCb, LuaBackend&>)
                {
                    auto c = set_current_cb(*backend);
                    return invoke<RetT>(cb, *backend, front_binder, back_binder, std::forward<ArgsT>(args)...);
                }
                else
                {
                    (void)set_current_cb;
                    return invoke<RetT>(cb, *backend, front_binder, back_binder, std::forward<ArgsT>(args)...);
                }
            }

            return RetT();
        };
    }
};
} // namespace detail

template <class T>
template <class... ArgsT>
decltype(auto) Binder<T>::get(
    [[maybe_unused]] LuaBackend& backend,
    [[maybe_unused]] ArgsT&&... args)
{
    if constexpr (std::is_invocable_v<T>)
    {
        return std::invoke(val);
    }
    else if constexpr (detail::is_auto_invocable_v<T, ArgsT...>)
    {
        return detail::invoke<detail::auto_tag_t>(val, backend, FrontBinder{}, BackBinder{}, std::forward<ArgsT>(args)...);
    }
    else
    {
        return val;
    }
}

template <function_signature FunT, class CallableT, class... FrontTs, class... BackTs>
static auto make_safe_cb(
    CallableT&& cb,
    FrontBinder<FrontTs...> front_binder,
    BackBinder<BackTs...> back_binder)
{
    return detail::make_safe_cb_impl<FunT>::build(
        std::forward<CallableT>(cb),
        std::monostate{},
        std::move(front_binder),
        std::move(back_binder),
        std::mem_fn(&LuaBackend::get_enabled));
}

template <function_signature FunT, CallbackType CbType, class CallableT, class... FrontTs, class... BackTs>
static auto make_safe_clearable_cb(
    CallableT&& cb,
    int32_t id,
    int32_t aux_id,
    FrontBinder<FrontTs...> front_binder,
    BackBinder<BackTs...> back_binder)
{
    auto set_current_cb = [=](LuaBackend& backend)
    {
        return backend.set_current_callback(aux_id, id, CbType);
    };

#define COMMON_ARGS std::forward<CallableT>(cb), \
                    set_current_cb,              \
                    std::move(front_binder),     \
                    std::move(back_binder),      \
                    std::mem_fn(&LuaBackend::get_enabled)

    // No std::bind_back  :(
    if constexpr (CbType == CallbackType::None || CbType == CallbackType::Normal)
    {
        return detail::make_safe_cb_impl<FunT>::build(
            COMMON_ARGS,
            [=](const LuaBackend& backend)
            { return !backend.is_callback_cleared(id); });
    }
    else if constexpr (CbType == CallbackType::Entity)
    {
        return detail::make_safe_cb_impl<FunT>::build(
            COMMON_ARGS,
            [=](const LuaBackend& backend)
            { return !backend.HookHandler<Entity, CallbackType::Entity>::is_hook_cleared(id, aux_id); });
    }
    else if constexpr (CbType == CallbackType::Theme)
    {
        return detail::make_safe_cb_impl<FunT>::build(
            COMMON_ARGS,
            [=](const LuaBackend& backend)
            { return !backend.HookHandler<ThemeInfo, CallbackType::Theme>::is_hook_cleared(id, aux_id); });
    }
    else if constexpr (CbType == CallbackType::Screen)
    {
        return detail::make_safe_cb_impl<FunT>::build(
            COMMON_ARGS,
            [=](const LuaBackend& backend)
            { return !backend.is_screen_callback_cleared({aux_id, id}); });
    }
#undef COMMON_ARGS
}
