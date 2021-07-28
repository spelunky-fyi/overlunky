#pragma once

#include <sol/forward.hpp>

// This implementation makes the loaded chunk inherit the env from the loading chunk
void register_custom_require(sol::state& lua);
sol::object custom_require(std::string path);
int custom_loader(struct lua_State* L);
