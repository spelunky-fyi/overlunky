add_library(overlunky_version STATIC
        version.hpp version.cpp)
target_include_directories(overlunky_version PUBLIC .)

execute_process(
        COMMAND git describe --always --dirty=-modified
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE OVERLUNKY_GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(overlunky_version PRIVATE
        GIT_VERSION=${OVERLUNKY_GIT_VERSION}
)
