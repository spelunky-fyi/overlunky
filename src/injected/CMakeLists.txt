add_library(injected SHARED
        ui.cpp ui.hpp
        ui_util.cpp ui_util.hpp
        decode_audio_file.cpp decode_audio_file.hpp
        main.cpp)
target_link_libraries(injected PRIVATE
        shared
        spel2_api)
target_link_libraries_system(injected PUBLIC
        imgui
        toml11::toml11
        libnyquist
        Shlwapi)
target_include_directories(injected PRIVATE
        ${LUA_INCLUDE_DIRS})
target_link_libraries(injected PRIVATE overlunky_warnings)

if(MSVC)
        target_compile_options(injected PRIVATE /Zc:__cplusplus)
        target_compile_definitions(injected PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

target_compile_definitions(imgui PUBLIC IMGUI_USE_WCHAR32)

set_target_properties(injected PROPERTIES OUTPUT_NAME Overlunky)
