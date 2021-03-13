#include "spel2.h"

#include "window_api.hpp"

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
