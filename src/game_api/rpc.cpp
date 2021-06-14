#include "rpc.hpp"

#include "drops.hpp"
#include "entity.hpp"
#include "logger.h"
#include "state.hpp"
#include "virtual_table.hpp"
#include <cstdarg>

uint32_t setflag(uint32_t flags, int bit)
{
    return flags | (1U << (bit - 1));
}
uint32_t clrflag(uint32_t flags, int bit)
{
    return flags & ~(1U << (bit - 1));
}
bool testflag(uint32_t flags, int bit)
{
    return (flags & (1U << (bit - 1))) > 0;
}
uint32_t flipflag(uint32_t flags, int bit)
{
    if (testflag(flags, bit))
        return clrflag(flags, bit);
    else
        return setflag(flags, bit);
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
    for (auto& item : state.layer(player->layer())->items())
    {
        DEBUG("Item {} {:x}, {}", item->uid, item->type->search_flags, item->position_self());
    }
}

int32_t get_entity_at(float x, float y, bool s, float radius, uint32_t mask)
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
    std::vector<std::tuple<int, float, Entity*>> found;
    for (auto& item : state.layer(player->layer())->items())
    {
        auto [ix, iy] = item->position();
        auto flags = item->type->search_flags;
        float distance = sqrt(pow(x - ix, 2) + pow(y - iy, 2));
        if (((mask & flags) > 0 || mask == 0) && distance < radius)
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
        std::sort(found.begin(), found.end(), [](auto a, auto b) -> bool
                  { return std::get<1>(a) < std::get<1>(b); });
        auto picked = found[0];
        auto entity = std::get<2>(picked);
        DEBUG("{}", (void*)entity);
        return std::get<0>(picked);
    }
    return -1;
}

void move_entity(uint32_t uid, float x, float y, bool s, float vx, float vy, bool snap)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->teleport(x, y, s, vx, vy, snap);
}

void move_entity_abs(uint32_t uid, float x, float y, float vx, float vy)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->teleport_abs(x, y, vx, vy);
}

uint32_t get_entity_flags(uint32_t uid)
{
    if (uid == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        return ent->flags;
    return 0;
}

void set_entity_flags(uint32_t uid, uint32_t flags)
{
    if (uid == 0)
        return;
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->flags = flags;
}

uint32_t get_entity_flags2(uint32_t uid)
{
    if (uid == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        return ent->more_flags;
    return 0;
}

void set_entity_flags2(uint32_t uid, uint32_t flags)
{
    if (uid == 0)
        return;
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->more_flags = flags;
}

int get_entity_ai_state(uint32_t uid)
{
    if (uid == 0)
        return 0;
    auto state = State::get();
    auto ent = state.find(uid)->as<Movable>();
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
    DEBUG("Player {}", (void*)status, status->ropes, status->bombs);
    status->ropes = (99);
    status->bombs = (99);
}

Entity* get_entity_ptr(uint32_t uid)
{
    auto state = State::get();
    auto p = state.find(uid);
    if (IsBadWritePtr(p, 0x178))
        return nullptr;
    return p;
}

int32_t get_entity_type(uint32_t uid)
{
    auto state = State::get();
    auto p = state.find(uid);
    if (p == nullptr || IsBadWritePtr(p, 0x178))
        return -1;
    return p->type->id;
}

StateMemory* get_state_ptr()
{
    auto state = State::get();
    return state.ptr();
}

std::vector<Player*> get_players()
{
    auto state = State::get();
    std::vector<Player*> found;
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        auto player = state.items()->player(i);
        if (player)
            found.push_back((Player*)player);
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
    for (auto& item : state.layer(0)->items())
    {
        found.push_back(item->uid);
    }
    for (auto& item : state.layer(1)->items())
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
    for (auto& item : state.layer(layer)->items())
    {
        found.push_back(item->uid);
    }
    return found;
}

std::vector<uint32_t> get_entities_by_type(std::vector<uint32_t> entity_types)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto& item : state.layer(0)->items())
    {
        if (std::find(entity_types.begin(), entity_types.end(), item->type->id) != entity_types.end())
        {
            found.push_back(item->uid);
        }
    }
    for (auto& item : state.layer(1)->items())
    {
        if (std::find(entity_types.begin(), entity_types.end(), item->type->id) != entity_types.end())
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

template <typename... Args>
std::vector<uint32_t> get_entities_by_type(Args... args)
{
    std::vector<uint32_t> types = {args...};
    auto state = State::get();
    auto player = state.items()->player(0);
    if (!player)
        return {};
    std::vector<uint32_t> found;
    for (auto& item : state.layer(0)->items())
    {
        if (std::find(types.begin(), types.end(), item->type->id) != types.end())
        {
            found.push_back(item->uid);
        }
    }
    for (auto& item : state.layer(1)->items())
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
    for (auto& item : state.layer(0)->items())
    {
        if (item->type->search_flags & mask)
        {
            found.push_back(item->uid);
        }
    }
    for (auto& item : state.layer(1)->items())
    {
        if (item->type->search_flags & mask)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_by(uint32_t entity_type, uint32_t mask, int layer)
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
        for (auto& item : state.layer(layer)->items())
        {
            if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == entity_type || entity_type == 0))
            {
                found.push_back(item->uid);
            }
        }
    }
    else
    {
        if (state.layer(0))
        {
            for (auto& item : state.layer(0)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == entity_type || entity_type == 0))
                {
                    found.push_back(item->uid);
                }
            }
        }
        if (state.layer(1))
        {
            for (auto& item : state.layer(1)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == entity_type || entity_type == 0))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_at(uint32_t entity_type, uint32_t mask, float x, float y, int layer, float radius)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    for (auto& item : state.layer(layer)->items())
    {
        auto [ix, iy] = item->position();
        float distance = sqrt(pow(x - ix, 2) + pow(y - iy, 2));
        if (((item->type->search_flags & mask) > 0 || mask == 0) && (item->type->id == entity_type || entity_type == 0) && distance < radius)
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_overlapping(uint32_t entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, int layer)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    for (auto& item : state.layer(layer)->items())
    {
        if (((item->type->search_flags & mask) > 0 || mask == 0) && (item->type->id == entity_type || entity_type == 0) && item->overlaps_with(sx, sy, sx2, sy2))
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

void set_door_target(uint32_t uid, uint8_t w, uint8_t l, uint8_t t)
{
    auto state = State::get();
    Entity* door = get_entity_ptr(uid);
    if (door == nullptr)
        return;
    static_cast<Door*>(door)->set_target(w, l, t);
}

std::tuple<uint8_t, uint8_t, uint8_t> get_door_target(uint32_t uid)
{
    auto state = State::get();
    Entity* door = get_entity_ptr(uid);
    if (door == nullptr)
        return std::make_tuple(0, 0, 0);
    return static_cast<Door*>(door)->get_target();
}

void set_contents(uint32_t uid, uint32_t item_entity_type)
{
    auto state = State::get();
    auto player = state.items()->player(0);
    if (player == nullptr)
        return;
    Entity* container = get_entity_ptr(uid);
    if (container == nullptr)
        return;
    int type = container->type->id;
    if (type != to_id("ENT_TYPE_ITEM_COFFIN") && type != to_id("ENT_TYPE_ITEM_CRATE") && type != to_id("ENT_TYPE_ITEM_PRESENT") &&
        type != to_id("ENT_TYPE_ITEM_GHIST_PRESENT") && type != to_id("ENT_TYPE_ITEM_POT"))
        return;
    container->as<Container>()->inside = item_entity_type;
}

void entity_remove_item(uint32_t uid, uint32_t item_uid)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return;
    entity->remove_item(item_uid);
}

bool entity_has_item_uid(uint32_t uid, uint32_t item_uid)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;
    if (entity->items.count > 0)
    {
        int* pitems = (int*)entity->items.begin;
        for (int i = 0; i < entity->items.count; i++)
        {
            if (pitems[i] == item_uid)
                return true;
        }
    }
    return false;
};

bool entity_has_item_type(uint32_t uid, uint32_t entity_type)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;
    if (entity->items.count > 0)
    {
        int* pitems = (int*)entity->items.begin;
        for (int i = 0; i < entity->items.count; i++)
        {
            Entity* item = get_entity_ptr(pitems[i]);
            if (item == nullptr)
                continue;
            if (item->type->id == entity_type)
                return true;
        }
    }
    return false;
};

std::vector<uint32_t> entity_get_items_by(uint32_t uid, uint32_t entity_type, uint32_t mask)
{
    std::vector<uint32_t> found;
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return found;
    if (entity->items.count > 0)
    {
        int* pitems = (int*)entity->items.begin;
        for (int i = 0; i < entity->items.count; i++)
        {
            Entity* item = get_entity_ptr(pitems[i]);
            if (item == nullptr)
            {
                continue;
            }
            if (((item->type->search_flags & mask) || mask == 0) && (item->type->id == entity_type || entity_type == 0))
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}

void lock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at(0, 0, x, y, 0, 1);
    for (auto id : items)
    {
        Entity* door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags &= ~(1U << 19);
            door->flags |= 1U << 21;
        }
        else if (
            door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") ||
            door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
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
        Entity* door = get_entity_ptr(id);
        if (door->type->id >= to_id("ENT_TYPE_FLOOR_DOOR_ENTRANCE") && door->type->id <= to_id("ENT_TYPE_FLOOR_DOOR_EGGPLANT_WORLD"))
        {
            door->flags |= 1U << 19;
            door->flags &= ~(1U << 21);
        }
        else if (
            door->type->id == to_id("ENT_TYPE_BG_DOOR") || door->type->id == to_id("ENT_TYPE_BG_DOOR_COG") ||
            door->type->id == to_id("ENT_TYPE_BG_DOOR_EGGPLANT_WORLD"))
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

void carry(uint32_t mount_uid, uint32_t rider_uid)
{
    auto mount = get_entity_ptr(mount_uid)->as<Mount>();
    auto rider = get_entity_ptr(rider_uid)->as<Player>();
    if (mount == nullptr || rider == nullptr)
        return;
    mount->carry(rider);
}

void kill_entity(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->kill(true, nullptr);
}

void destroy_entity(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->destroy(); // TODO
}

void apply_entity_db(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->apply_db();
}

void flip_entity(uint32_t uid)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent == nullptr)
        return;
    ent->flags = flipflag(ent->flags, 17);
    if (ent->items.count > 0)
    {
        int* items = (int*)ent->items.begin;
        for (int i = 0; i < ent->items.count; i++)
        {
            Entity* item = get_entity_ptr(items[i]);
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

void set_arrowtrap_projectile(uint32_t regular_entity_type, uint32_t poison_entity_type)
{
    static size_t offset_poison = 0;
    static size_t offset_regular = 0;
    if (offset_poison == 0)
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
    write_mem_prot(offset_regular + 1, to_le_bytes(regular_entity_type), true);
    write_mem_prot(offset_poison + 1, to_le_bytes(poison_entity_type), true);
}

void modify_sparktraps(float angle_increment, float distance)
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

void set_blood_multiplication(uint32_t default_multiplier, uint32_t vladscape_multiplier)
{
    size_t offset_default1 = 0;
    size_t offset_vladscape1 = 0;
    size_t offset_default2 = 0;
    size_t offset_vladscape2 = 0;
    if (offset_default1 == 0)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        std::string pattern = "\x41\xB8\x02\x00\x00\x00\x84\xC0\x75\x06\x41\xB8\x01\x00\x00\x00"s;
        auto offset = find_inst(exe, pattern, memory.after_bundle);
        offset_default1 = memory.at_exe(offset + 12);
        offset_vladscape1 = memory.at_exe(offset + 2);
        offset = find_inst(exe, pattern, offset + 1);
        offset_default2 = memory.at_exe(offset + 12);
        offset_vladscape2 = memory.at_exe(offset + 2);
    }

    write_mem_prot(offset_default1, to_le_bytes(default_multiplier), true);
    write_mem_prot(offset_default2, to_le_bytes(default_multiplier), true);
    write_mem_prot(offset_vladscape1, to_le_bytes(vladscape_multiplier), true);
    write_mem_prot(offset_vladscape2, to_le_bytes(vladscape_multiplier), true);
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

void pick_up(uint32_t who_uid, uint32_t what_uid)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        offset = memory.at_exe(find_inst(memory.exe(), "\x48\x89\x5c\x24\x08\x57\x48\x83\xec\x20\x4c\x8b\x5a\x08"s, memory.after_bundle));
    }
    Movable* ent = (Movable*)get_entity_ptr(who_uid);
    Movable* item = (Movable*)get_entity_ptr(what_uid);
    if (ent != nullptr && item != nullptr)
    {
        auto pick_up_func = (void (*)(Movable*, Movable*))offset;
        pick_up_func(ent, item);
    }
}

void set_olmec_phase_y_level(uint8_t phase, float y)
{
    // Sets the Y-level Olmec changes phases. The defaults are :
    // - phase 1 (bombs) = 99 (+1)  (the game adds 1 to the fixed value for some reason)
    // - phase 2 (ufos) = 83
    // Olmecs checks phases in order! The means if you want ufo's from the start
    // you have to put both phase 1 and 2 at e.g. level 199
    // If you want to make Olmec stay in phase 0 (stomping) all the time, you can just set
    // the phase 1 y level to e.g. 10.
    static size_t phase1_offset = 0;
    static size_t phase2_offset = 0;
    if (phase1_offset == 0)
    {
        auto state = State::get();
        auto memory = Memory::get();
        auto exe = memory.exe();

        std::string pattern_phase1 = "\xF3\x0F\x10\x15\x1B\x8C\x36\x00"s;
        std::string pattern_phase2 = "\xF3\x0F\x10\x0D\xE1\x87\x36\x00"s;

        // first look up these patterns so we are in the correct function
        auto offset1 = find_inst(exe, pattern_phase1, memory.after_bundle);
        auto offset2 = find_inst(exe, pattern_phase2, memory.after_bundle);

        // find the space inbetween this function and the next
        uint8_t cc_counter = 0;
        size_t op_counter = offset1;
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

        // here's the memory location where we save our floats
        phase1_offset = op_counter;
        phase2_offset = phase1_offset + 4;

        // write the default values to our new floats
        write_mem_prot(memory.at_exe(phase1_offset), to_le_bytes(99.0f), true);
        write_mem_prot(memory.at_exe(phase2_offset), to_le_bytes(83.0f), true);

        // calculate the distances between our floats and the movss instructions
        uint32_t distance_1 = phase1_offset - (offset1 + 8);
        uint32_t distance_2 = phase2_offset - (offset2 + 8);

        // overwrite the movss instructions to load our floats
        write_mem_prot(memory.at_exe(offset1 + 4), to_le_bytes(distance_1), true);
        write_mem_prot(memory.at_exe(offset2 + 4), to_le_bytes(distance_2), true);

        phase1_offset = memory.at_exe(phase1_offset);
        phase2_offset = memory.at_exe(phase2_offset);
    }

    if (phase == 1)
    {
        write_mem_prot(phase1_offset, to_le_bytes(y), true);
    }
    else if (phase == 2)
    {
        write_mem_prot(phase2_offset, to_le_bytes(y), true);
    }
}

void set_ghost_spawn_times(uint32_t normal, uint32_t cursed)
{
    static size_t normal_offset = 0;
    static size_t cursed_offset = 0;
    if (normal_offset == 0)
    {
        auto state = State::get();
        auto memory = Memory::get();
        auto exe = memory.exe();

        std::string pattern = "\xBA\x28\x23\x00\x00\x41\xB8\x30\x2A\x00\x00"s;
        auto offset = find_inst(exe, pattern, memory.after_bundle);
        normal_offset = memory.at_exe(offset + 7);
        cursed_offset = memory.at_exe(offset + 1);
    }
    write_mem_prot(normal_offset, to_le_bytes(normal), true);
    write_mem_prot(cursed_offset, to_le_bytes(cursed), true);
}

void set_drop_chance(uint16_t dropchance_id, uint32_t new_drop_chance)
{
    if (dropchance_id < dropchance_entries.size())
    {
        auto& entry = dropchance_entries.at(dropchance_id);
        if (entry.offset == 0)
        {
            auto memory = Memory::get();
            size_t offset = memory.at_exe(find_inst(memory.exe(), entry.pattern, get_virtual_function_address(entry.vtable_offset, entry.vtable_rel_offset)));
            if (offset > memory.exe_ptr)
            {
                entry.offset = offset;
            }
        }

        if (entry.offset != 0)
        {
            if (entry.chance_sizeof == 4)
            {
                write_mem_prot(entry.offset, to_le_bytes(new_drop_chance), true);
            }
            else if (entry.chance_sizeof == 1)
            {
                uint8_t value = new_drop_chance;
                write_mem_prot(entry.offset, to_le_bytes(value), true);
            }
        }
    }
}

void replace_drop(uint16_t drop_id, uint32_t new_drop_entity_type)
{
    if (new_drop_entity_type == 0)
    {
        return;
    }

    if (drop_id < drop_entries.size())
    {
        auto& entry = drop_entries.at(drop_id);
        if (entry.offsets[0] == 0)
        {
            auto memory = Memory::get();
            size_t offset = 0;
            size_t exe_offset = 0;
            if (entry.vtable_offset == VTABLE_OFFSET::NONE)
            {
                exe_offset = memory.at_exe(find_inst(memory.exe(), entry.pattern, memory.after_bundle) + entry.value_offset);
            }
            else
            {
                offset = find_inst(memory.exe(), entry.pattern, get_virtual_function_address(entry.vtable_offset, entry.vtable_rel_offset)) + entry.value_offset;
                exe_offset = memory.at_exe(offset);
            }

            for (auto x = 0; x < entry.vtable_occurrence; ++x)
            {
                if (exe_offset > memory.exe_ptr)
                {
                    entry.offsets[x] = exe_offset;
                }

                if (x + 1 < entry.vtable_occurrence)
                {
                    offset = find_inst(memory.exe(), entry.pattern, offset + 1) + entry.value_offset;
                    exe_offset = memory.at_exe(offset);
                }
            }
        }

        if (entry.offsets[0] != 0)
        {
            for (auto x = 0; x < entry.vtable_occurrence; ++x)
            {
                write_mem_prot(entry.offsets[x], to_le_bytes(new_drop_entity_type), true);
            }
        }
    }
}

void force_co_subtheme(int8_t subtheme)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        offset = memory.at_exe(find_inst(memory.exe(), "\x48\xC1\xE0\x03\x48\xC1\xE8\x20\x49\x89\x48\x08\x48\x98"s, memory.after_bundle));
    }
    if (subtheme >= 0 && subtheme <= 7)
    {
        uint8_t replacement[] = {0xB8, (uint8_t)subtheme, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90};
        std::string replacement_s = std::string((char*)replacement, sizeof(replacement));
        write_mem_prot(offset, replacement_s, true);
    }
    else if (subtheme == -1)
    {
        write_mem_prot(offset, "\x48\xC1\xE0\x03\x48\xC1\xE8\x20"s, true);
    }
}

void generate_particles(uint32_t particle_emitter_id, uint32_t uid)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        std::string pattern = "\x48\x8B\xD9\xB9\xB0\x01\x00\x00"s;
        offset = function_start(memory.at_exe(find_inst(exe, pattern, memory.after_bundle)));
    }

    if (offset != 0)
    {
        auto entity = get_entity_ptr(uid);
        if (entity != nullptr)
        {
            auto state = get_state_ptr();
            typedef size_t generate_particles_func(PointerList*, uint32_t, Entity*);
            static generate_particles_func* gpf = (generate_particles_func*)(offset);
            auto result = gpf(state->particle_emitters, particle_emitter_id, entity);
        }
    }
}

void set_journal_enabled(bool b)
{
    static size_t offset = 0;
    static char original_call_instruction[5] = {0};
    if (offset == 0)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        std::string pattern = "\x75\x1F\xF6\xC1\x08"s;
        offset = memory.at_exe(find_inst(exe, pattern, memory.after_bundle) + 14);
        for (uint8_t x = 0; x < 5; ++x)
        {
            original_call_instruction[x] = read_u8(offset + x);
        }
    }
    if (b)
    {
        write_mem_prot(offset, std::string(original_call_instruction, 5), true);
    }
    else
    {
        write_mem_prot(offset, "\x90\x90\x90\x90\x90"s, true);
    }
}

uint8_t waddler_count_entity(uint32_t entity_type)
{
    auto state = get_state_ptr();
    uint8_t count = 0;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (state->waddler_storage[x] == entity_type)
        {
            count++;
        }
    }
    return count;
}

bool waddler_store_entity(uint32_t entity_type)
{
    auto state = get_state_ptr();
    bool item_stored = false;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (state->waddler_storage[x] == 0)
        {
            state->waddler_storage[x] = entity_type;
            item_stored = true;
            break;
        }
    }
    return item_stored;
}

void waddler_remove_entity(uint32_t entity_type, uint8_t amount_to_remove)
{
    auto state = get_state_ptr();

    uint8_t remove_count = 0;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (amount_to_remove == remove_count)
        {
            break;
        }

        if (state->waddler_storage[x] == entity_type)
        {
            state->waddler_storage[x] = 0;
            remove_count++;
        }
    }

    if (remove_count > 0)
    {
        uint32_t tmp[99] = {0};
        uint8_t tmp_x = 0;
        for (uint8_t x = 0; x < 99; ++x)
        {
            if (state->waddler_storage[x] != 0)
            {
                tmp[tmp_x++] = state->waddler_storage[x];
            }
        }
        memcpy(&(state->waddler_storage[0]), tmp, 99 * sizeof(uint32_t));
    }
}
