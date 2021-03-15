#pragma once

#include <cstdint>

using OnInputFunc = bool(*)(std::uint32_t, std::uint64_t, std::int64_t);
using ImguiInitFunc = void(*)(struct ImGuiContext*);
using ImguiDrawFunc = void(*)();
using PreDrawFunc = void(*)();
using PostDrawFunc = void(*)();

class SpelunkyScript;

void InitSwapChainHooks(struct IDXGISwapChain* swap_chain);

void RegisterOnInputFunc(OnInputFunc on_input);
void RegisterImguiInitFunc(ImguiInitFunc imgui_init);
void RegisterImguiDrawFunc(ImguiDrawFunc imgui_draw);
void RegisterPreDrawFunc(PreDrawFunc pre_draw);
void RegisterPostDrawFunc(PostDrawFunc post_draw);

SpelunkyScript* CreateScript(const char *file_path, bool enabled);
void FreeScript(SpelunkyScript* script);

bool SpelunkyScipt_IsEnabled(SpelunkyScript* script);
void SpelunkyScipt_SetEnabled(SpelunkyScript* script, bool enabled);

void SpelunkyScript_Update(SpelunkyScript* script);
void SpelunkyScript_Draw(SpelunkyScript* script, struct ImDrawList *draw_list);
void SpelunkyScript_DrawOptions(SpelunkyScript* script);
const char* SpelunkyScript_GetResult(SpelunkyScript* script);
