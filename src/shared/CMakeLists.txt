add_library(shared INTERFACE)
target_include_directories(shared INTERFACE .)
target_link_libraries(shared INTERFACE
        fmt)

target_sources(shared INTERFACE logger.h)
