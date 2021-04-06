#include <cstdarg>
#include "../injected/ui.hpp"
#include "entity.hpp"
#include "state.hpp"
#include "logger.h"

uint32_t setflag(uint32_t flags, int bit) { return flags | (1U << (bit - 1)); }
uint32_t clrflag(uint32_t flags, int bit) { return flags & ~(1U << (bit - 1)); }
bool testflag(uint32_t flags, int bit) { return (flags & (1U << (bit - 1))) > 0; }
uint32_t flipflag(uint32_t flags, int bit)
{
    if (testflag(flags, bit))
        return clrflag(flags, bit);
    else
        return setflag(flags, bit);
}

int32_t spawn_entity(uint32_t id, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    auto [_x, _y] = player->position();
    if (!s)
    {
        DEBUG("Spawning {} on {}, {}", id, x + _x, y + _y);
        return state.layer(player->layer())->spawn_entity(id, x + _x, y + _y, s, vx, vy, snap)->uid;
    }
    else
    {
        DEBUG("Spawning {} on screen {}, {}", id, x, y);
        return state.layer(player->layer())->spawn_entity(id, x, y, s, vx, vy, snap)->uid;
    }
}

int32_t spawn_entity_abs(uint32_t id, float x, float y, int layer, float vx, float vy)
{
    auto state = State::get();
    if (layer == 0 || layer == 1)
    {
        return state.layer(layer)->spawn_entity(id, x, y, false, vx, vy, false)->uid;
    }
    else if (layer < 0)
    {
        auto player = state.items()->player(abs(layer)-1);
        if (player == nullptr)
            return -1;
        auto [_x, _y] = player->position();
        DEBUG("Spawning {} on {}, {}", id, x + _x, y + _y);
        return state.layer(player->layer())->spawn_entity(id, x + _x, y + _y, false, vx, vy, false)->uid;
    }
    return -1;
}

int32_t spawn_door(float x, float y, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    auto [_x, _y] = player->position();
    DEBUG("Spawning door on {}, {}", x + _x, y + _y);
    state.layer(player->layer())->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    return state.layer(player->layer())->spawn_door(x + _x, y + _y, w, l, t)->uid;
}

int32_t spawn_door_abs(float x, float y, int layer, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();
    if (layer == 0 || layer == 1)
    {
        return state.layer(layer)->spawn_door(x, y, w, l, t)->uid;
    }
    else if (layer < 0)
    {
        auto player = state.items()->player(abs(layer)-1);
        if (player == nullptr)
            return -1;
        auto [_x, _y] = player->position();
        DEBUG("Spawning door on {}, {}", x + _x, y + _y);
        return state.layer(player->layer())->spawn_door(x + _x, y + _y, w, l, t)->uid;
    }
    return -1;
}

void spawn_backdoor(float x, float y)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto [_x, _y] = player->position();
    DEBUG("Spawning backdoor on {}, {}", x + _x, y + _y);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x + _x, y + _y - 1.0, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_BG_DOOR_BACK_LAYER"), x + _x, y + _y, false, 0.0, 0.0, true);
}

void spawn_backdoor_abs(float x, float y)
{
    auto state = State::get();
    DEBUG("Spawning backdoor on {}, {}", x, y);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_FLOOR_DOOR_LAYER"), x, y, false, 0.0, 0.0, true);
    state.layer(0)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0, false, 0.0, 0.0, true);
    state.layer(1)->spawn_entity(to_id("ENT_TYPE_LOGICAL_PLATFORM_SPAWNER"), x, y - 1.0, false, 0.0, 0.0, true);
}

void teleport(float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();

    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    DEBUG("Teleporting to relative {}, {}, {}", x, y, s);
    player->teleport(x, y, s, vx, vy, snap);
}

void godmode(bool g)
{
    State::get().godmode(g);
}

void darkmode(bool g)
{
    State::get().darkmode(g);
}

void zoom(float level)
{
    State::get().zoom(level);
}

float get_zoom_level()
{
    return State::get().get_zoom_level();
}

void list_items()
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    for (auto &item : state.layer(player->layer())->items())
    {
        DEBUG("Item {} {:x}, {}", item->uid, item->type->search_flags, item->position_self());
    }
}

int32_t get_entity_at(float x, float y, bool s, float r, uint32_t mask)
{
    auto state = State::get();
    if (s)
    {
        auto [rx, ry] = state.click_position(x, y);
        x = rx;
        y = ry;
    }
    DEBUG("Items at {}:", (x, y));
    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    std::vector<std::tuple<int, float, Entity *>> found;
    for (auto &item : state.layer(player->layer())->items())
    {
        auto [ix, iy] = item->position();
        auto flags = item->type->search_flags;
        float distance = sqrt(pow(x - ix, 2) + pow(y - iy, 2));
        if (((mask & flags) > 0 || mask == 0) && distance < r)
        {
            DEBUG(
                "Item {}, {:x} type, {} position, {} distance, {:x}",
                item->uid,
                item->type->search_flags,
                item->position_self(),
                distance,
                item->pointer());
            found.push_back({item->uid, distance, item});
        }
    }
    if (!found.empty())
    {
        std::sort(found.begin(), found.end(), [](auto a, auto b) -> bool { return std::get<1>(a) < std::get<1>(b); });
        auto picked = found[0];
        auto entity = std::get<2>(picked);
        DEBUG("{}", (void *)entity);
        return std::get<0>(picked);
    }
    return -1;
}

void move_entity(uint32_t id, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->teleport(x, y, s, vx, vy, snap);
}

void move_entity_abs(uint32_t id, float x, float y, float vx, float vy)
{
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->teleport_abs(x, y, vx, vy);
}

uint32_t get_entity_flags(uint32_t id)
{
    if (id == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(id);
    if(ent)
        return ent->flags;
    return 0;
}

void set_entity_flags(uint32_t id, uint32_t flags)
{
    if (id == 0)
        return;
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->flags = flags;
}

uint32_t get_entity_flags2(uint32_t id)
{
    if (id == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        return ent->more_flags;
    return 0;
}

void set_entity_flags2(uint32_t id, uint32_t flags)
{
    if (id == 0)
        return;
    auto state = State::get();
    auto ent = state.find(id);
    if (ent)
        ent->more_flags = flags;
}

int get_entity_ai_state(uint32_t id)
{
    if (id == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(id)->as<Movable>();
    if (ent)
        return ent->move_state;
    return 0;
}

uint32_t get_hud_flags()
{
    auto state = State::get();
    return state.flags();
}

void set_hud_flags(uint32_t flags)
{
    auto state = State::get();
    state.set_flags(flags);
}

void set_pause(uint8_t pause)
{
    auto state = State::get();
    state.set_pause(pause);
}

void player_status()
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    auto status = player->inventory_ptr;
    DEBUG("Player {}", (void *)status, status->ropes, status->bombs);
    status->ropes = (99);
    status->bombs = (99);
}

Entity *get_entity_ptr(uint32_t id)
{
    auto state = State::get();
    auto p = state.find(id);
    if (IsBadWritePtr(p, 0x178))
        return nullptr;
    return p;
}

int32_t get_entity_type(uint32_t id)
{
    auto state = State::get();
    auto p = state.find(id);
    if (p == nullptr || IsBadWritePtr(p, 0x178))
        return -1;
    return p->type->id;
}

StateMemory *get_state_ptr()
{
    auto state = State::get();
    return state.ptr();
}

std::vector<Player *> get_players()
{
    auto state = State::get();
    std::vector<Player *> found;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state.items()->player(i);
        if (player)
            found.push_back((Player *)player);
    }
    return found;
}

std::pair<float, float> click_position(float x, float y)
{
    return State::get().click_position(x, y);
}

std::pair<float, float> screen_position(float x, float y)
{
    return State::get().screen_position(x, y);
}

float screen_distance(float x)
{
    auto a = State::get().screen_position(0, 0);
    auto b = State::get().screen_position(x, 0);
    return b.first - a.first;
}

std::vector<uint32_t> get_entities()
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        found.push_back(item->uid);
    }
    for (auto &item : state.layer(1)->items())
    {
        found.push_back(item->uid);
    }
    return found;
}

std::vector<uint32_t> get_entities_by_layer(int layer)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    if (layer == -1)
        layer = player->layer();
    std::vector<uint32_t> found;
    for (auto &item : state.layer(layer)->items())
    {
        found.push_back(item->uid);
    }
    return found;
}

std::vector<uint32_t> get_entities_by_type(std::vector<uint32_t> types)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

template<typename... Args>
std::vector<uint32_t> get_entities_by_type(Args... args)
{
    std::vector<uint32_t> types = {args...};
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

/*std::vector<uint32_t> get_entities_by_type(uint32_t type)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (item->type->id == type)
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (item->type->id == type)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}*/

std::vector<uint32_t> get_entities_by_mask(uint32_t mask)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto &item : state.layer(0)->items())
    {
        if (item->type->search_flags & mask)
        {
            found.push_back(item->uid);
        }
    }
    for (auto &item : state.layer(1)->items())
    {
        if (item->type->search_flags & mask)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_by(uint32_t type, uint32_t mask, int layer)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    std::vector<uint32_t> found;
    if (!player)
        return {};
    if (layer == -1)
        layer = player->layer();
    if (layer >= 0 && state.layer(layer))
    {
        for (auto &item : state.layer(layer)->items())
        {
            if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == type || type == 0))
            {
                found.push_back(item->uid);
            }
        }
    }
    else
    {
        if (state.layer(0))
        {
            for (auto &item : state.layer(0)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == type || type == 0))
                {
                    found.push_back(item->uid);
                }
            }
        }
        if (state.layer(1))
        {
            for (auto &item : state.layer(1)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == type || type == 0))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_at(uint32_t type, uint32_t mask, float x, float y, int layer, float r)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    for (auto &item : state.layer(layer)->items())
    {
        auto [ix, iy] = item->position();
        float distance = sqrt(pow(x - ix, 2) + pow(y - iy, 2));
        if (((item->type->search_flags & mask) > 0 || mask == 0) && (item->type->id == type || type == 0) && distance < r)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

void set_door_target(uint32_t id, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();
    Entity *door = get_entity_ptr(id);
    if (door == nullptr)
        return;
    static_cast<Door *>(door)->set_target(w, l, t);
}

std::tuple<uint8_t, uint8_t, uint8_t> get_door_target(uint32_t id)
{
    auto state = State::get();
    Entity *door = get_entity_ptr(id);
    if (door == nullptr)
        return std::make_tuple(0, 0, 0);
    return static_cast<Door *>(door)->get_target();
}

void set_contents(uint32_t id, uint32_t item)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    Entity *container = get_entity_ptr(id);
    if (container == nullptr)
        return;
    int type = container->type->id;
    if (type != to_id("ENT_TYPE_ITEM_COFFIN") && type != to_id("ENT_TYPE_ITEM_CRATE") && type != to_id("ENT_TYPE_ITEM_PRESENT") && type != to_id("ENT_TYPE_ITEM_GHIST_PRESENT") && type != to_id("ENT_TYPE_ITEM_POT"))
        return;
    container->as<Container>()->inside = item;
}

void entity_remove_item(uint32_t id, uint32_t item)
{
    Entity *entity = get_entity_ptr(id);
    if (entity == nullptr)
        return;
    entity->remove_item(item);
}

int32_t spawn_entity_over(uint32_t id, uint32_t over, float x, float y)
{
    auto state = State::get();
    Entity *overlay = get_entity_ptr(over);
    if (overlay == nullptr)
        return -1;
    int layer = overlay->layer();
    return state.layer(layer)->spawn_entity_over(id, overlay, x, y)->uid;
}

bool entity_has_item_uid(uint32_t id, uint32_t item)
{
    Entity *entity = get_entity_ptr(id);
    if (entity == nullptr)
        return false;
    if (entity->items.count > 0)
    {
        int *pitems = (int *)entity->items.begin;
        for (int i = 0; i < entity->items.count; i++)
        {
            if (pitems[i] == item)
                return true;
        }
    }
    return false;
};

bool entity_has_item_type(uint32_t id, uint32_t type)
{
    Entity *entity = get_entity_ptr(id);
    if (entity == nullptr)
        return false;
    if (entity->items.count > 0)
    {
        int *pitems = (int *)entity->items.begin;
        for (int i = 0; i < entity->items.count; i++)
        {
            Entity *item = get_entity_ptr(pitems[i]);
            if (item == nullptr)
                continue;
            if (item->type->id == type)
                return true;
        }
    }
    return false;
};

void lock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at(0, 0, x, y, 0, 1);
    for (auto id : items)
    {
        Entity *door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags &= ~(1U << 19);
            door->flags |= 1U << 21;
        }
        else if (door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") || door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
        {
            door->animation_frame &= ~1U;
        }
    }
}

void unlock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at(0, 0, x, y, 0, 1);
    for (auto id : items)
    {
        Entity *door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags |= 1U << 19;
            door->flags &= ~(1U << 21);
        }
        else if (door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") || door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
        {
            door->animation_frame |= 1U;
        }
    }
}

uint32_t get_frame_count()
{
    auto state = State::get();
    return state.get_frame_count();
}

void carry(uint32_t id, uint32_t item)
{
    auto mount = get_entity_ptr(id)->as<Mount>();
    auto rider = get_entity_ptr(item)->as<Player>();
    if (mount == nullptr || rider == nullptr)
        return;
    mount->carry(rider);
}

void kill_entity(uint32_t id)
{
    Entity *ent = get_entity_ptr(id);
    if(ent != nullptr)
        ent->kill(true, nullptr);
}

void apply_entity_db(uint32_t id)
{
    Entity *ent = get_entity_ptr(id);
    if(ent != nullptr)
        ent->apply_db();
}

void flip_entity(uint32_t id)
{
    Entity *ent = get_entity_ptr(id);
    if(ent == nullptr)
        return;
    ent->flags = flipflag(ent->flags, 17);
    if (ent->items.count > 0)
    {
        int *items = (int *)ent->items.begin;
        for (int i = 0; i < ent->items.count; i++)
        {
            Entity *item = get_entity_ptr(items[i]);
            item->flags = flipflag(item->flags, 17);
        }
    }
}

std::pair<float, float> get_camera_position()
{
    auto state = State::get();
    return state.get_camera_position();
}

void set_camera_position(float cx, float cy)
{
    auto state = State::get();
    state.set_camera_position(cx, cy);
}

void warp(uint8_t world, uint8_t level, uint8_t theme)
{
    auto state = State::get();
    state.warp(world, level, theme);
}

void set_seed(uint32_t seed)
{
    auto state = State::get();
    state.set_seed(seed);
}

void set_arrowtrap_projectile(uint32_t regular_item_id, uint32_t poison_item_id)
{
    static size_t offset_poison = 0;
    static size_t offset_regular = 0;
    if ( offset_poison == 0 )
    {
        std::string pattern = "\xBA\x73\x01\x00\x00\x48\x8B\x8C\xC1\xC0\x12\x00\x00"s;
        auto memory = Memory::get();
        // the pattern occurs twice in the executable
        // the first instance is for poison arrowtraps
        // the second is for regular arrowtraps
        offset_poison = find_inst(memory.exe(), pattern, memory.after_bundle);
        offset_regular = memory.at_exe(find_inst(memory.exe(), pattern, offset_poison + 1));
        offset_poison = memory.at_exe(offset_poison);
    }
    write_mem_prot(offset_regular + 1, to_le_bytes(regular_item_id), true);
    write_mem_prot(offset_poison + 1, to_le_bytes(poison_item_id), true);
}

void modify_sparktraps(float angle_increment, bool clockwise, float distance)
{
    static size_t angle_instruction_offset = 0;
    static size_t angle_increment_offset = 0;
    if (angle_instruction_offset == 0)
    {
        auto memory = Memory::get();
        std::string pattern = "\xF3\x0F\x10\x81\x50\x01\x00\x00\x48\xBE\x4B\x57\x4C\x4F\x80\x3E\x83\xD3"s;
        angle_instruction_offset = find_inst(memory.exe(), pattern, memory.after_bundle) + 18;
        angle_increment_offset = memory.at_exe(decode_pc(memory.exe(), angle_instruction_offset, 4));
        angle_instruction_offset = memory.at_exe(angle_instruction_offset);
    }
    write_mem_prot(angle_increment_offset, to_le_bytes(angle_increment), true);
    if (clockwise)
    {
        write_mem_prot(angle_instruction_offset + 2, {0x5C}, true);
    }
    else
    {
        write_mem_prot(angle_instruction_offset + 2, {0x58}, true);
    }

    static size_t distance_offset = 0;
    if (distance_offset == 0)
    {
        auto state = State::get();
        auto memory = Memory::get();
        auto exe = memory.exe();

        std::string pattern = "\xF3\x0F\x10\x83\x50\x01\x00\x00\xF3\x0F\x59\x3D"s;

        // first look up this pattern so we are in the correct function
        auto offset = find_inst(exe, pattern, memory.after_bundle) + 8;

        // dirty trick: inject the distance float between this function and the next
        // reason is that the default float being referenced is a const with value 3.0
        // and this value is used elsewhere (as a compiler optimization)
        // so if we overwrite this value we crash elsewhere
        // -> save our own float and adjust the mulss calls to reference this
        uint8_t cc_counter = 0;
        size_t op_counter = offset;
        uint8_t previous_opcode = 0;
        while (cc_counter < 4)
        {
            unsigned char opcode = exe[op_counter];
            if (opcode == 0xcc && previous_opcode == 0xcc)
            {
                cc_counter++;
            }
            previous_opcode = opcode;
            op_counter++;
        }
        distance_offset = memory.at_exe(op_counter);

        // now overwrite the mulss instructions 4 times:
        // mulss xmm7 -> mulss xmm0 -> mulss xmm7 -> mulss xmm0
        std::string pattern1 = "\xF3\x0F\x59\x3D"s;
        std::string pattern2 = "\xF3\x0F\x59\x05"s;
        bool use_pattern1 = true;
        auto start = offset - 20;
        for (auto x = 0; x < 4; ++x)
        {
            auto mulss_offset = find_inst(exe, use_pattern1 ? pattern1 : pattern2, start);
            uint32_t distance_offset_relative = op_counter - (mulss_offset + 8);
            write_mem_prot(memory.at_exe(mulss_offset + 4), to_le_bytes(distance_offset_relative), true);
            start = mulss_offset + 1;
            use_pattern1 = !use_pattern1;
        }
    }
    write_mem_prot(distance_offset, to_le_bytes(distance), true);
}

void set_kapala_blood_threshold(uint8_t threshold)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        std::string pattern = "\xFE\x80\x28\x01\x00\x00\x80\xB8\x28\x01\x00\x00\x07"s;
        offset = memory.at_exe(find_inst(memory.exe(), pattern, memory.after_bundle) + 12);
    }
    write_mem_prot(offset, to_le_bytes(threshold), true);
}

void set_kapala_hud_icon(int8_t icon_index)
{
    static size_t instruction_offset = 0;
    static size_t icon_index_offset = 0;
    static uint32_t distance = 0;

    if (instruction_offset == 0)
    {
        auto state = State::get();
        auto memory = Memory::get();
        auto exe = memory.exe();

        std::string pattern = "\x0F\xB6\x81\x28\x01\x00\x00\x89\x02\x48\x8B\xC2"s;

        instruction_offset = find_inst(exe, pattern, memory.after_bundle);

        uint8_t cc_counter = 0;
        size_t op_counter = instruction_offset;
        uint8_t previous_opcode = 0;
        while (cc_counter < 4)
        {
            unsigned char opcode = exe[op_counter];
            if (opcode == 0xcc && previous_opcode == 0xcc)
            {
                cc_counter++;
            }
            previous_opcode = opcode;
            op_counter++;
        }
        icon_index_offset = memory.at_exe(op_counter);
        distance = op_counter - (instruction_offset + 7);
        instruction_offset = memory.at_exe(instruction_offset);
    }

    if (icon_index < 0) // reset to original
    {
        write_mem_prot(instruction_offset + 2, to_le_bytes(0x00012881), true);
    }
    else
    {
        write_mem_prot(instruction_offset + 2, {0x05}, true);
        write_mem_prot(instruction_offset + 3, to_le_bytes(distance), true);
        if (icon_index > 6)
        {
            icon_index = 6;
        }
        write_mem_prot(icon_index_offset, to_le_bytes(icon_index), true);
    }
}

SaveData* savedata()
{
    auto state = State::get();
    return state.savedata();
}

std::vector<int64_t> read_prng()
{
    auto state = State::get();
    return state.read_prng();
}
