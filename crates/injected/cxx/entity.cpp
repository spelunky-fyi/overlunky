#include "entity.hpp"
#include "injected-dll/src/db.rs.h"

#include <cstdio>

// Items::entity_map = EntityMap;
const int NAME_TO_INDEX = 0x387c8;
using EntityMap = std::unordered_map<std::string, uint16_t>;

// State::instance_id_to_pointer = std::unordered_map<int, EntityMemory *>;
const int INSTANCE_ID_TO_POINTER = 0x1308;
using InstanceMap = std::unordered_map<uint32_t, size_t>;

rust::Vec<EntityItem>
list_entities(size_t map_ptr)
{
    // Special case: map_ptr might be 0 if it's not initialized.
    // This only occurs in list_entities; for others, do not check the pointer to see if this assumption works.
    if (!map_ptr)
        return {};

    auto map = reinterpret_cast<EntityMap *>(map_ptr + NAME_TO_INDEX);

    rust::Vec<EntityItem> result;
    for (const auto &kv : *map)
    {
        result.push_back(EntityItem{kv.first, kv.second});
        // auto entities = reinterpret_cast<EntityDB *>(map_ptr);
        // EntityDB *entity = &entities[kv.second];
        // printf("%d\n", entity->id);
    }

    return result;
}

size_t
to_id(size_t map_ptr, rust::String name)
{
    auto map = reinterpret_cast<EntityMap *>(map_ptr + NAME_TO_INDEX);
    auto it = map->find(std::string(name.data(), name.size()));
    return it != map->end() ? it->second : -1;
}

size_t
state_find_item(size_t state_ptr, uint32_t unique_id)
{
    auto map = reinterpret_cast<InstanceMap *>(state_ptr + INSTANCE_ID_TO_POINTER);
    auto it = map->find(unique_id);
    if (it == map->end())
        return 0;
    return it->second;
}