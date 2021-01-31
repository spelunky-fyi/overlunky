add_library(shared INTERFACE)
target_include_directories(shared INTERFACE .)

file(GLOB_RECURSE shared_sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE shared_headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.inl")
target_sources(shared INTERFACE ${shared_sources} ${shared_headers})
