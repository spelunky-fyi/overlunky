#include "entity.hpp"

#include <cstdio>
#include <string>
#include <vector>

#include "state.hpp"

// Items::entity_map = EntityMap;
const int NAME_TO_INDEX = 0x387c8;
using EntityMap = std::unordered_map<std::string, uint16_t>;

class Entity;

// State::instance_id_to_pointer = std::unordered_map<int, EntityMemory *>;
const int INSTANCE_ID_TO_POINTER = 0x1308;
using InstanceMap = std::unordered_map<uint32_t, Entity *>;

size_t entities_ptr() {
    auto mem = Memory::get();
    return mem.at_exe(decode_pc(
        mem.exe(),
        find_inst(mem.exe(), "\x48\xB8\x02\x55\xA7\x74\x52\x9D\x51\x43"s,
                  mem.after_bundle) -
            7));
}

std::vector<EntityItem> list_entities() {
    size_t map_ptr = *(size_t *)entities_ptr();
    // Special case: map_ptr might be 0 if it's not initialized.
    // This only occurs in list_entities; for others, do not check the pointer
    // to see if this assumption works.
    if (!map_ptr) return {};

    auto map = reinterpret_cast<EntityMap *>(map_ptr + NAME_TO_INDEX);

    std::vector<EntityItem> result;
    for (const auto &kv : *map) {
        result.push_back(EntityItem{kv.first, kv.second});
        // auto entities = reinterpret_cast<EntityDB *>(map_ptr);
        // EntityDB *entity = &entities[kv.second];
        // printf("%d\n", entity->id);
    }

    return result;
}

size_t to_id(size_t map_ptr, std::string name) {
    auto map = reinterpret_cast<EntityMap *>(map_ptr + NAME_TO_INDEX);
    auto it = map->find(std::string(name.data(), name.size()));
    return it != map->end() ? it->second : -1;
}

Entity *state_find_item(size_t state_ptr, uint32_t unique_id) {
    auto map =
        reinterpret_cast<InstanceMap *>(state_ptr + INSTANCE_ID_TO_POINTER);
    auto it = map->find(unique_id);
    if (it == map->end()) return nullptr;
    return it->second;
}

Carry get_carry() {
    ONCE(Carry) {
        auto memory = Memory::get();
        auto off = find_inst(memory.exe(), "\xBA\xE1\x00\x00\x00\x49\x8B\xCD"s,
                             memory.after_bundle);
        off = find_inst(memory.exe(), "\xE8"s, off + 1);
        off = find_inst(memory.exe(), "\xE8"s, off + 1);

        return res = (Carry)memory.at_exe(decode_call(off));
    }
}

void Entity::teleport(float dx, float dy, bool s, float vx, float vy,
                      bool snap) {
    // e.g. topmost == turkey if riding turkey. player has relative coordinate
    // to turkey.
    auto topmost = topmost_mount();
    topmost->overlay = NULL;
    auto [x, y] = topmost->position();
    if (!s) {
        // player relative coordinates
        x += dx;
        y += dy;
        if (snap) {
            x = round(x);
            y = round(y);
        }
        topmost->x = x;
        topmost->y = y;
    } else {
        // screen coordinates -1..1
        // log::debug!("Teleporting to screen {}, {}", x, y);
        auto state = State::get();
        auto [x, y] = state.click_position(dx, dy);
        if (snap && abs(vx) + abs(vy) <= 0.04) {
            x = round(x);
            y = round(y);
        }
        // log::debug!("Teleporting to {}, {}", x, y);
        topmost->x = x;
        topmost->y = y;
    }
    // set velocity
    if (topmost->type->search_flags < 0x80) {
        auto player = (Player *)topmost;
        player->velocityx = vx;
        player->velocityy = vy;
    }
    return;
}

std::pair<float, float> Entity::position() {
    // Return the resolved position
    // overlay exists if player is riding something / etc
    auto [x, y] = position_self();
    // log::debug!("Item #{}: Position is {}, {}", unique_id(), x, y);
    switch ((size_t)overlay) {
        case NULL:
            return {x, y};
        default: {
            float _x, _y;
            std::tie(_x, _y) = overlay->position();
            return {x + _x, y + _y};
        }
    }
}

std::pair<float, float> Entity::position_self() const {
    return std::pair<float, float>(x, y);
}

void Door::set_target(uint8_t w, uint8_t l, uint8_t f, uint8_t t) {
    uint8_t array[5] = {1, l, f, w, t};
    DEBUG("Making door go to {}", array);
    write_mem(pointer() + 0xc1, std::string((char *)array, sizeof(array)));
}

void Mount::carry(Movable *rider) {
    auto carry = (get_carry());
    INFO("{:#x}", read_u8(rider->pointer() + 0x10e));
    write_mem(rider->pointer() + 0x10e, "\x11");
    return carry(this, rider);
}

void Mount::tame(bool value) {
    write_mem(pointer() + 0x149, to_le_bytes(value));
    flags = flags | 0x20000;
}
