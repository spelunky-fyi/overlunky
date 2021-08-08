file(GLOB_RECURSE spel2_sources CONFIGURE_DEPENDS *.cpp)
file(GLOB_RECURSE spel2_headers CONFIGURE_DEPENDS *.h *.hpp *.inl)

add_library(spel2_api STATIC
        ${spel2_sources}
        ${spel2_headers})
target_include_directories(spel2_api PUBLIC .)
target_link_libraries(spel2_api PUBLIC
        imgui)
target_link_libraries(spel2_api PRIVATE
        shared
        sol2::sol2
        ${LUA_LIBRARIES}
        lib_detours_overlunky)
target_compile_definitions(spel2_api PRIVATE
        SOL_ALL_SAFETIES_ON=1
        SOL_PRINT_ERRORS=0)
target_precompile_headers(spel2_api PRIVATE
        <sol/sol.hpp>
        <d3d11.h>)
target_link_libraries(spel2_api PRIVATE overlunky_warnings)
if (MSVC)
        target_compile_options(spel2_api PRIVATE /bigobj)
endif()
