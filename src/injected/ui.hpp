#include <d3d11.h>
#include <dxgi.h>

#include <vector>

const int OL_KEY_CTRL = 0x100;
const int OL_KEY_SHIFT = 0x200;
const int OL_BUTTON_MOUSE = 0x400;
const int OL_MOUSE_WHEEL = 0x10;
const int OL_WHEEL_DOWN = 0x11;
const int OL_WHEEL_UP = 0x12;

struct EntityItem;

void create_box(std::vector<EntityItem> items);
void init_ui();
void reload_enabled_scripts();
