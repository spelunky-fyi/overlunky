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
# Detours
add_library(lib_detours_overlunky STATIC
        detours/src/creatwth.cpp
        detours/src/detours.cpp
        detours/src/detours.h
        detours/src/detver.h
        detours/src/disasm.cpp
        detours/src/disolarm.cpp
        detours/src/disolarm64.cpp
        detours/src/disolia64.cpp
        detours/src/disolx64.cpp
        detours/src/disolx86.cpp
        detours/src/image.cpp
        detours/src/modules.cpp
        detours/src/uimports.cpp)

# This file is included and not compiled on its own
set_property(
        SOURCE detours/src/uimports.cpp
        APPEND PROPERTY HEADER_FILE_ONLY true)

target_compile_options(lib_detours_overlunky PRIVATE /W4 /WX /Zi /MT /Gy /Gm- /Zl /Od)
target_include_directories(lib_detours_overlunky PUBLIC detours/src)

# --------------------------------------------------
# overlunky spel2_api
add_subdirectory(shared)
add_subdirectory(game_api)

if (BUILD_INFO_DUMP)
        # --------------------------------------------------
        # json
        set(JSON_BuildTests OFF CACHE INTERNAL "")
        add_subdirectory(json)

        add_subdirectory(info_dump)
endif()

if(BUILD_SPEL2_DLL)
        add_subdirectory(spel2_dll)
endif()

if(BUILD_OVERLUNKY)
        # --------------------------------------------------
        # nyquist
        option(BUILD_EXAMPLE "Build example application" OFF)
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
