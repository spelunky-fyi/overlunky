#pragma once

#include <cstddef>
#include <cstdint>

enum class Spelunky_SoundFormat
{
    PCM_8,
    PCM_16,
    PCM_24,
    PCM_32,
    PCM_64,
    PCM_FLOAT,
    PCM_DOUBLE
};

struct Spelunky_DecodedAudioBuffer
{
    std::int32_t num_channels;
    std::int32_t frequency;
    Spelunky_SoundFormat format;
    const char* data;
    std::size_t data_size;
};

using Spelunky_DecodeAudioFile = Spelunky_DecodedAudioBuffer(const char* file_path);

using OnInputFunc = bool (*)(std::uint32_t, std::uint64_t, std::int64_t);
using ImguiInitFunc = void (*)(struct ImGuiContext*);
using ImguiDrawFunc = void (*)();
using PreDrawFunc = void (*)();
using PostDrawFunc = void (*)();

using Spelunky_MakeSavePathFunc = bool (*)(
    const char* script_path, size_t script_path_size, const char* script_name, size_t script_name_size, char* out_buffer, size_t out_buffer_size);

class SpelunkyScript;

void SetWriteLoadOptimization(bool write_load_opt);

void InitSwapChainHooks(struct IDXGISwapChain* swap_chain);
void InitSoundManager(Spelunky_DecodeAudioFile* decode_function);

void ShowCursor();
void HideCursor();

void RegisterOnInputFunc(OnInputFunc on_input);
void RegisterImguiInitFunc(ImguiInitFunc imgui_init);
void RegisterImguiDrawFunc(ImguiDrawFunc imgui_draw);
void RegisterPreDrawFunc(PreDrawFunc pre_draw);
void RegisterPostDrawFunc(PostDrawFunc post_draw);

void RegisterMakeSavePathFunc(Spelunky_MakeSavePathFunc make_save_path);

SpelunkyScript* CreateScript(const char* file_path, bool enabled);
void FreeScript(SpelunkyScript* script);

void SpelunkyScipt_ReloadScript(SpelunkyScript* script, const char* file_path);

bool SpelunkyScipt_IsEnabled(SpelunkyScript* script);
void SpelunkyScipt_SetEnabled(SpelunkyScript* script, bool enabled);

void SpelunkyScript_Update(SpelunkyScript* script);
void SpelunkyScript_Draw(SpelunkyScript* script, struct ImDrawList* draw_list);
void SpelunkyScript_DrawOptions(SpelunkyScript* script);
const char* SpelunkyScript_GetResult(SpelunkyScript* script);

struct SpelunkyScriptMessage
{
    const char* Message{nullptr};
    std::size_t TimeMilliSecond{0};
};
std::size_t SpelunkyScript_GetNumMessages(SpelunkyScript* script);
SpelunkyScriptMessage SpelunkyScript_GetMessage(SpelunkyScript* script, std::size_t message_idx);

struct SpelunkyScriptMeta
{
    const char* file;
    const char* name;
    const char* version;
    const char* description;
    const char* author;
    const char* id;
    const char* path;
    const char* filename;
    bool unsafe;
};
SpelunkyScriptMeta SpelunkyScript_GetMeta(SpelunkyScript* script);

enum class SpelunkyScreen
{
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
    Spaceship = 16,
    Win = 16,
    Credits = 17,
    Scores = 18,
    Constellation = 19,
    Recap = 20,
    Online = 29
};
SpelunkyScreen SpelunkyState_GetScreen();
