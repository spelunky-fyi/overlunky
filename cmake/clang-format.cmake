function(setup_format_target FORMAT_TARGET_NAME)
    set(CLANG_FORMAT_UTIL_FILE "${CMAKE_BINARY_DIR}/git-clang-format.py")
    if(NOT EXISTS ${CLANG_FORMAT_UTIL_FILE})
        message(STATUS "Downloading formatting utility from github.com/TheLartians/Format.cmake")

        # Using this one instead of the one in the llvm repo because it allows specifying trees in order to format all files
        set(FORMAT_SOURCE_URL "https://raw.githubusercontent.com/TheLartians/Format.cmake/v1.7.1/git-clang-format.py")

        file(DOWNLOAD "${FORMAT_SOURCE_URL}" ${CLANG_FORMAT_UTIL_FILE})
    endif()

    find_program(CLANG_FORMAT_PROGRAM clang-format)
    find_program(GIT_PROGRAM git)
    find_package(Python)

    set(CLANG_FORMAT_COMMAND ${Python_EXECUTABLE} ${CLANG_FORMAT_UTIL_FILE}
        --binary=${CLANG_FORMAT_PROGRAM}
    )
    set(GIT_EMPTY_TREE_HASH 4b825dc642cb6eb9a060e54bf8d69288fbee4904)

    add_custom_target(
        ${FORMAT_TARGET_NAME}
        COMMAND ${CLANG_FORMAT_COMMAND} ${GIT_EMPTY_TREE_HASH} -f
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    add_custom_target(
        ${FORMAT_TARGET_NAME}_changes
        COMMAND ${CLANG_FORMAT_COMMAND} -f
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endfunction()