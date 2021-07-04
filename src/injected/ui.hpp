#include <d3d11.h>
#include <dxgi.h>

#include <entity.hpp>
#include <vector>

const int OL_KEY_CTRL = 0x100;
const int OL_KEY_SHIFT = 0x200;
const int OL_BUTTON_MOUSE = 0x400;

void create_box(std::vector<EntityItem> items);
void init_ui();
void reload_enabled_scripts();
