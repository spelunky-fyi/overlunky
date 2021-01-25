#include <d3d11.h>
#include <dxgi.h>

struct Inventory {
    uint32_t money;
    uint8_t bombs;
    uint8_t ropes;
    uint8_t b06;
    uint8_t b07;
    uint8_t pad08[0x141c]; // specific treasure and killed monsters here, boring
    uint32_t kills_level;
    uint32_t kills_total;
};

void set_players(std::vector<uintptr_t> ids);
void create_box(std::vector<EntityItem> items);
bool init_hooks(size_t _ptr);
