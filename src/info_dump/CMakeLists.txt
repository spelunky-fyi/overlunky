add_library(info_dump SHARED
        main.cpp)
target_link_libraries(info_dump PRIVATE
        shared
        spel2_api
        overlunky_warnings)
target_link_libraries_system(info_dump PRIVATE
        nlohmann_json::nlohmann_json)

if(MSVC)
        target_compile_options(info_dump PRIVATE /bigobj)
endif()
