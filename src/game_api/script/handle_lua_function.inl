#pragma once

#include "handle_lua_function.hpp"

#include <optional>    // for optional
#include <type_traits> // for move, forward

#include <sol/sol.hpp> // for state

#include "entity.hpp"        // for Entity
#include "script/lua_vm.hpp" // for get_lua_vm
#include "util.hpp"          // for ON_SCOPE_EXIT

template <class... ArgsT>
auto handle_function_raw(LuaBackend* calling_backend, sol::function fun, ArgsT&&... args)
{
    LuaBackend::push_calling_backend(calling_backend);
    ON_SCOPE_EXIT(LuaBackend::pop_calling_backend(calling_backend));

    auto lua_result = fun(std::forward<ArgsT>(args)...);

    if (!lua_result.valid())
    {
        sol::error e = lua_result;
        calling_backend->set_error(e.what());
    }

    return lua_result;
}

template <class RetT>
struct handle_function_impl
{
    template <class... ArgsT>
    static std::optional<RetT> call(LuaBackend* calling_backend, sol::function fun, ArgsT&&... args)
    {
        auto lua_result = handle_function_raw(calling_backend, std::move(fun), std::forward<ArgsT>(args)...);
        if (!lua_result.valid())
        {
            return std::nullopt;
        }
        else if constexpr (std::is_same_v<RetT, std::monostate>)
        {
            return std::optional{std::monostate{}};
        }
        else
        {
            try
            {
                auto return_type = lua_result.get_type();
                if (return_type == sol::type::none || return_type == sol::type::nil)
                {
                    return std::optional<RetT>{};
                }

                RetT return_value = lua_result;
                return return_value;
            }
            catch (...)
            {
                calling_backend->set_error("Unexpected return type from function...");
                return std::nullopt;
            }
        }
    }
};
template <>
struct handle_function_impl<void>
{
    template <class... ArgsT>
    static bool call(LuaBackend* calling_backend, sol::function fun, ArgsT&&... args)
    {
        return handle_function_impl<std::monostate>::call(calling_backend, std::move(fun), std::forward<ArgsT>(args)...) != std::nullopt;
    }
};

template <class RetT, class... ArgsT>
auto handle_function_with_cast_entities(LuaBackend* calling_backend, sol::function fun, ArgsT&&... args)
{
    return handle_function_impl<RetT>::call(calling_backend, std::move(fun), std::forward<ArgsT>(args)...);
}

template <class T>
concept entity_ptr = std::derived_from<std::remove_pointer_t<std::remove_reference_t<T>>, Entity>;
inline auto cast_entity(Entity* ent)
{
    static auto& lua{get_lua_vm()};
    return lua["cast_entity"](ent);
}

template <class T>
struct forward_or_cast_entity_impl
{
    static T&& call(std::remove_reference_t<T>& val) noexcept
    {
        return static_cast<T&&>(val);
    }
    static T&& call(std::remove_reference_t<T>&& val) noexcept
    {
        static_assert(!std::is_lvalue_reference_v<T>, "bad forward call");
        return static_cast<T&&>(val);
    }
};
template <entity_ptr T>
struct forward_or_cast_entity_impl<T>
{
    static auto call(Entity* val) noexcept
    {
        return cast_entity(val);
    }
};

template <class T>
auto forward_or_cast_entity(T&& val) noexcept
{
    return forward_or_cast_entity_impl<T>::call(val);
}

template <class RetT, class... ArgsT>
optional_function_result<RetT> handle_function(LuaBackend* calling_backend, sol::function fun, ArgsT&&... args)
{
    return handle_function_with_cast_entities<RetT>(calling_backend, std::move(fun), forward_or_cast_entity(std::forward<ArgsT>(args))...);
}
