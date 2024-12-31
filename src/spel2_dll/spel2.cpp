#include "spel2.h"

#include "character_def.hpp"
#include "console.hpp"
#include "entity.hpp"
#include "file_api.hpp"
#include "memory.hpp"
#include "render_api.hpp"
#include "screen.hpp"
#include "script.hpp"
#include "sound_manager.hpp"
#include "spawn_api.hpp"
#include "state.hpp"
#include "steam_api.hpp"
#include "texture.hpp"
#include "window_api.hpp"

#include <cstring>

SoundManager* g_SoundManager{nullptr};
SpelunkyConsole* g_Console{nullptr};

void Spelunky_SetDoHooks(bool do_hooks)
{
    API::set_do_hooks(do_hooks);
}
void Spelunky_SetWriteLoadOptimization(bool write_load_opt)
{
    API::set_write_load_opt(write_load_opt);
}
void Spelunky_InitState()
{
    API::init();
}
void Spelunky_PostInitState()
{
    API::post_init();
}

void Spelunky_RegisterApplicationVersion(const char* version)
{
    register_application_version(version);
}
void Spelunky_InitMemoryDatabase()
{
    preload_addresses();
}
void Spelunky_InitSwapChainHooks(IDXGISwapChain* swap_chain)
{
    init_hooks(swap_chain);
}
void Spelunky_InitSoundManager(Spelunky_DecodeAudioFile decode_function)
{
    static Spelunky_DecodeAudioFile local_decode_function = decode_function;
    g_SoundManager = new SoundManager(
        [](const char* file_path)
        {
            Spelunky_DecodedAudioBuffer buffer = local_decode_function(file_path);
            return DecodedAudioBuffer{
                buffer.num_channels,
                buffer.frequency,
                static_cast<SoundFormat>(buffer.format),
                std::unique_ptr<const std::byte[]>{reinterpret_cast<const std::byte*>(buffer.data)},
                buffer.data_size,
            };
        });
}
void Spelunky_DestroySoundManager()
{
    if (g_SoundManager != nullptr)
    {
        delete g_SoundManager;
        g_SoundManager = nullptr;
    }
}

void Spelunky_ShowCursor()
{
    show_cursor();
}
void Spelunky_HideCursor()
{
    hide_cursor();
}

void Spelunky_RegisterOnInputFunc(OnInputFunc on_input)
{
    register_on_input(on_input);
}
void Spelunky_RegisterImguiInitFunc(ImguiInitFunc imgui_init)
{
    register_imgui_init(imgui_init);
}
void Spelunky_RegisterImguiDrawFunc(ImguiDrawFunc imgui_draw)
{
    register_imgui_draw(imgui_draw);
}
void Spelunky_RegisterPreDrawFunc(PreDrawFunc pre_draw)
{
    register_pre_draw(pre_draw);
}
void Spelunky_RegisterPostDrawFunc(PostDrawFunc post_draw)
{
    register_post_draw(post_draw);
}
void Spelunky_RegisterOnQuitFunc(OnQuitFunc on_quit)
{
    register_on_quit(on_quit);
}

void Spelunky_RegisterMakeSavePathFunc(Spelunky_MakeSavePathFunc make_save_path)
{
    static Spelunky_MakeSavePathFunc local_make_save_path_func;
    local_make_save_path_func = make_save_path;
    register_make_save_path(
        [](std::string_view script_path, std::string_view script_name) -> std::string
        {
            char out_buffer[MAX_PATH];
            if (local_make_save_path_func(script_path.data(), script_path.size(), script_name.data(), script_name.size(), out_buffer, MAX_PATH))
            {
                return out_buffer;
            }
            return "";
        });
}

void Spelunky_RegisterOnLoadFileFunc(Spelunky_LoadFileFunc on_load_file)
{
    register_on_load_file((LoadFileCallback*)on_load_file);
}
void Spelunky_RegisterOnReadFromFileFunc(Spelunky_ReadFromFileFunc on_read_from_file)
{
    register_on_read_from_file(on_read_from_file);
}
void Spelunky_RegisterOnWriteToFileFunc(Spelunky_WriteToFileFunc on_write_to_file)
{
    register_on_write_to_file(on_write_to_file);
}
void Spelunky_RegisterGetImagePathFunc(Spelunky_GetImageFilePathFunc get_image_file_path)
{
    static Spelunky_GetImageFilePathFunc local_get_image_file_path;
    local_get_image_file_path = get_image_file_path;
    register_get_image_file_path(
        [](std::string root_path, std::string relative_path) -> std::string
        {
            char out_file_path[MAX_PATH];
            if (local_get_image_file_path(root_path.c_str(), relative_path.c_str(), out_file_path, MAX_PATH))
            {
                return out_file_path;
            }
            return "";
        });
}

void Spelunky_DisableSteamAchievements()
{
    disable_steam_achievements();
}

ID3D11Device* SpelunkyGetD3D11Device()
{
    return get_device();
}

std::string read_whole_file(const char* file_path)
{
    FILE* file{nullptr};
    auto error = fopen_s(&file, file_path, "rb");
    if (error == 0 && file != nullptr)
    {
        struct CloseFileOnScopeExit
        {
            ~CloseFileOnScopeExit()
            {
                fclose(File);
            }
            FILE* File;
        };
        auto close_file = CloseFileOnScopeExit{file};

        fseek(file, 0, SEEK_END);
        const size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        std::string code(file_size, '\0');

        const auto size_read = fread(code.data(), 1, file_size, file);
        if (size_read != file_size)
        {
            code.clear();
            return code;
        }

        return code;
    }
    return {};
}

SpelunkyScript* Spelunky_CreateScript(const char* file_path, bool enabled)
{
    std::string code = read_whole_file(file_path);
    if (!code.empty())
    {
        return new SpelunkyScript(std::move(code), file_path, g_SoundManager, g_Console, enabled);
    }
    return nullptr;
}
void Spelunky_FreeScript(SpelunkyScript* script)
{
    delete script;
}

void SpelunkyScript_ReloadScript(SpelunkyScript* script, const char* file_path)
{
    std::string code = read_whole_file(file_path);
    if (!code.empty())
    {
        script->update_code(std::move(code));
    }
}

bool SpelunkyScript_IsEnabled(SpelunkyScript* script)
{
    return script->is_enabled();
}
void SpelunkyScript_SetEnabled(SpelunkyScript* script, bool enabled)
{
    script->set_enabled(enabled);
}

void SpelunkyScript_Draw(SpelunkyScript* script, struct ImDrawList* draw_list)
{
    script->draw(draw_list);
}
void SpelunkyScript_DrawOptions(SpelunkyScript* script)
{
    script->render_options();
}
void SpelunkyScript_GetResult(SpelunkyScript* script, char* out_buffer, size_t out_buffer_size)
{
    const std::string result = script->get_result();
    strncpy_s(out_buffer, out_buffer_size, result.c_str(), result.size());
}
void SpelunkyScript_LoopMessages(SpelunkyScript* script, SpelunkyScript_MessageFun message_fun)
{
    script->loop_messages(
        [=](const ScriptMessage& message)
        {
            message_fun(SpelunkyScriptMessage{
                message.message.c_str(),
                static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(message.time.time_since_epoch()).count()),
            });
        });
}
void SpelunkyScript_GetMeta(SpelunkyScript* script, SpelunkyScript_MetaFun meta_fun)
{
    script->get_meta(
        [=](const ScriptMeta& meta)
        {
            meta_fun(SpelunkyScriptMeta{
                meta.file.c_str(),
                meta.name.c_str(),
                meta.version.c_str(),
                meta.description.c_str(),
                meta.author.c_str(),
                meta.id.c_str(),
                meta.path.c_str(),
                meta.filename.c_str(),
                meta.unsafe,
                meta.online_safe,
            });
        });
}

SpelunkyConsole* CreateConsole()
{
    if (g_Console == nullptr)
    {
        g_Console = new SpelunkyConsole(g_SoundManager);
    }
    return g_Console;
}
void FreeConsole(SpelunkyConsole* console)
{
    if (g_Console == console)
    {
        g_Console = nullptr;
    }
    delete console;
}

void SpelunkyConsole_Draw(SpelunkyConsole* console, struct ImDrawList* draw_list)
{
    console->draw(draw_list);
}
void SpelunkyConsole_DrawOptions(SpelunkyConsole* console)
{
    console->render_options();
}
bool SpelunkyConsole_IsToggled(SpelunkyConsole* console)
{
    return console->is_toggled();
}
void SpelunkyConsole_Toggle(SpelunkyConsole* console)
{
    console->toggle();
}
bool SpelunkyConsole_Execute(SpelunkyConsole* console, const char* code, char* out_buffer, size_t out_buffer_size)
{
    std::string result = console->execute(code);
    if (result.empty())
    {
        return false;
    }

    auto num_written = snprintf(
        out_buffer,
        out_buffer_size - 1,
        "%s",
        result.c_str());
    out_buffer[num_written] = '\0';
    return static_cast<size_t>(num_written) < out_buffer_size;
}
void SpelunkyConsole_ConsumeMessages(SpelunkyConsole* console, SpelunkyConsole_MessageFun message_fun)
{
    const auto& messages = console->consume_messages();
    for (const auto& message : messages)
    {
        message_fun(message.message.c_str());
    }
}

bool SpelunkyConsole_HasNewHistory(SpelunkyConsole* console)
{
    return console->has_new_history();
}
void SpelunkyConsole_SetMaxHistorySize(SpelunkyConsole* console, size_t max_history)
{
    console->set_max_history_size(max_history);
}
void SpelunkyConsole_SaveHistory(SpelunkyConsole* console, const char* path)
{
    console->save_history(path);
}
void SpelunkyConsole_LoadHistory(SpelunkyConsole* console, const char* path)
{
    console->load_history(path);
}

StateMemory& get_state()
{
    static StateMemory* state = State::get().ptr();
    return *state;
}

SpelunkyScreen SpelunkyState_GetScreen()
{
    return static_cast<SpelunkyScreen>(get_state().screen);
}

int32_t Spelunky_SpawnEntity(uint32_t entity_id, int32_t layer, float x, float y, float vel_x, float vel_y)
{
    return spawn_entity_abs(entity_id, x, y, (LAYER)layer, vel_x, vel_y);
}

const char16_t* Spelunky_GetCharacterFullName(uint32_t character_index)
{
    return NCharacterDB::get_character_full_name(character_index);
}
const char16_t* Spelunky_GetCharacterShortName(uint32_t character_index)
{
    return NCharacterDB::get_character_short_name(character_index);
}
void Spelunky_GetCharacterHeartColor(uint32_t character_index, float (&color)[4])
{
    NCharacterDB::get_character_heart_color(character_index).to_float(color);
}
bool Spelunky_GetCharacterGender(uint32_t character_index)
{
    return NCharacterDB::get_character_gender(character_index);
}

void Spelunky_SetCharacterFullName(uint32_t character_index, const char16_t* name)
{
    NCharacterDB::set_character_full_name(character_index, name);
}
void Spelunky_SetCharacterShortName(uint32_t character_index, const char16_t* name)
{
    NCharacterDB::set_character_short_name(character_index, name);
}
void Spelunky_SetCharacterHeartColor(uint32_t character_index, float (&color)[4])
{
    NCharacterDB::set_character_heart_color(character_index, Color{color});
}
void Spelunky_SetCharacterGender(uint32_t character_index, bool female)
{
    NCharacterDB::set_character_gender(character_index, female);
}

int64_t Spelunky_DefineTexture(Spelunky_TextureDefinition texture_defintion)
{
    TextureDefinition real_texture_defintion{
        texture_defintion.texture_path,
        texture_defintion.width,
        texture_defintion.height,
        texture_defintion.tile_width,
        texture_defintion.tile_height,
        texture_defintion.sub_image_offset_x,
        texture_defintion.sub_image_offset_y,
        texture_defintion.sub_image_width,
        texture_defintion.sub_image_height,
    };
    return define_texture(real_texture_defintion);
}
void Spelunky_ReloadTexture(const char* texture_path)
{
    reload_texture(texture_path);
}

int64_t SpelunkyEntity_GetTexture(Entity* entity)
{
    return entity->texture->id;
}
void SpelunkyEntity_SetTexture(Entity* entity, int64_t texture)
{
    entity->set_texture(static_cast<TEXTURE>(texture));
}
uint16_t SpelunkyEntity_GetTextureTile(Entity* entity)
{
    return entity->animation_frame;
}
void SpelunkyEntity_SetTextureTile(Entity* entity, uint16_t texture_tile)
{
    entity->animation_frame = texture_tile;
}

void Spelunky_SetPostRenderGame(Spelunky_PostRenderGameFunc post_render_game)
{
    RenderAPI::get().set_post_render_game(post_render_game);
}
Spelunky_TextSize Spelunky_DrawTextSize(const char* text, float scale_x, float scale_y, uint32_t fontstyle)
{
    const auto [w, h] = RenderAPI::get().draw_text_size(text, scale_x, scale_y, fontstyle);
    return {w, h};
}
void Spelunky_DrawText(const char* text, float x, float y, float scale_x, float scale_y, const float (&color)[4], uint32_t alignment, uint32_t fontstyle)
{
    TextRenderingInfo tri{};
    tri.set_text(text, x, y, scale_x, scale_y, alignment, fontstyle);
    RenderAPI::get().draw_text(&tri, Color{color});
}

void Spelunky_EnabledAdvancedHud()
{
    RenderAPI::get().set_advanced_hud();
}

void Spelunky_ReloadShaders()
{
    RenderAPI::get().reload_shaders();
}
