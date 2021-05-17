add_library(spel2_api STATIC
        entity.cpp
        entity.hpp
        items.cpp
        items.hpp
        layer.cpp
        layer.hpp
        level_api.cpp
        level_api.hpp
        render_api.cpp
        render_api.hpp
        rpc.cpp
        rpc.hpp
        state.cpp
        state.hpp
        texture.cpp
        texture.hpp
        particles.cpp
        particles.hpp
        search.hpp
        memory.hpp
        thread_utils.cpp
        thread_utils.hpp
        movable.hpp
        window_api.cpp
        window_api.hpp
        script.cpp
        script.hpp
        script_context.cpp
        script_context.hpp
        sound_manager.cpp
        sound_manager.hpp
        fmod.hpp
        audio_buffer.hpp
        game_manager.cpp
        game_manager.hpp
        overloaded.hpp)
target_include_directories(spel2_api PUBLIC .)
target_link_libraries(spel2_api PUBLIC
        imgui)
target_link_libraries(spel2_api PRIVATE
        shared
        sol2::sol2
        ${LUA_LIBRARIES})

if (MSVC)
        target_compile_options(spel2_api PRIVATE /bigobj)
endif()
