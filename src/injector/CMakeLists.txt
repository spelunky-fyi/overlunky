add_executable(injector cxx/main.cpp cxx/injector.cpp cxx/injector.h)
add_dependencies(injector injected)