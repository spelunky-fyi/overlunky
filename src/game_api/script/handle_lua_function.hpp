#pragma once

#include <optional> // for optional

#include <sol/sol.hpp> // for function

#include "script/lua_backend.hpp" // for LuaBackend

template <class T>
using optional_function_result = std::conditional_t<
    std::is_void_v<T>,
    bool,
    std::optional<T>>;

// Calls a Lua function and tries to cast the return value to Ret. Will return an empty optional if
// the Lua function returns nothing, nil or caused an error. Errors are logged in the backend.
template <class Ret, class... Args>
optional_function_result<Ret> handle_function(LuaBackend* calling_backend, sol::function func, Args&&... args);

#include "handle_lua_function.inl"
