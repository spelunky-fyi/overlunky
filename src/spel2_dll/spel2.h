#pragma once

#include <stddef.h>
#include <stdint.h>

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
    int32_t num_channels;
    int32_t frequency;
    Spelunky_SoundFormat format;
    const char* data;
    size_t data_size;
};

using Spelunky_DecodeAudioFile = Spelunky_DecodedAudioBuffer (*)(const char* file_path);

using OnInputFunc = bool (*)(uint32_t, uint64_t, int64_t);
using ImguiInitFunc = void (*)(struct ImGuiContext*);
using ImguiDrawFunc = void (*)();
using PreDrawFunc = void (*)();
using PostDrawFunc = void (*)();
using OnQuitFunc = void (*)();

using Spelunky_PostRenderGameFunc = void (*)();

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
using Spelunky_ReadFromFileOriginal = void (*)(const char* file, void** out_data, size_t* out_data_size);
using Spelunky_ReadFromFileFunc = void (*)(const char* file, void** out_data, size_t* out_data_size, SpelunkyAllocFun alloc_fun, Spelunky_ReadFromFileOriginal original);
using Spelunky_WriteToFileOriginal = void (*)(const char* backup_file, const char* file, void* data, size_t data_size);
using Spelunky_WriteToFileFunc = void (*)(const char* backup_file, const char* file, void* data, size_t data_size, Spelunky_WriteToFileOriginal original);
using Spelunky_GetImageFilePathFunc = bool (*)(const char* root_path, const char* relative_path, char* out_buffer, size_t out_buffer_size);

class SpelunkyScript;
class SpelunkyConsole;

void Spelunky_SetDoHooks(bool do_hooks);
void Spelunky_SetWriteLoadOptimization(bool write_load_opt);
void Spelunky_InitState();
void Spelunky_PostInitState();

void Spelunky_RegisterApplicationVersion(const char* version);
void Spelunky_InitMemoryDatabase();
void Spelunky_InitSwapChainHooks(struct IDXGISwapChain* swap_chain);
void Spelunky_InitSoundManager(Spelunky_DecodeAudioFile decode_function);
void Spelunky_DestroySoundManager();

void Spelunky_ShowCursor();
void Spelunky_HideCursor();

void Spelunky_RegisterOnInputFunc(OnInputFunc on_input);
void Spelunky_RegisterImguiInitFunc(ImguiInitFunc imgui_init);
void Spelunky_RegisterImguiDrawFunc(ImguiDrawFunc imgui_draw);
void Spelunky_RegisterPreDrawFunc(PreDrawFunc pre_draw);
void Spelunky_RegisterPostDrawFunc(PostDrawFunc post_draw);
void Spelunky_RegisterOnQuitFunc(OnQuitFunc on_quit);

void Spelunky_RegisterMakeSavePathFunc(Spelunky_MakeSavePathFunc make_save_path);

// Hook loading a file into memory, used when reading packed assets
void Spelunky_RegisterOnLoadFileFunc(Spelunky_LoadFileFunc on_load_file);
// Hook read/write raw files from/to disk, e.g. save game or config files
void Spelunky_RegisterOnReadFromFileFunc(Spelunky_ReadFromFileFunc on_read_from_file);
void Spelunky_RegisterOnWriteToFileFunc(Spelunky_WriteToFileFunc on_write_to_file);
// Converts a image path to a real path, use for example when reading files from places other than the game folder
void Spelunky_RegisterGetImagePathFunc(Spelunky_GetImageFilePathFunc get_image_file_path);

void Spelunky_DisableSteamAchievements();

struct ID3D11Device* SpelunkyGetD3D11Device();

SpelunkyScript* Spelunky_CreateScript(const char* file_path, bool enabled);
void Spelunky_FreeScript(SpelunkyScript* script);

void SpelunkyScript_ReloadScript(SpelunkyScript* script, const char* file_path);

bool SpelunkyScript_IsEnabled(SpelunkyScript* script);
void SpelunkyScript_SetEnabled(SpelunkyScript* script, bool enabled);

void SpelunkyScript_Draw(SpelunkyScript* script, struct ImDrawList* draw_list);
void SpelunkyScript_DrawOptions(SpelunkyScript* script);
void SpelunkyScript_GetResult(SpelunkyScript* script, char* out_buffer, size_t out_buffer_size);

struct SpelunkyScriptMessage
{
    const char* Message{nullptr};
    size_t TimeMilliSecond{0};
};
using SpelunkyScript_MessageFun = void (*)(SpelunkyScriptMessage);
void SpelunkyScript_LoopMessages(SpelunkyScript* script, SpelunkyScript_MessageFun message_fun);

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
    bool online_safe;
};
using SpelunkyScript_MetaFun = void (*)(const SpelunkyScriptMeta&);
void SpelunkyScript_GetMeta(SpelunkyScript* script, SpelunkyScript_MetaFun meta_fun);

SpelunkyConsole* CreateConsole();
void FreeConsole(SpelunkyConsole* console);

void SpelunkyConsole_Draw(SpelunkyConsole* console, struct ImDrawList* draw_list);
void SpelunkyConsole_DrawOptions(SpelunkyConsole* console);
bool SpelunkyConsole_IsToggled(SpelunkyConsole* console);
void SpelunkyConsole_Toggle(SpelunkyConsole* console);
bool SpelunkyConsole_Execute(SpelunkyConsole* console, const char* code, char* out_buffer, size_t out_buffer_size);

using SpelunkyConsole_MessageFun = void (*)(const char*);
void SpelunkyConsole_ConsumeMessages(SpelunkyConsole* console, SpelunkyConsole_MessageFun message_fun);

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

int32_t Spelunky_SpawnEntity(uint32_t entity_id, int32_t layer, float x, float y, float vel_x, float vel_y);

const char16_t* Spelunky_GetCharacterFullName(uint32_t character_index);
const char16_t* Spelunky_GetCharacterShortName(uint32_t character_index);
void Spelunky_GetCharacterHeartColor(uint32_t character_index, float (&color)[4]);
bool Spelunky_GetCharacterGender(uint32_t character_index);

void Spelunky_SetCharacterFullName(uint32_t character_index, const char16_t* name);
void Spelunky_SetCharacterShortName(uint32_t character_index, const char16_t* name);
void Spelunky_SetCharacterHeartColor(uint32_t character_index, float (&color)[4]);
void Spelunky_SetCharacterGender(uint32_t character_index, bool female);

struct Spelunky_TextureDefinition
{
    const char* texture_path;
    uint32_t width;
    uint32_t height;
    uint32_t tile_width;
    uint32_t tile_height;
    uint32_t sub_image_offset_x{0};
    uint32_t sub_image_offset_y{0};
    uint32_t sub_image_width{0};
    uint32_t sub_image_height{0};
};
int64_t Spelunky_DefineTexture(Spelunky_TextureDefinition texture_defintion);
void Spelunky_ReloadTexture(const char* texture_path);

class Entity;
int64_t SpelunkyEntity_GetTexture(Entity* entity);
void SpelunkyEntity_SetTexture(Entity* entity, int64_t texture);
uint16_t SpelunkyEntity_GetTextureTile(Entity* entity);
void SpelunkyEntity_SetTextureTile(Entity* entity, uint16_t texture_tile);

struct Spelunky_TextSize
{
    float width;
    float height;
};
void Spelunky_SetPostRenderGame(Spelunky_PostRenderGameFunc post_render_game);
Spelunky_TextSize Spelunky_DrawTextSize(const char* text, float scale_x, float scale_y, uint32_t fontstyle);
void Spelunky_DrawText(const char* text, float x, float y, float scale_x, float scale_y, const float (&color)[4], uint32_t alignment, uint32_t fontstyle);

void Spelunky_EnabledAdvancedHud();

void Spelunky_ReloadShaders();
