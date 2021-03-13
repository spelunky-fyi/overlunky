add_library(injected SHARED
        ui.cpp ui.hpp
        main.cpp)
target_link_libraries(injected
        shared
        spel2_api
        imgui
        toml11::toml11
        Shlwapi)
target_include_directories(injected PRIVATE
        ${LUA_INCLUDE_DIRS})

if (MSVC)
        target_compile_options(injected PRIVATE /bigobj)
endif()
