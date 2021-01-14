#include <d3d11.h>
#include <dxgi.h>
#include "rust/cxx.h"

void create_box(rust::Vec<rust::String> names, rust::Vec<uint16_t> ids);
void set_players(rust::Vec<uintptr_t> ids);
bool init_hooks(size_t ptr);
