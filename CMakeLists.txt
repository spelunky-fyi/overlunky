cmake_minimum_required(VERSION 3.17)
project(overlunky)

set(CMAKE_CXX_STANDARD 20)

include(cmake/clang-format.cmake)
setup_format_target(format_overlunky)

include(cmake/include-what-you-use.cmake)

include(cmake/link_sys_library.cmake)

string(COMPARE EQUAL "Clang" "${CMAKE_CXX_COMPILER_ID}" CLANG)

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../../bin)
endif()

option(BUILD_OVERLUNKY CACHE ON)
option(BUILD_INFO_DUMP CACHE ON)
option(BUILD_SPEL2_DLL CACHE OFF)

add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
add_compile_definitions(NOMINMAX)
add_compile_definitions(WIN32_LEAN_AND_MEAN)

if(BUILD_OVERLUNKY)
  add_compile_definitions(SPEL2_EDITABLE_SCRIPTS)
  add_compile_definitions(SPEL2_EXTRA_ANNOYING_SCRIPT_ERRORS)
endif()

add_subdirectory(src)

if(MSVC)
  add_definitions(/bigobj)

  if(BUILD_OVERLUNKY)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT injector)
  endif()
endif()
