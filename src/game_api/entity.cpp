#include "entity.hpp"

#include <cstdio>
#include <string>
#include <vector>

#include "logger.h"
#include "render_api.hpp"
#include "rpc.hpp"
#include "state.hpp"
#include "texture.hpp"

// Items::entity_map = EntityMap;
using EntityMap = std::unordered_map<std::string, uint16_t>;

class Entity;

size_t cache_entities_ptr = 0;

size_t entities_offset()
{
    ONCE(size_t)
    {
        auto mem = Memory::get();
        return res = decode_imm(mem.exe(), find_inst(mem.exe(), "\x48\x8D\x8B"s, find_inst(mem.exe(), "\x29\x5C\x8F\x3D"s, mem.after_bundle)));
    }
}

size_t entities_ptr()
{
    if (cache_entities_ptr == 0)
    {
        auto mem = Memory::get();
        cache_entities_ptr =
            mem.at_exe(decode_pc(mem.exe(), find_inst(mem.exe(), "\x48\xB8\x02\x55\xA7\x74\x52\x9D\x51\x43"s, mem.after_bundle) - 7));
    }
    return cache_entities_ptr;
}

std::vector<EntityItem> list_entities()
{
    size_t map_ptr = *(size_t*)entities_ptr();
    size_t off = entities_offset();
    // Special case: map_ptr might be 0 if it's not initialized.
    // This only occurs in list_entities; for others, do not check the pointer
    // to see if this assumption works.
    if (!map_ptr)
        return {};

    auto map = reinterpret_cast<EntityMap*>(map_ptr + off);

    std::vector<EntityItem> result;
    for (const auto& kv : *map)
    {
        result.emplace_back(kv.first, kv.second);
        // auto entities = reinterpret_cast<EntityDB *>(map_ptr);
        // EntityDB *entity = &entities[kv.second];
        // printf("%d\n", entity->id);
    }

    return result;
}

EntityDB* get_type(uint32_t id)
{
    size_t map_ptr = *(size_t*)entities_ptr();
    // Special case: map_ptr might be 0 if it's not initialized.
    // This only occurs in list_entities; for others, do not check the pointer
    // to see if this assumption works.
    if (!map_ptr)
        return nullptr;

    auto map = reinterpret_cast<EntityMap*>(map_ptr + entities_offset());

    std::vector<EntityItem> result;
    auto entities = reinterpret_cast<EntityDB*>(map_ptr);
    return &entities[id];
}

int32_t to_id(std::string_view name)
{
    size_t map_ptr = *(size_t*)entities_ptr();
    size_t off = entities_offset();
    if (!map_ptr)
        return 0;
    auto map = reinterpret_cast<EntityMap*>(map_ptr + off);
    auto it = map->find(std::string(name));
    return it != map->end() ? it->second : -1;
}

using Carry = void (*)(Entity*, Entity*);

Carry get_carry()
{
    ONCE(Carry)
    {
        auto memory = Memory::get();
        size_t off = memory.after_bundle;
        do
        {
            // call load_item; if(rax + 0x10e != 0x0B) { ... }
            off = find_inst(memory.exe(), "\x80\xB8\x0E\x01\x00\x00\x0B"s, off + 1);
        } while (read_u8(memory.at_exe(off - 5)) != 0xE8);
        off = find_inst(memory.exe(), "\xE8"s, off + 1);

        return res = (Carry)memory.at_exe(decode_call(off));
    }
}

void Entity::teleport(float dx, float dy, bool s, float vx, float vy, bool snap)
{
    if (overlay)
        overlay->remove_item(uid);
    overlay = NULL;
    auto topmost = topmost_mount();
    auto [x, y] = topmost->position();
    if (!s)
    {
        // player relative coordinates
        x += dx;
        y += dy;
        if (snap)
        {
            x = round(x);
            y = round(y);
        }
        topmost->x = x;
        topmost->y = y;
    }
    else
    {
        // screen coordinates -1..1
        // log::debug!("Teleporting to screen {}, {}", x, y);
        auto state = State::get();
        auto [x, y] = state.click_position(dx, dy);
        if (snap && abs(vx) + abs(vy) <= 0.04)
        {
            x = round(x);
            y = round(y);
        }
        // log::debug!("Teleporting to {}, {}", x, y);
        topmost->x = x;
        topmost->y = y;
    }
    // set velocity
    if (topmost->type->search_flags < 0x80)
    {
        auto player = (Player*)topmost;
        player->velocityx = vx;
        player->velocityy = vy;
    }
    return;
}

void Entity::teleport_abs(float dx, float dy, float vx, float vy)
{
    if (overlay)
        overlay->remove_item(uid);
    overlay = NULL;
    x = dx;
    y = dy;
    if (type->search_flags < 0x80)
    {
        auto player = (Player*)pointer();
        player->velocityx = vx;
        player->velocityy = vy;
    }
}

std::pair<float, float> Entity::position()
{
    // Return the resolved position
    // overlay exists if player is riding something / etc
    auto [x, y] = position_self();
    // log::debug!("Item #{}: Position is {}, {}", unique_id(), x, y);
    switch ((size_t)overlay)
    {
    case NULL:
        return {x, y};
    default:
    {
        float _x, _y;
        std::tie(_x, _y) = overlay->position();
        return {x + _x, y + _y};
    }
    }
}

std::pair<float, float> Entity::position_self() const
{
    return std::pair<float, float>(x, y);
}

std::pair<float, float> Entity::position_render() const
{
    return std::pair<float, float>(
        rendering_info->x,
        rendering_info->y);
}

void Entity::remove_item(uint32_t id)
{
    remove_item_ptr(State::get().find(id));
}

void Door::set_target(uint8_t w, uint8_t l, uint8_t t)
{
    uint8_t array[5] = {1, l, 1, w, t};
    DEBUG("Making door go to {}-{}, {}", w, l, t);
    write_mem(pointer() + 0xc1, std::string((char*)array, sizeof(array)));
}

std::tuple<uint8_t, uint8_t, uint8_t> Door::get_target()
{
    uint8_t l = read_u8(pointer() + 0xc2);
    uint8_t w = read_u8(pointer() + 0xc4);
    uint8_t t = read_u8(pointer() + 0xc5);
    return std::make_tuple(w, l, t);
}

void Mount::carry(Movable* rider)
{
    auto carry = (get_carry());
    rider->move_state = 0x11;
    return carry(this, rider);
}

void Mount::tame(bool value)
{
    write_mem(pointer() + 0x149, to_le_bytes(value));
    flags = flags | 0x20000;
}

void Arrowtrap::rearm()
{
    if (arrow_shot)
    {
        static auto arrow_trap_trigger_id = to_id("ENT_TYPE_LOGICAL_ARROW_TRAP_TRIGGER");
        arrow_shot = false;
        auto trigger = get_entity_ptr(spawn_entity_over(arrow_trap_trigger_id, uid, 0., 0.));
        if ((flags & (1 << 16)) > 0)
        {
            trigger->flags |= (1 << 16);
        }
    }
}

void Player::set_jetpack_fuel(uint8_t fuel)
{
    static auto jetpackID = to_id("ENT_TYPE_ITEM_JETPACK");
    int* pitems = (int*)items.begin;
    for (uint8_t x = 0; x < items.count; ++x)
    {
        auto type = get_entity_type(pitems[x]);
        if (type == jetpackID)
        {
            auto jetpack = get_entity_ptr(pitems[x])->as<Jetpack>();
            jetpack->fuel = fuel;
            break;
        }
    }
}

uint8_t Player::kapala_blood_amount()
{
    static auto kapalaPowerupID = to_id("ENT_TYPE_ITEM_POWERUP_KAPALA");
    int* pitems = (int*)items.begin;
    for (uint8_t x = 0; x < items.count; ++x)
    {
        auto type = get_entity_type(pitems[x]);
        if (type == kapalaPowerupID)
        {
            auto kapala = get_entity_ptr(pitems[x])->as<KapalaPowerup>();
            return kapala->amount_of_blood;
        }
    }
    return 0;
}

void Movable::poison(int16_t frames)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        offset = memory.at_exe(find_inst(memory.exe(), "\xB8\x08\x07\x00\x00\x66\x89\x87\x18\x01\x00\x00"s, memory.after_bundle));
    }
    poison_tick_timer = frames;

    if (frames == -1)
    {
        frames = 1800;
    }
    write_mem_prot(offset + 1, to_le_bytes(frames), true);
}

bool Movable::is_poisoned()
{
    return (poison_tick_timer != -1);
}

bool Movable::is_button_pressed(uint32_t button)
{
    return (buttons & button) != 0 && (buttons & (button << 8)) == 0;
}
bool Movable::is_button_held(uint32_t button)
{
    return (buttons & button) != 0 && (buttons & (button << 8)) != 0;
}
bool Movable::is_button_released(uint32_t button)
{
    return (buttons & button) == 0 && (buttons & (button << 8)) != 0;
}

uint8_t Olmec::broken_floaters()
{
    static auto olmec_floater_id = to_id("ENT_TYPE_FX_OLMECPART_FLOATER");
    uint8_t broken = 0;
    int* pitems = (int*)items.begin;
    for (uint8_t x = 0; x < items.count; ++x)
    {
        auto type = get_entity_type(pitems[x]);
        if (type == olmec_floater_id)
        {
            auto olmec_floater = get_entity_ptr(pitems[x]);
            if (olmec_floater->animation_frame == 0x27)
            {
                broken++;
            }
        }
    }
    return broken;
}

void Entity::destroy()
{
    delete this; // TODO
}

bool Entity::set_texture(std::uint32_t texture_id)
{
    const auto* textures = get_textures();
    if (texture_id >= sizeof(textures->texture_map) / sizeof(Texture*))
        return false;

    texture = textures->texture_map[texture_id];
    rendering_info->texture = texture;
    rendering_info->texture_name = texture->name;
    return true;
}
