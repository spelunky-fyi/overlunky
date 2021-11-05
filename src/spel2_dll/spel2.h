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

using Spelunky_DecodeAudioFile = Spelunky_DecodedAudioBuffer (*)(const char* file_path);

using OnInputFunc = bool (*)(std::uint32_t, std::uint64_t, std::int64_t);
using ImguiInitFunc = void (*)(struct ImGuiContext*);
using ImguiDrawFunc = void (*)();
using PreDrawFunc = void (*)();
using PostDrawFunc = void (*)();

using Spelunky_MakeSavePathFunc = bool (*)(
    const char* script_path, size_t script_path_size, const char* script_name, size_t script_name_size, char* out_buffer, size_t out_buffer_size);

using SpelunkyAllocFun = void* (*)(size_t);
struct SpelunkyFileInfo
{
    void* Data{nullptr};
    int _member_1{0};
    int DataSize{0};
    int AllocationSize{0};
    int _member_4{0};
};
using Spelunky_LoadFileFunc = SpelunkyFileInfo* (*)(const char* file_path, SpelunkyAllocFun alloc_fun);
using Spelunky_GetImageFilePathFunc = bool (*)(const char* root_path, const char* relative_path, char* out_buffer, size_t out_buffer_size);

class SpelunkyScript;
class SpelunkyConsole;

void Spelunky_SetWriteLoadOptimization(bool write_load_opt);

void Spelunky_RegisterApplicationVersion(const char* version);
void Spelunky_InitMemoryDatabase();
void Spelunky_InitSwapChainHooks(struct IDXGISwapChain* swap_chain);
void Spelunky_InitSoundManager(Spelunky_DecodeAudioFile decode_function);

void Spelunky_ShowCursor();
void Spelunky_HideCursor();

void Spelunky_RegisterOnInputFunc(OnInputFunc on_input);
void Spelunky_RegisterImguiInitFunc(ImguiInitFunc imgui_init);
void Spelunky_RegisterImguiDrawFunc(ImguiDrawFunc imgui_draw);
void Spelunky_RegisterPreDrawFunc(PreDrawFunc pre_draw);
void Spelunky_RegisterPostDrawFunc(PostDrawFunc post_draw);

void Spelunky_RegisterMakeSavePathFunc(Spelunky_MakeSavePathFunc make_save_path);

void Spelunky_RegisterOnLoadFileFunc(Spelunky_LoadFileFunc on_load_file);
void Spelunky_RegisterGetImagePathFunc(Spelunky_GetImageFilePathFunc get_image_file_path);

SpelunkyScript* Spelunky_CreateScript(const char* file_path, bool enabled);
void Spelunky_FreeScript(SpelunkyScript* script);

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

SpelunkyConsole* CreateConsole();
void FreeConsole(SpelunkyConsole* console);

void SpelunkyConsole_Update(SpelunkyConsole* console);
void SpelunkyConsole_Draw(SpelunkyConsole* console, struct ImDrawList* draw_list);
void SpelunkyConsole_DrawOptions(SpelunkyConsole* console);
bool SpelunkyConsole_IsToggled(SpelunkyConsole* console);
void SpelunkyConsole_Toggle(SpelunkyConsole* console);
bool SpelunkyConsole_Execute(SpelunkyConsole* console, const char* code, char* out_buffer, size_t out_buffer_size);

std::size_t SpelunkyConsole_GetNumMessages(SpelunkyConsole* console);
const char* SpelunkyConsole_GetMessage(SpelunkyConsole* console, std::size_t message_idx);
void SpelunkyConsole_ConsumeMessages(SpelunkyConsole* console);

bool SpelunkyConsole_HasNewHistory(SpelunkyConsole* console);
void SpelunkyConsole_SetMaxHistorySize(SpelunkyConsole* console, size_t max_history);
void SpelunkyConsole_SaveHistory(SpelunkyConsole* console, const char* path);
void SpelunkyConsole_LoadHistory(SpelunkyConsole* console, const char* path);

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

std::int32_t Spelunky_SpawnEntity(std::uint32_t entity_id, std::int32_t layer, float x, float y, float vel_x, float vel_y);

const char16_t* Spelunky_GetCharacterFullName(std::uint32_t character_index);
const char16_t* Spelunky_GetCharacterShortName(std::uint32_t character_index);
void Spelunky_GetCharacterHeartColor(std::uint32_t character_index, float (&color)[4]);
bool Spelunky_GetCharacterGender(std::uint32_t character_index);

void Spelunky_SetCharacterFullName(std::uint32_t character_index, const char16_t* name);
void Spelunky_SetCharacterShortName(std::uint32_t character_index, const char16_t* name);
void Spelunky_SetCharacterHeartColor(std::uint32_t character_index, float (&color)[4]);
void Spelunky_SetCharacterGender(std::uint32_t character_index, bool female);
