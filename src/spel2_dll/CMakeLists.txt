add_library(spel2 SHARED
        spel2.h
        spel2.cpp)
target_include_directories(spel2 PUBLIC .)
target_link_libraries(spel2 PRIVATE
        spel2_api)
set_target_properties(spel2 PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON)
