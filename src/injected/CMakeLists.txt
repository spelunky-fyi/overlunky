include_directories(.)
include_directories(imgui)
include_directories(toml11)
include_directories(../injector/cxx)

add_subdirectory(toml11)

add_library(injected SHARED
        entity.cpp
        entity.hpp
        memory.h
        api.cpp
        api.h
        search.h
        ui.cpp
        ui.hpp
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_internal.h
        imgui/backends/imgui_impl_dx11.cpp
        imgui/backends/imgui_impl_dx11.h
        imgui/backends/imgui_impl_win32.cpp
        imgui/backends/imgui_impl_win32.h
        imgui/imgui_widgets.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_demo.cpp
        toml11/toml.hpp
        ../injector/cxx/logger.h
        rpc.cpp
        rpc.hpp
        thread_utils.h
        thread_utils.cpp
        main.cpp
        state.cpp
        state.hpp layer.cpp layer.h items.cpp items.h api.h)

target_link_libraries(injected toml11 Shlwapi)
