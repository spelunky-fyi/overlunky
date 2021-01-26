file(GLOB_RECURSE injected_sources CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE injected_headers CONFIGURE_DEPENDS "*.h" "*.h" "*.inl")
add_library(injected SHARED
        ${injected_sources}
        ${injected_headers})
target_link_libraries(injected
        shared
        imgui
        toml11::toml11
        Shlwapi)

execute_process(
        COMMAND git describe --always --dirty=-modified
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(injected PRIVATE
        "GIT_VERSION=${GIT_VERSION}")