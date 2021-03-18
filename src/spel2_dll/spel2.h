#pragma once

#include <cstddef>
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

void SpelunkyScipt_ReloadScript(SpelunkyScript* script, const char *file_path);

bool SpelunkyScipt_IsEnabled(SpelunkyScript* script);
void SpelunkyScipt_SetEnabled(SpelunkyScript* script, bool enabled);

void SpelunkyScript_Update(SpelunkyScript* script);
void SpelunkyScript_Draw(SpelunkyScript* script, struct ImDrawList *draw_list);
void SpelunkyScript_DrawOptions(SpelunkyScript* script);
const char* SpelunkyScript_GetResult(SpelunkyScript* script);

struct SpelunkyScriptMessage {
	const char* Message{ nullptr };
	std::size_t TimeMilliSecond{ 0 };
};
std::size_t SpelunkyScript_GetNumMessages(SpelunkyScript* script);
SpelunkyScriptMessage SpelunkyScript_GetMessage(SpelunkyScript* script, std::size_t message_idx);

enum class SpelunkyScreen {
	Logo = 0,
	Intro = 1,
	Prologue = 2,
	Title = 3,
	Menu = 4,
	Options = 5,
	Leaderboard = 7,
	SeedInput = 8,
	CharacterSelect = 9,
	TeamSelect = 10,
	Camp = 11,
	Level = 12,
	Transition = 13,
	Death = 14,
	Sapceship = 16,
	Win = 16,
	Credits = 17,
	Scores = 18,
	Constellation = 19,
	Recap = 20
};
SpelunkyScreen SpelunkyState_GetScreen();
