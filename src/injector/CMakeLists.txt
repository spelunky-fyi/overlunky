add_executable(injector
    main.cpp
    cmd_line.cpp
    cmd_line.h
    injector.cpp
    injector.h)
target_link_libraries(injector
    shared)
add_dependencies(injector injected)

# --------------------------------------------------
# Find the Spel2.exe and set it for debugging in MSVC
if(MSVC)
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
        
        set_target_properties(injector PROPERTIES
            VS_DEBUGGER_COMMAND_ARGUMENTS "--launch_game \"${SPELUNKY_INSTALL_DIR}\"")
    endif()
endif()
