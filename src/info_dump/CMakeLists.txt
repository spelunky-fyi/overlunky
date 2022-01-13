add_library(info_dump SHARED
        main.cpp)
target_link_libraries(info_dump PRIVATE
        shared
        spel2_api
        nlohmann_json::nlohmann_json
        overlunky_warnings
        sockpp-static)

if (MSVC)
        target_compile_options(info_dump PRIVATE /bigobj)
endif()
