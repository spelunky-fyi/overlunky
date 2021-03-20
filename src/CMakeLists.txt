# --------------------------------------------------
# imgui
set(imgui_sources
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_internal.h
        imgui/backends/imgui_impl_dx11.cpp
        imgui/backends/imgui_impl_dx11.h
        imgui/backends/imgui_impl_win32.cpp
        imgui/backends/imgui_impl_win32.h
        imgui/imgui_widgets.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_demo.cpp)
add_library(imgui STATIC ${imgui_sources})
target_include_directories(imgui PUBLIC imgui)

# --------------------------------------------------
# sol
set(BUILD_LUA_AS_DLL OFF)
set(SOL2_LUA_VERSION "5.4")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/sol2/cmake/Modules")
find_package(LuaBuild REQUIRED COMPONENTS ${SOL2_LUA_VERSION})
add_subdirectory(sol2)

# --------------------------------------------------
# fmt
set(FMT_MASTER_PROJECT OFF)
add_subdirectory(fmt)

# --------------------------------------------------
# overlunky spel2_api
add_subdirectory(shared)
add_subdirectory(game_api)

if(BUILD_SPEL2_DLL)
        add_subdirectory(spel2_dll)
endif()

if(BUILD_OVERLUNKY)
        # --------------------------------------------------
        # nyquist
        add_subdirectory(libnyquist)

        # --------------------------------------------------
        # toml11
        option(toml11_BUILD_TEST OFF)
        add_subdirectory(toml11)

        # --------------------------------------------------
        # overlunky exe and dll
        add_subdirectory(injected)
        add_subdirectory(injector)
endif()
