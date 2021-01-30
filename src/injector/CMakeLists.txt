file(GLOB_RECURSE injector_sources CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE injector_headers CONFIGURE_DEPENDS "*.h" "*.hpp" "*.inl")
add_executable(injector
    ${injector_sources}
    ${injector_headers})
target_link_libraries(injector
        shared)
add_dependencies(injector injected)
