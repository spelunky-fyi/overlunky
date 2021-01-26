
set(imgui_sources
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
    imgui/imgui_demo.cpp)
add_library(imgui STATIC ${imgui_sources})
target_include_directories(imgui PUBLIC imgui)

option(toml11_BUILD_TEST OFF)
add_subdirectory(toml11)

add_subdirectory(shared)
add_subdirectory(injected)
add_subdirectory(injector)
