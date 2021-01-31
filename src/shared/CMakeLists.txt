add_library(shared INTERFACE)
target_include_directories(shared INTERFACE .)

target_sources(shared INTERFACE logger.h)
