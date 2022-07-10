
if(DEFINED IWYU)
    message("Configuring include-what-you-use...")
    set(IWYU_PATH_AND_OPTIONS
        ${IWYU}
        -Xiwyu
        --mapping_file=${CMAKE_SOURCE_DIR}/overlunky.3rdparty.headers.imp
        --no-warnings)

    function(setup_iwyu TARGET_NAME)
        message("Adding include-what-you-use to target ${TARGET_NAME}...")

        set_property(
            TARGET ${TARGET_NAME}
            PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH_AND_OPTIONS})
    endfunction()
else()
    function(setup_iwyu TARGET_NAME)
    endfunction()
endif()
