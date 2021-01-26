add_executable(injector main.cpp injector.cpp injector.h)
target_link_libraries(injector
    shared)
add_dependencies(injector injected)
