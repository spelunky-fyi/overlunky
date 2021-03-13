#pragma once

#include <cstdint>

using OnInputFunc = bool(*)(std::uint32_t, std::uint64_t, std::int64_t);
using ImguiInitFunc = void(*)(struct ImGuiContext*);
using ImguiDrawFunc = void(*)();
using PreDrawFunc = void(*)();
using PostDrawFunc = void(*)();

void InitSwapChainHooks(struct IDXGISwapChain* swap_chain);

void RegisterOnInputFunc(OnInputFunc on_input);
void RegisterImguiInitFunc(ImguiInitFunc imgui_init);
void RegisterImguiDrawFunc(ImguiDrawFunc imgui_draw);
void RegisterPreDrawFunc(PreDrawFunc pre_draw);
void RegisterPostDrawFunc(PostDrawFunc post_draw);
