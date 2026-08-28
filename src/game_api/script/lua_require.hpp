#pragma once

#include <sol/forward.hpp> // for object
#include <string>          // for string

namespace sol
{
class state;
} // namespace sol

// This implementation makes the loaded chunk inherit the env from the loading chunk
void register_custom_require(sol::state& lua);
sol::object custom_require(std::string path);
sol::object custom_loadlib(std::string path, std::string func);
int custom_loader(struct lua_State* L);

// Same for the stock chunk loaders, which would otherwise give the chunk the real _G
int custom_load(struct lua_State* L);
int custom_loadfile(struct lua_State* L);
int custom_dofile(struct lua_State* L);
