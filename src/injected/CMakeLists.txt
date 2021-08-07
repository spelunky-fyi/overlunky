add_library(injected SHARED
        ui.cpp ui.hpp
        decode_audio_file.cpp decode_audio_file.hpp
        main.cpp)
target_link_libraries(injected PRIVATE
        shared
        spel2_api)
target_link_libraries(injected PUBLIC
        imgui
        toml11::toml11
        libnyquist
        Shlwapi)
target_include_directories(injected PRIVATE
        ${LUA_INCLUDE_DIRS})
target_link_libraries(injected PRIVATE overlunky_warnings)
