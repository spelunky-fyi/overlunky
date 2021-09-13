#include "rpc.hpp"

#include "drops.hpp"
#include "entities_floors.hpp"
#include "entities_mounts.hpp"
#include "entity.hpp"
#include "logger.h"
#include "state.hpp"
#include "virtual_table.hpp"
#include <cstdarg>

uint32_t setflag(uint32_t flags, int bit) //shouldn't we change those to #define ?
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
    return (flags ^ (1U << (bit - 1)));
}

void teleport(float x, float y, bool s, float vx, float vy, bool snap) //ui only
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

void attach_entity(Entity* overlay, Entity* attachee)
{
    using AttachEntity = void (*)(Entity*, Entity*);
    static AttachEntity attach_entity = []()
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x48\x8b\xd0\x48\x8b\xcd\x48\x8b\xd8"s, memory.after_bundle);
        off = find_inst(memory.exe(), "\xe8"s, off);
        return (AttachEntity)memory.at_exe(Memory::decode_call(off));
    }();
    attach_entity(overlay, attachee);
}

void attach_entity_by_uid(uint32_t overlay_uid, uint32_t attachee_uid)
{
    if (Entity* overlay = get_entity_ptr(overlay_uid))
    {
        if (Entity* attachee = get_entity_ptr(attachee_uid))
        {
            attach_entity(overlay, attachee);
        }
    }
}

int32_t attach_ball_and_chain(uint32_t uid, float off_x, float off_y)
{
    if (Entity* entity = get_entity_ptr(uid))
    {
        static const auto ball_entity_type = to_id("ENT_TYPE_ITEM_PUNISHBALL");
        static const auto chain_entity_type = to_id("ENT_TYPE_ITEM_PUNISHCHAIN");

        auto [x, y, l] = get_position(uid);
        auto* layer_ptr = State::get().layer(l);

        PunishBall* ball = (PunishBall*)layer_ptr->spawn_entity(ball_entity_type, x + off_x, y + off_y, false, 0.0f, 0.0f, false);

        ball->attached_to_uid = uid;

        const uint8_t chain_length = 15;
        for (uint8_t i = 0; i < chain_length; i++)
        {
            StretchChain* chain = (StretchChain*)layer_ptr->spawn_entity(chain_entity_type, x, y, false, 0.0f, 0.0f, false);
            chain->animation_frame -= (i % 2);

            chain->at_end_of_chain_uid = ball->uid;
            chain->dot_offset = (float)i / chain_length;
            chain->position_in_chain = i;
            chain->inverse_doubled_position_in_chain = (chain_length - i) * 2;
        }
        return ball->uid;
    }
    return -1;
}

void stack_entities(uint32_t bottom_uid, uint32_t top_uid, float (&offset)[2])
{
    using StackEntities = void (*)(Entity*, Entity*, float(&)[2]);
    static StackEntities stack_entities = []()
    {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\x49\x8b\xc9\xf3\x0f\x11\x5c\x24\x34"s, memory.after_bundle);
        off = find_inst(memory.exe(), "\xe8"s, off);
        return (StackEntities)memory.at_exe(Memory::decode_call(off));
    }();

    if (Entity* bottom = get_entity_ptr(bottom_uid))
    {
        if (Entity* top = get_entity_ptr(top_uid))
        {
            stack_entities(bottom, top, offset);
        }
    }
}

int32_t get_entity_at(float x, float y, bool s, float radius, uint32_t mask) // ui only
{
    auto state = State::get();
    if (s)
    {
        auto [rx, ry] = state.click_position(x, y);
        x = rx;
        y = ry;
    }
    //DEBUG("Items at {}:", (x, y));
    auto player = state.items()->player(0);
    if (player == nullptr)
        return -1;
    std::vector<std::tuple<int32_t, float, Entity*>> found;
    for (auto& item : state.layer(player->layer)->items())
    {
        auto [ix, iy] = item->position();
        auto flags = item->type->search_flags;
        float distance = sqrt(pow(x - ix, 2.0f) + pow(y - iy, 2.0f));
        if (((mask & flags) > 0 || mask == 0) && distance < radius)
        {
            /*DEBUG(
                "Item {}, {:x} type, {} position, {} distance, {:x}",
                item->uid,
                item->type->search_flags,
                item->position_self(),
                distance,
                item->pointer());*/
            found.push_back({item->uid, distance, item});
        }
    }
    if (!found.empty())
    {
        std::sort(found.begin(), found.end(), [](auto a, auto b) -> bool
                  { return std::get<1>(a) < std::get<1>(b); });
        auto picked = found[0];
        //auto entity = std::get<2>(picked);
        //DEBUG("{}", (void*)entity);
        return std::get<0>(picked);
    }
    return -1;
}

int32_t get_grid_entity_at(float x, float y, LAYER layer)
{
    auto state = State::get();
    uint8_t actual_layer = enum_to_layer(layer);

    if (Entity* ent = state.layer(actual_layer)->get_grid_entity_at(x, y))
        return ent->uid;

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
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        return ent->flags;
    return 0;
}

void set_entity_flags(uint32_t uid, uint32_t flags)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->flags = flags;
}

uint32_t get_entity_flags2(uint32_t uid)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        return ent->more_flags;
    return 0;
}

void set_entity_flags2(uint32_t uid, uint32_t flags)
{
    auto state = State::get();
    auto ent = state.find(uid);
    if (ent)
        ent->more_flags = flags;
}

int get_entity_ai_state(uint32_t uid)
{
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

ENT_TYPE get_entity_type(uint32_t uid)
{
    auto state = State::get();
    auto p = state.find(uid);
    if (p == nullptr || IsBadWritePtr(p, 0x178))
        return UINT32_MAX;
    return p->type->id;
}

StateMemory* get_state_ptr()
{
    return State::get().ptr();
}

std::vector<Player*> get_players()
{
    auto state = State::get();
    std::vector<Player*> found;
    for (uint8_t i = 0; i < MAX_PLAYERS; i++)
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

std::tuple<float, float, float, float> screen_aabb(float left, float top, float right, float bottom)
{
    auto [sx1, sy1] = screen_position(left, top);
    auto [sx2, sy2] = screen_position(right, bottom);
    return std::tuple{sx1, sy1, sx2, sy2};
}

float screen_distance(float x)
{
    auto a = State::get().screen_position(0, 0);
    auto b = State::get().screen_position(x, 0);
    return b.first - a.first;
}

std::vector<uint32_t> get_entities()
{
    return get_entities_by({}, 0, LAYER::BOTH);
}

std::vector<uint32_t> get_entities_by_layer(LAYER layer)
{
    return get_entities_by({}, 0, layer);
}

std::vector<uint32_t> get_entities_by_type(std::vector<ENT_TYPE> entity_types)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    uint8_t layer = 2;
    while (layer)
    {
        layer--;
        for (auto& item : state.layer(layer)->items())
        {
            if (std::find(entity_types.begin(), entity_types.end(), item->type->id) != entity_types.end())
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}

template <typename... Args>
std::vector<uint32_t> get_entities_by_type(Args... args)
{
    std::vector<ENT_TYPE> types = {args...};
    return get_entities_by_type(types);
}

std::vector<uint32_t> get_entities_by_mask(uint32_t mask)
{
    return get_entities_by({}, mask, LAYER::BOTH);
}

std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    if (layer == LAYER::BOTH)
    {
        uint8_t layeridx = 2;
        while (layeridx)
        {
            layeridx--;
            for (auto& item : state.layer(layeridx)->items())
            {
                if (((item->type->search_flags & mask) || mask == 0) && entity_type_check(entity_types, item->type->id))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);

        for (auto& item : state.layer(actual_layer)->items())
        {
            if (((item->type->search_flags & mask) || mask == 0) && entity_type_check(entity_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_at(std::vector<ENT_TYPE> entity_types, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    auto state = State::get();
    std::vector<uint32_t> found;
    if (layer == LAYER::BOTH)
    {
        uint8_t layeridx = 2;
        while (layeridx)
        {
            layeridx--;
            for (auto& item : state.layer(layeridx)->items())
            {
                auto [ix, iy] = item->position();
                float distance = sqrt(pow(x - ix, 2.0f) + pow(y - iy, 2.0f));
                if (((item->type->search_flags & mask) > 0 || mask == 0) && distance < radius && entity_type_check(entity_types, item->type->id))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);

        for (auto& item : state.layer(actual_layer)->items())
        {
            auto [ix, iy] = item->position();
            float distance = sqrt(pow(x - ix, 2.0f) + pow(y - iy, 2.0f));
            if (((item->type->search_flags & mask) > 0 || mask == 0) && distance < radius && entity_type_check(entity_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_overlapping_hitbox(std::vector<ENT_TYPE> entity_types, uint32_t mask, AABB hitbox, LAYER layer)
{
    auto state = State::get();
    std::vector<uint32_t> result;
    if (layer == LAYER::BOTH)
    {
        std::vector<uint32_t> result2;
        result = get_entities_overlapping_by_pointer(entity_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(0));
        result2 = get_entities_overlapping_by_pointer(entity_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(1));
        result.insert(result.end(), result2.begin(), result2.end());
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);
        result = get_entities_overlapping_by_pointer(entity_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state.layer(actual_layer));
    }
    return result;
}

std::vector<uint32_t> get_entities_overlapping(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(entity_types, mask, {sx, sy2, sx2, sy}, layer);
}

std::vector<uint32_t> get_entities_overlapping_by_pointer(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    std::vector<uint32_t> found;
    for (auto& item : layer->items())
    {
        if (((item->type->search_flags & mask) > 0 || mask == 0) && entity_type_check(entity_types, item->type->id) && item->overlaps_with(sx, sy, sx2, sy2))
        {
            found.push_back(item->uid);
        }
    }
    return found;
}

void set_door_target(uint32_t uid, uint8_t w, uint8_t l, uint8_t t)
{
    Entity* door = get_entity_ptr(uid);
    if (door == nullptr)
        return;
    static_cast<Door*>(door)->set_target(w, l, t);
}

std::tuple<uint8_t, uint8_t, uint8_t> get_door_target(uint32_t uid)
{
    Entity* door = get_entity_ptr(uid);
    if (door == nullptr)
        return std::make_tuple((uint8_t)0, (uint8_t)0, (uint8_t)0);
    return static_cast<Door*>(door)->get_target();
}

void set_contents(uint32_t uid, ENT_TYPE item_entity_type)
{
    Entity* container = get_entity_ptr(uid);
    if (container == nullptr)
        return;
    uint32_t type = container->type->id;
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
        auto pitems = entity->items.begin;
        for (unsigned int i = 0; i < entity->items.count; i++)
        {
            if (pitems[i] == item_uid)
                return true;
        }
    }
    return false;
};

bool entity_has_item_type(uint32_t uid, std::vector<ENT_TYPE> entity_types)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;
    if (entity->items.count > 0)
    {
        int* pitems = (int*)entity->items.begin;
        for (unsigned int i = 0; i < entity->items.count; i++)
        {
            Entity* item = get_entity_ptr(pitems[i]);
            if (item == nullptr)
                continue;
            if (std::find(entity_types.begin(), entity_types.end(), item->type->id) != entity_types.end())
                return true;
        }
    }
    return false;
};

std::vector<uint32_t> entity_get_items_by(uint32_t uid, std::vector<ENT_TYPE> entity_types, uint32_t mask)
{
    std::vector<uint32_t> found;
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return found;
    if (entity->items.count > 0)
    {
        uint32_t* pitems = entity->items.begin;
        for (unsigned int i = 0; i < entity->items.count; i++)
        {
            Entity* item = get_entity_ptr(pitems[i]);
            if (item == nullptr)
            {
                continue;
            }
            if (((item->type->search_flags & mask) || mask == 0) && entity_type_check(entity_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    }
    return found;
}

void lock_door_at(float x, float y)
{
    std::vector<uint32_t> items = get_entities_at({}, 0, x, y, LAYER::FRONT, 1);
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
    std::vector<uint32_t> items = get_entities_at({}, 0, x, y, LAYER::FRONT, 1);
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

void kill_entity(uint32_t uid, std::optional<bool> destroy_corpse)
{
    Entity* ent = get_entity_ptr(uid);
    if (ent != nullptr)
        ent->kill(destroy_corpse.value_or(true), nullptr);
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
        for (unsigned int i = 0; i < ent->items.count; i++)
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

void set_arrowtrap_projectile(ENT_TYPE regular_entity_type, ENT_TYPE poison_entity_type)
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
    static size_t angle_increment_offset = 0;
    if (angle_increment_offset == 0)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        std::string pattern = "\xF3\x0F\x10\x81\x50\x01\x00\x00\x48\xBE\x4B\x57\x4C\x4F\x80\x3E\x83\xD3"s;
        size_t angle_instruction_offset = find_inst(memory.exe(), pattern, memory.after_bundle) + 18;

        uint8_t cc_counter = 0;
        size_t op_counter = angle_instruction_offset;
        uint8_t previous_opcode = 0;
        while (cc_counter < 8)
        {
            unsigned char opcode = exe[op_counter];
            if (opcode == 0xcc && previous_opcode == 0xcc)
            {
                cc_counter++;
            }
            previous_opcode = opcode;
            op_counter++;
        }
        angle_increment_offset = memory.at_exe(op_counter);

        uint32_t distance_offset_relative = static_cast<uint32_t>(op_counter - (angle_instruction_offset + 8));
        write_mem_prot(memory.at_exe(angle_instruction_offset + 4), to_le_bytes(distance_offset_relative), true);
    }
    write_mem_prot(angle_increment_offset, to_le_bytes(angle_increment), true);

    static size_t distance_offset = 0;
    if (distance_offset == 0)
    {
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
            uint32_t distance_offset_relative = static_cast<uint32_t>(op_counter - (mulss_offset + 8));
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
        distance = static_cast<uint32_t>(op_counter - (instruction_offset + 7));
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
    Movable* ent = (Movable*)get_entity_ptr(who_uid);
    Movable* item = (Movable*)get_entity_ptr(what_uid);
    if (ent != nullptr && item != nullptr)
    {
        ent->pick_up(item);
    }
}

void drop(uint32_t who_uid, uint32_t what_uid)
{
    Movable* ent = (Movable*)get_entity_ptr(who_uid);
    Movable* item = (Movable*)get_entity_ptr(what_uid);
    if (ent != nullptr && item != nullptr)
    {
        ent->drop(item);
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
        uint32_t distance_1 = static_cast<uint32_t>(phase1_offset - (offset1 + 8));
        uint32_t distance_2 = static_cast<uint32_t>(phase2_offset - (offset2 + 8));

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
                uint8_t value = static_cast<uint8_t>(new_drop_chance);
                write_mem_prot(entry.offset, to_le_bytes(value), true);
            }
        }
    }
}

void replace_drop(uint16_t drop_id, ENT_TYPE new_drop_entity_type)
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
            gpf(state->particle_emitters, particle_emitter_id, entity);
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

uint8_t waddler_count_entity(ENT_TYPE entity_type)
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

int8_t waddler_store_entity(ENT_TYPE entity_type)
{
    auto state = get_state_ptr();
    int8_t item_stored_in_slot = -1;
    for (uint8_t x = 0; x < 99; ++x)
    {
        if (state->waddler_storage[x] == 0)
        {
            state->waddler_storage[x] = entity_type;
            item_stored_in_slot = x;
            break;
        }
    }
    return item_stored_in_slot;
}

void waddler_remove_entity(ENT_TYPE entity_type, uint8_t amount_to_remove)
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

int16_t waddler_get_entity_meta(uint8_t slot)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        return state->waddler_storage_meta[slot];
    }
    return 0;
}

void waddler_set_entity_meta(uint8_t slot, int16_t meta)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        state->waddler_storage_meta[slot] = meta;
    }
}

uint32_t waddler_entity_type_in_slot(uint8_t slot)
{
    if (slot < 99)
    {
        auto state = get_state_ptr();
        return state->waddler_storage[slot];
    }
    return 0;
}

int32_t spawn_companion(ENT_TYPE companion_type, float x, float y, LAYER layer)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        std::string pattern = "\xBA\xD7\x00\x00\x00\x8B\x44\x24\x60"s;
        offset = function_start(memory.at_exe(find_inst(exe, pattern, memory.after_bundle)));
    }
    if (offset != 0)
    {
        auto state = get_state_ptr();
        typedef Player* spawn_companion_func(StateMemory*, float x, float y, size_t layer, uint32_t entity_type);
        static spawn_companion_func* sc = (spawn_companion_func*)(offset);
        Player* spawned = sc(state, x, y, enum_to_layer(layer), companion_type);
        return spawned->uid;
    }
    return -1;
}

uint8_t enum_to_layer(const LAYER layer)
{
    if (layer == LAYER::FRONT)
        return 0;
    else if (layer == LAYER::BACK)
        return 1;
    else if ((int)layer < -MAX_PLAYERS)
        return 0;
    else if (layer < LAYER::FRONT)
    {
        auto state = State::get();
        auto player = state.items()->player(static_cast<uint8_t>(abs((int)layer) - 1));
        if (player != nullptr)
        {
            return player->layer;
        }
    }
    return 0;
}

uint8_t enum_to_layer(const LAYER layer, std::pair<float, float>& player_position)
{
    if (layer == LAYER::FRONT)
    {
        player_position = {0.0f, 0.0f};
        return 0;
    }
    else if (layer == LAYER::BACK)
    {
        player_position = {0.0f, 0.0f};
        return 1;
    }
    else if ((int)layer < -MAX_PLAYERS)
        return 0;
    else if (layer < LAYER::FRONT)
    {
        auto state = State::get();
        auto player = state.items()->player(static_cast<uint8_t>(abs((int)layer) - 1));
        if (player != nullptr)
        {
            player_position = player->position();
            return player->layer;
        }
    }
    return 0;
}

bool entity_type_check(const std::vector<ENT_TYPE>& types_array, const ENT_TYPE find)
{
    if (types_array.empty() || types_array[0] == 0 || std::find(types_array.begin(), types_array.end(), find) != types_array.end())
        return true;

    return false;
}
