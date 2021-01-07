#include "entity.hpp"
#include "injected-dll/src/db.rs.h"

#include <cstdio>

const int NAME_TO_INDEX = 0x387c8;
using EntityMap = std::unordered_map<std::string, uint16_t>;

rust::Vec<EntityItem>
list_entities(size_t map_ptr)
{
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
to_id(size_t map_ptr, rust::String id) {
    if(!map_ptr) {
        return -1;
    }

    auto map = reinterpret_cast<EntityMap *>(map_ptr + NAME_TO_INDEX);
    auto it = map->find(std::string(id.data(), id.size()));
    return it != map->end() ? it->second : -1;
}