add_library(game_api STATIC
        entity.cpp
        entity.hpp
        items.cpp
        items.hpp
        layer.cpp
        layer.hpp
        render_api.cpp
        render_api.hpp
        rpc.cpp
        rpc.hpp
        state.cpp
        state.hpp
        search.hpp
        memory.hpp
        thread_utils.cpp
        thread_utils.hpp)
target_include_directories(game_api PUBLIC .)
target_link_libraries(game_api shared)