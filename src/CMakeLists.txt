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
set(SOL2_LUA_VERSION "5.4.6")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/sol2/cmake/Modules;${CMAKE_CURRENT_SOURCE_DIR}/sol2/cmake/Packages;${CMAKE_CURRENT_SOURCE_DIR}/sol2/cmake/Includes")
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

if(MSVC)
        target_compile_options(lib_detours_overlunky PRIVATE /w /Zi /MT /Gy /Gm- /Zl /Od)
else()
        target_compile_options(lib_detours_overlunky PRIVATE -w)
endif()

target_include_directories(lib_detours_overlunky PUBLIC detours/src)

# --------------------------------------------------
# wrapper for version to avoid excessive recompiles
add_subdirectory(version)

# --------------------------------------------------
# warnings interface target
add_library(overlunky_warnings INTERFACE)

if(MSVC)
        target_compile_options(overlunky_warnings INTERFACE /W4 /WX)
        target_compile_options(overlunky_warnings INTERFACE /wd4201) # nonstandard extension used : nameless struct/union

        if(CLANG)
                target_compile_options(overlunky_warnings INTERFACE -Wshadow-all -Wno-missing-field-initializers -Wno-microsoft-cast -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-gnu-zero-variadic-macro-arguments -Wno-microsoft-enum-value -Wno-deprecated-declarations -Wno-language-extension-token -Wno-sign-compare)
                target_compile_options(overlunky_warnings INTERFACE -D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH)
        endif()
else()
        target_compile_options(overlunky_warnings INTERFACE -Wall -Wextra -pedantic -Werror)
        target_compile_options(overlunky_warnings INTERFACE -Wno-missing-field-initializers -Wno-microsoft-cast -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-gnu-zero-variadic-macro-arguments -Wno-microsoft-enum-value -Wno-deprecated-declarations -Wno-language-extension-token -Wno-sign-compare)
endif()

# --------------------------------------------------
# overlunky spel2_api
add_subdirectory(shared)
add_subdirectory(game_api)

setup_ol_target(spel2_api)

if(BUILD_INFO_DUMP)
        # --------------------------------------------------
        # json
        set(JSON_BuildTests OFF CACHE INTERNAL "")
        add_subdirectory(json)

        add_subdirectory(info_dump)
        setup_ol_target(info_dump)
endif()

if(BUILD_SPEL2_DLL)
        add_subdirectory(spel2_dll)
        setup_ol_target(spel2)
endif()

if(BUILD_OVERLUNKY)
        # --------------------------------------------------
        # nyquist
        option(BUILD_EXAMPLE "Build example application" OFF)
        add_subdirectory(libnyquist)

        # Remove the vectorization instructions since those cause a crash for some users
        function(remove_arch_avx target)
                get_target_property(target_compile_options ${target} COMPILE_OPTIONS)
                list(REMOVE_ITEM target_compile_options /arch:AVX)
                set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "${target_compile_options}")
        endfunction()

        remove_arch_avx(libwavpack)
        remove_arch_avx(libnyquist)

        if(WIN32)
                if(NOT MSVC)
                        # Remove other instructions that is added on Windows even if not MSVC
                        function(remove_zi target)
                                get_target_property(target_compile_options ${target} COMPILE_OPTIONS)
                                list(REMOVE_ITEM target_compile_options /Zi)
                                set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "${target_compile_options}")
                        endfunction()

                        remove_zi(libwavpack)
                        remove_zi(libnyquist)
                endif()
        endif()

        # Disable error on Clang16
        if(CLANG)
                target_compile_options(libwavpack PRIVATE "-Wno-implicit-function-declaration")
        endif()

        # --------------------------------------------------
        # toml11
        option(toml11_BUILD_TEST OFF)
        add_subdirectory(toml11)

        if(MSVC)
                # --------------------------------------------------
                # Find the Spel2.exe, if not passed to cmake and set it for debugging in MSVC
                if(NOT EXISTS ${SPELUNKY_INSTALL_DIR}/Spel2.exe)
                        get_filename_component(STEAM_INSTALL_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Valve\\Steam;InstallPath]" ABSOLUTE)
                        set(SPELUNKY_INSTALL_DIR "${STEAM_INSTALL_DIR}/SteamApps/common/Spelunky 2")

                        if(NOT EXISTS ${SPELUNKY_INSTALL_DIR}/Spel2.exe)
                                set(STEAM_LIBRARY_FOLDERS_CONFIG "${STEAM_INSTALL_DIR}/SteamApps/libraryfolders.vdf")

                                if(EXISTS ${STEAM_LIBRARY_FOLDERS_CONFIG})
                                        FILE(READ "${STEAM_LIBRARY_FOLDERS_CONFIG}" STEAM_LIBRARY_FOLDERS_CONFIG_CONTENT)
                                        STRING(REGEX MATCHALL "\"[A-Z]\:[^\"]*\"" POTENTIAL_STEAM_LIBRARY_FOLDERS ${STEAM_LIBRARY_FOLDERS_CONFIG_CONTENT})

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
        endif()

        # --------------------------------------------------
        # overlunky exe and dll
        add_subdirectory(injected)
        add_subdirectory(injector)

        setup_ol_target(injected)
        setup_ol_target(injector)

        if(MSVC)
                if(BUILD_INFO_DUMP AND EXISTS ${SPELUNKY_INSTALL_DIR})
                        find_package(Python3 COMPONENTS Interpreter)

                        if(Python3_FOUND)
                                add_custom_target(update_docs_overlunky
                                        COMMAND ${CMAKE_COMMAND} -E rm -Rf ${SPELUNKY_INSTALL_DIR}/game_data
                                        COMMAND injector --launch_game ${SPELUNKY_INSTALL_DIR} --info_dump
                                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPELUNKY_INSTALL_DIR}/game_data ./docs/game_data
                                        COMMAND cd docs
                                        COMMAND ${Python3_EXECUTABLE} generate.py
                                        COMMAND ${Python3_EXECUTABLE} generate_emmylua.py
                                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                                        COMMENT "Update docs, including collecting all game data.")
                                add_custom_target(update_auto_docs_overlunky
                                        COMMAND cd docs
                                        COMMAND ${Python3_EXECUTABLE} generate.py
                                        COMMAND ${Python3_EXECUTABLE} generate_emmylua.py
                                        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                                        COMMENT "Update only auto-docs.")
                        else()
                                message("You need python 3 for auto-docs.")
                        endif()
                endif()
        endif()
endif()

add_subdirectory(sockpp)
target_include_directories(spel2_api PUBLIC sockpp/include)
target_link_libraries_system(spel2_api PUBLIC sockpp-static)

# --------------------------------------------------
# disable all warnings on 3rd-party
if(MSVC)
        target_compile_options(imgui PRIVATE "/w")
        target_compile_options(lib_detours_overlunky PRIVATE "/w")
        target_compile_options(sockpp-objs PRIVATE "/w")
        target_compile_options(sockpp-static PRIVATE "/w")

        if(BUILD_OVERLUNKY)
                target_compile_options(libwavpack PRIVATE "/w")
                target_compile_options(libnyquist PRIVATE "/w")
        endif()
else()
        target_compile_options(imgui PRIVATE "--no-warnings")
        target_compile_options(lib_detours_overlunky PRIVATE "--no-warnings")
        target_compile_options(sockpp-objs PRIVATE "--no-warnings")
        target_compile_options(sockpp-static PRIVATE "--no-warnings")

        if(BUILD_OVERLUNKY)
                target_compile_options(libwavpack PRIVATE "--no-warnings")
                target_compile_options(libnyquist PRIVATE "--no-warnings")
        endif()
endif()
