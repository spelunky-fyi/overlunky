#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

bool init_hooks(void* swap_chain_ptr);

using OnInputCallback = bool(*)(UINT, WPARAM, LPARAM);
using ImguiInitCallback = void(*)();
using ImguiDrawCallback = void(*)();
using PreDrawCallback = void(*)();
using PostDrawCallback = void(*)();

enum WndProcResult : LRESULT {
	RunImguiWindowProc = 1 << 0,
	RunGameWindowProc = 1 << 1,
	RunAllWindowProc = RunImguiWindowProc | RunGameWindowProc,
};
void register_on_input(OnInputCallback on_input);
void register_imgui_init(ImguiInitCallback imgui_init);
void register_imgui_draw(ImguiDrawCallback imgui_draw);
void register_pre_draw(PreDrawCallback pre_draw);
void register_post_draw(PostDrawCallback post_draw);

HWND get_window();
