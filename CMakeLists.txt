cmake_minimum_required(VERSION 3.17)
project(overlunky)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)
endif()

option(BUILD_OVERLUNKY CACHE ON)
option(BUILD_SPEL2_DLL CACHE OFF)

execute_process(
  COMMAND git describe --always --dirty=-modified
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions("-DGIT_VERSION=${GIT_VERSION}")
add_definitions("-D_ITERATOR_DEBUG_LEVEL=0")
add_definitions("-DNOMINMAX")
add_definitions("-DWIN32_LEAN_AND_MEAN")

add_subdirectory(src)

if(MSVC)
  add_definitions(/bigobj)
  if(BUILD_OVERLUNKY)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT injector)
  endif()
endif()
