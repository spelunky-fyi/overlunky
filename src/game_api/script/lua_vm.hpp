#pragma once

#include <memory>      // for shared_ptr
#include <sol/sol.hpp> // for environment, protected_function_result
#include <string_view> // for string_view

std::shared_ptr<sol::state> acquire_lua_vm(class SoundManager* sound_manager = nullptr);
sol::state& get_lua_vm(class SoundManager* sound_manager = nullptr);

sol::protected_function_result execute_lua(sol::environment& env, std::string_view code);

void populate_lua_env(sol::environment& env);
void hide_unsafe_libraries(sol::environment& env);
void expose_unsafe_libraries(sol::environment& env);
