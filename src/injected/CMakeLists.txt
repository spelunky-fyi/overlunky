file(GLOB_RECURSE injected_sources CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE injected_headers CONFIGURE_DEPENDS "*.h" "*.hpp" "*.inl")
add_library(injected SHARED
        ${injected_sources}
        ${injected_headers})
target_link_libraries(injected
        shared
        imgui
        toml11::toml11
        Shlwapi)
