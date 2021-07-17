#pragma once

class LuaBackend;
void load_libraries(LuaBackend* backend);
void load_unsafe_libraries(LuaBackend* backend);
void populate_lua_state(LuaBackend* backend);
