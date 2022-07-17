
if(DEFINED IWYU AND CMAKE_GENERATOR STREQUAL "Ninja")
    message("Configuring include-what-you-use...")

    if(NOT DEFINED IWYU_MAPPING_FILE)
        message(FATAL_ERROR "Please specify a mapping file for IWYU via the variable IWYU_MAPPING_FILE...")
    endif()

    set(IWYU_HELPER_FILE "${CMAKE_SOURCE_DIR}/iwyu_helper.py")
    set(IWYU_TOOL_FILE "${CMAKE_BINARY_DIR}/iwyu_tool.py")
    set(FIX_INCLUDES_FILE "${CMAKE_BINARY_DIR}/fix_includes.py")
    set(COMPILE_COMMANDS_FILE "${CMAKE_BINARY_DIR}/compile_commands.json")

    if(NOT EXISTS ${IWYU_TOOL_FILE} OR NOT EXISTS ${FIX_INCLUDES_FILE})
        message(STATUS "Downloading iwyu tools github.com/include-what-you-use/include-what-you-use...")

        if(NOT EXISTS ${IWYU_TOOL_FILE})
            set(IWYU_TOOL_URL "https://raw.githubusercontent.com/include-what-you-use/include-what-you-use/master/iwyu_tool.py")
            file(DOWNLOAD "${IWYU_TOOL_URL}" ${IWYU_TOOL_FILE})
        endif()

        if(NOT EXISTS ${FIX_INCLUDES_FILE})
            set(FIX_INCLUDES_URL "https://raw.githubusercontent.com/include-what-you-use/include-what-you-use/master/fix_includes.py")
            file(DOWNLOAD "${FIX_INCLUDES_URL}" ${FIX_INCLUDES_FILE})
        endif()
    endif()

    find_package(Python)

    message("Adding target apply_iwyu_fixes...")
    set(IWYU_COMMAND
        ${Python_EXECUTABLE}
        ${IWYU_HELPER_FILE}
        -i="${IWYU}"
        -t="${IWYU_TOOL_FILE}"
        -c="${COMPILE_COMMANDS_FILE}"
        -f="${FIX_INCLUDES_FILE}"
        -e=--no-warnings
        -e=-Xiwyu
        -e=--mapping_file="${IWYU_MAPPING_FILE}")

    if(DEFINED POST_IWYU_FORMATTING_TARGET)
        add_custom_target(
            apply_iwyu_fixes
            COMMAND ${IWYU_COMMAND}
            COMMAND cmake --build . --target ${POST_IWYU_FORMATTING_TARGET}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            USES_TERMINAL
        )
        add_custom_target(
            apply_iwyu_fixes_changes
            COMMAND ${IWYU_COMMAND} -s
            COMMAND cmake --build . --target ${POST_IWYU_FORMATTING_TARGET}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            USES_TERMINAL
        )
    else()
        add_custom_target(
            apply_iwyu_fixes
            ${IWYU_COMMAND}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            USES_TERMINAL
        )
        add_custom_target(
            apply_iwyu_fixes_changes
            ${IWYU_COMMAND} -s
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            USES_TERMINAL
        )
    endif()

    set(IWYU_PATH_AND_OPTIONS
        ${IWYU}
        --no-warnings
        -Xiwyu
        --mapping_file=${IWYU_MAPPING_FILE})

    function(setup_iwyu TARGET_NAME)
        message("Adding include-what-you-use to target ${TARGET_NAME}...")

        set_property(
            TARGET ${TARGET_NAME}
            PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH_AND_OPTIONS})
        set_property(
            TARGET ${TARGET_NAME}
            PROPERTY EXPORT_COMPILE_COMMANDS ON)
    endfunction()
else()
    function(setup_iwyu TARGET_NAME)
    endfunction()
endif()
