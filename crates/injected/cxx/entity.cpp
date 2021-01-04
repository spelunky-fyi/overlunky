#include "entity.hpp"
#include "injected-dll/src/db.rs.h"

#include <cstdio>

const int NAME_TO_INDEX = 0x387c8;

rust::Vec<EntityItem>
list_entities(size_t map_ptr)
{
    if (!map_ptr)
        return {};

    using EntityMap = std::unordered_map<std::string, uint16_t>;
    auto map = reinterpret_cast<EntityMap *>(map_ptr + NAME_TO_INDEX);
    auto entities = reinterpret_cast<EntityDB *>(map_ptr);

    rust::Vec<EntityItem> result;
    for (const auto &kv : *map)
    {
        result.push_back(EntityItem{kv.first, kv.second});
        // EntityDB *entity = &entities[kv.second];
        // printf("%d\n", entity->id);
    }

    return result;
}
