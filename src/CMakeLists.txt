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
        imgui/imgui_demo.cpp
        imgui/misc/cpp/imgui_stdlib.cpp)
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

target_compile_options(lib_detours_overlunky PRIVATE /w /Zi /MT /Gy /Gm- /Zl /Od)
target_include_directories(lib_detours_overlunky PUBLIC detours/src)

# --------------------------------------------------
# warnings interface target
add_library(overlunky_warnings INTERFACE)
if(MSVC)
        target_compile_options(overlunky_warnings INTERFACE /W4 /WX)
        target_compile_options(overlunky_warnings INTERFACE /wd4201) #nonstandard extension used : nameless struct/union
        if(CLANG)
                target_compile_options(overlunky_warnings INTERFACE -Wno-missing-field-initializers -Wno-microsoft-cast)
        endif()
else()
        target_compile_options(overlunky_warnings INTERFACE -Wall -Wextra -Wshadow-all -pedantic -Werror)
        target_compile_options(overlunky_warnings INTERFACE -Wno-missing-field-initializers -Wno-microsoft-cast)
endif()

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

        if(MSVC)
                # --------------------------------------------------
                # Find the Spel2.exe and set it for debugging in MSVC
                get_filename_component(STEAM_INSTALL_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Valve\\Steam;InstallPath]" ABSOLUTE)
                set(SPELUNKY_INSTALL_DIR "${STEAM_INSTALL_DIR}/SteamApps/common/Spelunky 2")
                if(NOT EXISTS ${SPELUNKY_INSTALL_DIR}/Spel2.exe)
                        set(STEAM_LIBRARY_FOLDERS_CONFIG "${STEAM_INSTALL_DIR}/SteamApps/libraryfolders.vdf")
                        if(EXISTS ${STEAM_LIBRARY_FOLDERS_CONFIG})
                                FILE(READ "${STEAM_LIBRARY_FOLDERS_CONFIG}" STEAM_LIBRARY_FOLDERS_CONFIG_CONTENT)
                                STRING(REGEX MATCHALL "\"[A-Z]\:.*\"" POTENTIAL_STEAM_LIBRARY_FOLDERS ${STEAM_LIBRARY_FOLDERS_CONFIG_CONTENT})
                                foreach(STEAM_LIBRARY_FOLDER ${POTENTIAL_STEAM_LIBRARY_FOLDERS})
                                        string(REGEX REPLACE "\^\"" "" STEAM_LIBRARY_FOLDER ${STEAM_LIBRARY_FOLDER})
                                        string(REGEX REPLACE "\"$" "" STEAM_LIBRARY_FOLDER ${STEAM_LIBRARY_FOLDER})
                                        string(REGEX REPLACE "\\\\\\\\" "/" STEAM_LIBRARY_FOLDER ${STEAM_LIBRARY_FOLDER}) # double slash, escaped for cmake string then escaped for regex, requires a total of 8 backslashes
                                        if(EXISTS ${STEAM_LIBRARY_FOLDER})
                                                set(SPELUNKY_INSTALL_DIR "${STEAM_LIBRARY_FOLDER}/SteamApps/common/Spelunky 2")
                                                if(EXISTS ${SPELUNKY_INSTALL_DIR}/Spel2.exe)
                                                        break()
                                                endif()
                                        endif()
                                endforeach()
                        endif()
                endif()

                if(NOT EXISTS ${SPELUNKY_INSTALL_DIR})
                        message(STATUS "Could not find Spelunky 2 installation")
                else()
                        message(STATUS "Found Spelunky 2 installation at '${SPELUNKY_INSTALL_DIR}'")
                endif()
        endif()

        # --------------------------------------------------
        # overlunky exe and dll
        add_subdirectory(injected)
        add_subdirectory(injector)
        
        if(MSVC)
                if(BUILD_INFO_DUMP AND EXISTS ${SPELUNKY_INSTALL_DIR})
                        find_package(Python3 COMPONENTS Interpreter)
                        if(Python3_FOUND)
                                add_custom_target(update_docs_overlunky
                                        COMMAND injector --launch_game ${SPELUNKY_INSTALL_DIR} --info_dump
                                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPELUNKY_INSTALL_DIR}/game_data ./docs/game_data
                                        COMMAND cd docs && ${Python3_EXECUTABLE} generate.py
                                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                                        COMMENT "Update docs, including collecting all game data.")
                                add_custom_target(update_auto_docs_overlunky
                                        COMMAND cd docs && ${Python3_EXECUTABLE} generate.py
                                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                                        COMMENT "Update only auto-docs.")
                        else()
                                message("You need python 3 for auto-docs.")
                        endif()
                endif()
        endif()
endif()
