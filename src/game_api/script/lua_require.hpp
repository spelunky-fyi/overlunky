#pragma once

// This implementation makes the loaded chunk inherit the env from the loading chunk
sol::object custom_require(std::string path);
int custom_loader(struct lua_State* L);
