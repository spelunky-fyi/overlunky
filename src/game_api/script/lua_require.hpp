#pragma once

// This implementation makes the loaded chunk inherit the env from the loading chunk
int custom_loader(struct lua_State* L);
