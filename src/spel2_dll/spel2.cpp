#include "spel2.h"

#include "script.hpp"
#include "script_context.hpp"
#include "sound_manager.hpp"
#include "state.hpp"
#include "window_api.hpp"

SoundManager* g_SoundManager{nullptr};

void SetWriteLoadOptimization(bool write_load_opt)
{
    State::set_write_load_opt(write_load_opt);
}

void InitSwapChainHooks(IDXGISwapChain* swap_chain)
{
    init_hooks(swap_chain);
}
void InitSoundManager(Spelunky_DecodeAudioFile* decode_function)
{
    static Spelunky_DecodeAudioFile* local_decode_function = decode_function;
    g_SoundManager = new SoundManager(
        [](const char* file_path)
        {
            Spelunky_DecodedAudioBuffer buffer = local_decode_function(file_path);
            return DecodedAudioBuffer{buffer.num_channels, buffer.frequency, static_cast<SoundFormat>(buffer.format), std::unique_ptr<const std::byte[]>{reinterpret_cast<const std::byte*>(buffer.data)}, buffer.data_size};
        });
}

void ShowCursor()
{
    show_cursor();
}
void HideCursor()
{
    hide_cursor();
}

void RegisterOnInputFunc(OnInputFunc on_input)
{
    register_on_input(on_input);
}
void RegisterImguiInitFunc(ImguiInitFunc imgui_init)
{
    register_imgui_init(imgui_init);
}
void RegisterImguiDrawFunc(ImguiDrawFunc imgui_draw)
{
    register_imgui_draw(imgui_draw);
}
void RegisterPreDrawFunc(PreDrawFunc pre_draw)
{
    register_pre_draw(pre_draw);
}
void RegisterPostDrawFunc(PostDrawFunc post_draw)
{
    register_post_draw(post_draw);
}

void RegisterMakeSavePathFunc(Spelunky_MakeSavePathFunc make_save_path)
{
    static Spelunky_MakeSavePathFunc local_make_save_path_func = make_save_path;
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
        const std::size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        std::string code(file_size + 1, '\0');

        const auto size_read = fread(code.data(), 1, file_size, file);
        if (size_read != file_size)
        {
            return nullptr;
        }

        return code;
    }
    return {};
}

SpelunkyScript* CreateScript(const char* file_path, bool enabled)
{
    std::string code = read_whole_file(file_path);
    if (!code.empty())
    {
        return new SpelunkyScript(std::move(code), file_path, g_SoundManager, enabled);
    }
    return nullptr;
}
void FreeScript(SpelunkyScript* script)
{
    delete script;
}

void SpelunkyScipt_ReloadScript(SpelunkyScript* script, const char* file_path)
{
    std::string code = read_whole_file(file_path);
    if (!code.empty())
    {
        script->update_code(std::move(code));
    }
}

bool SpelunkyScipt_IsEnabled(SpelunkyScript* script)
{
    return script->is_enabled();
}
void SpelunkyScipt_SetEnabled(SpelunkyScript* script, bool enabled)
{
    script->set_enabled(enabled);
}

void SpelunkyScript_Update(SpelunkyScript* script)
{
    script->run();
}
void SpelunkyScript_Draw(SpelunkyScript* script, struct ImDrawList* draw_list)
{
    script->draw(draw_list);
}
void SpelunkyScript_DrawOptions(SpelunkyScript* script)
{
    script->render_options();
}
const char* SpelunkyScript_GetResult(SpelunkyScript* script)
{
    return script->get_result().c_str();
}
std::size_t SpelunkyScript_GetNumMessages(SpelunkyScript* script)
{
    return script->get_messages().size();
}
SpelunkyScriptMessage SpelunkyScript_GetMessage(SpelunkyScript* script, std::size_t message_idx)
{
    const auto& messages = script->get_messages();
    if (message_idx < messages.size())
    {
        const auto& message = messages[message_idx];
        return SpelunkyScriptMessage{message.message.c_str(), static_cast<std::size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(message.time.time_since_epoch()).count())};
    }
    return SpelunkyScriptMessage{};
}
SpelunkyScriptMeta SpelunkyScript_GetMeta(SpelunkyScript* script)
{
    return {script->get_file().c_str(), script->get_name().c_str(), script->get_version().c_str(), script->get_description().c_str(), script->get_author().c_str(), script->get_id().c_str(), script->get_path().c_str(), script->get_filename().c_str(), script->get_unsafe()};
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
