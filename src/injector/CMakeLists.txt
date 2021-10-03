add_executable(injector
    main.cpp
    cmd_line.cpp
    cmd_line.h
    injector.cpp
    injector.h)
target_link_libraries(injector PRIVATE
    shared
    overlunky_warnings)
add_dependencies(injector injected)

if(MSVC AND EXISTS ${SPELUNKY_INSTALL_DIR})
    set_target_properties(injector PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS "--launch_game \"${SPELUNKY_INSTALL_DIR}\"")
endif()
