#pragma once

#include <cstdint>
#include <minwindef.h> // for UINT, WPARAM, LPARAM

bool detect_wine();

bool init_hooks(void* swap_chain_ptr);

using OnInputCallback = bool (*)(UINT, WPARAM, LPARAM);
using ImguiInitCallback = void (*)(struct ImGuiContext*);
using ImguiDrawCallback = void (*)();
using PreDrawCallback = void (*)();
using PostDrawCallback = void (*)();
using OnQuitCallback = void (*)();

enum WndProcResult : LRESULT
{
    RunImguiWindowProc = 1 << 0,
    RunGameWindowProc = 1 << 1,
    RunAllWindowProc = RunImguiWindowProc | RunGameWindowProc,
};
void register_on_input(OnInputCallback on_input);
void register_imgui_pre_init(ImguiInitCallback imgui_init);
void register_imgui_init(ImguiInitCallback imgui_init);
void register_imgui_draw(ImguiDrawCallback imgui_draw);
void register_pre_draw(PreDrawCallback pre_draw);
void register_post_draw(PostDrawCallback post_draw);
void register_on_quit(OnQuitCallback on_quit);

HWND get_window();

void show_cursor();
void hide_cursor();
void imgui_vsync(bool enable);

struct ID3D11Device* get_device();
