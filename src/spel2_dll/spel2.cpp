#include "spel2.h"

#include "window_api.hpp"
#include "script.hpp"

#include <imgui.h>

void InitSwapChainHooks(IDXGISwapChain* swap_chain)
{
	init_hooks(swap_chain);
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

std::string read_whole_file(const char* file_path)
{
	FILE* file{ nullptr };
	auto error = fopen_s(&file, file_path, "rb");
	if (error == 0 && file != nullptr) {
		struct CloseFileOnScopeExit {
			~CloseFileOnScopeExit() { fclose(File); }
			FILE* File;
		};
		auto close_file = CloseFileOnScopeExit{ file };

		fseek(file, 0, SEEK_END);
		const std::size_t file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		std::string code(file_size + 1, '\0');

		const auto size_read = fread(code.data(), 1, file_size, file);
		if (size_read != file_size) {
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
		return new SpelunkyScript(std::move(code), file_path, enabled);
	}
    return nullptr;
}
void FreeScript(SpelunkyScript* script)
{
    delete script;
}

void SpelunkyScipt_ReloadScript(SpelunkyScript* script, const char *file_path)
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
