
option(IWYU "Find include-what-you-use and setup targets for applying fixes." OFF)

if(IWYU AND
    (CMAKE_GENERATOR STREQUAL "Ninja" OR CMAKE_GENERATOR STREQUAL "Unix Makefiles"))
    message(STATUS "Configuring include-what-you-use...")

    if(NOT DEFINED IWYU_BINARY_PATH)
        if(NOT WIN32)
            message(STATUS "IWYU_BINARY_PATH not specified, using installed binary...")
            find_program(IWYU_BINARY_PATH NAMES include-what-you-use iwyu REQUIRED)
        else()
            # We have to fetch our own binaries because IWYU is not officially supported on Windows
            set(IWYU_BINARY_PATH "${CMAKE_BINARY_DIR}/include-what-you-use.exe")

            if(NOT EXISTS ${IWYU_BINARY_PATH})
                message(STATUS "IWYU_BINARY_PATH not specified, downloading include-what-you-use binary...")

                set(IWYU_EXE_URL "https://github.com/Malacath-92/include-what-you-use/releases/download/${CMAKE_CXX_COMPILER_VERSION}/include-what-you-use.exe")

                message("Downloading binary from \"${IWYU_EXE_URL}\"...")
                file(DOWNLOAD "${IWYU_EXE_URL}" ${IWYU_BINARY_PATH} SHOW_PROGRESS STATUS IWYU_BINARY_DOWNLOAD_STATUS)
                list(GET IWYU_BINARY_DOWNLOAD_STATUS 0 IWYU_BINARY_DOWNLOAD_STATUS_CODE)

                if(NOT IWYU_BINARY_DOWNLOAD_STATUS_CODE EQUAL 0)
                    list(GET IWYU_BINARY_DOWNLOAD_STATUS 1 IWYU_BINARY_DOWNLOAD_STATUS_ERROR)
                    message(FATAL_ERROR "Failed downloading include-what-you-use.exe for compiler version ${CMAKE_CXX_COMPILER_VERSION}: \"${IWYU_BINARY_DOWNLOAD_STATUS_ERROR}\". Either open an issue, make a PR or manually specify binary path in IWYU_BINARY_PATH...")
                endif()
            else()
                message(STATUS "IWYU_BINARY_PATH not specified, using cached binary...")
            endif()
        endif()
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

    message(STATUS "Adding target apply_iwyu_fixes...")
    set(IWYU_COMMAND
        ${Python_EXECUTABLE}
        ${IWYU_HELPER_FILE}
        -i="${IWYU_BINARY_PATH}"
        -t="${IWYU_TOOL_FILE}"
        -c="${COMPILE_COMMANDS_FILE}"
        -f="${FIX_INCLUDES_FILE}"
        -e=--no-warnings)

    if(DEFINED IWYU_MAPPING_FILE)
        list(APPEND IWYU_COMMAND -e=-Xiwyu -e=--mapping_file="${IWYU_MAPPING_FILE}")
    endif()

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
        ${IWYU_BINARY_PATH}
        --no-warnings)

    if(DEFINED IWYU_MAPPING_FILE)
        list(APPEND IWYU_PATH_AND_OPTIONS -Xiwyu --mapping_file=${IWYU_MAPPING_FILE})
    endif()

    function(setup_iwyu TARGET_NAME)
        message(STATUS "Adding include-what-you-use to target ${TARGET_NAME}...")

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
