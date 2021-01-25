cmake_minimum_required(VERSION 3.17)
project(injected)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)

add_subdirectory(src/injector)
add_subdirectory(src/injected)
