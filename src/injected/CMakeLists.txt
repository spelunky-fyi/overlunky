add_library(injected SHARED
        script.cpp script.hpp
        ui.cpp ui.hpp
        main.cpp)
target_link_libraries(injected
        shared
        game_api
        imgui
        toml11::toml11
        sol2::sol2
        ${LUA_LIBRARIES}
        Shlwapi)
target_include_directories(injected PRIVATE
        ${LUA_INCLUDE_DIRS})

if (MSVC)
        add_definitions(/bigobj)
endif()
