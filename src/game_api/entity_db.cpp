#include "entity_db.hpp"
#include "entity.hpp"

#include <Windows.h> // for IsBadWritePtr
#include <chrono>    // for operator<=>, operator-, operator+
#include <cmath>     // for round
#include <compare>   // for operator<, operator<=, operator>
#include <cstdint>   // for uint32_t, uint16_t, uint8_t
#include <cstdlib>   // for abs, NULL, size_t
#include <list>      // for _List_const_iterator
#include <map>       // for _Tree_iterator, map, _Tree_cons...
#include <new>       // for operator new
#include <string>    // for allocator, string, operator""sv
#include <thread>    // for sleep_for
#include <vector>    // for vector, _Vector_iterator, erase_if

#include "containers/custom_map.hpp" // for custom_map
#include "entities_chars.hpp"        // for Player
#include "entity_hooks_info.hpp"     // for EntityHooksInfo
#include "memory.hpp"                // for write_mem_prot
#include "movable.hpp"               // for Movable
#include "movable_behavior.hpp"      // for MovableBehavior
#include "render_api.hpp"            // for RenderInfo
#include "search.hpp"                // for get_address
#include "state.hpp"                 // for State, StateMemory, enum_to_layer
#include "state_structs.hpp"         // for LiquidPhysicsEngine
#include "texture.hpp"               // for get_texture, Texture
#include "vtable_hook.hpp"           // for hook_vtable, hook_dtor, unregis...

EntityDB::EntityDB(const ENT_TYPE other)
    : EntityDB(*get_type(other)){};

EntityFactory* entity_factory()
{
    using namespace std::chrono_literals;
    static EntityFactory* cache_entity_factory = *(EntityFactory**)get_address("entity_factory"sv);
    while (cache_entity_factory == 0)
    {
        std::this_thread::sleep_for(500ms);
        cache_entity_factory = *(EntityFactory**)get_address("entity_factory"sv);
    }
    return cache_entity_factory;
}

std::vector<EntityItem> list_entities()
{
    const EntityFactory* entity_factory_ptr = entity_factory();
    if (!entity_factory_ptr)
        return {};

    const auto& map = entity_factory_ptr->entity_map;

    std::vector<EntityItem> result;
    for (const auto& [name, id] : map)
    {
        result.emplace_back(name, id);
    }
    return result;
}

EntityDB* get_type(ENT_TYPE id)
{
    EntityFactory* entity_factory_ptr = entity_factory();

    // Special case: map_ptr might be 0 if it's not initialized.
    // This only occurs in list_entities; for others, do not check the pointer
    // to see if this assumption works.
    if (!entity_factory_ptr || id > 0x393)
        return nullptr;

    return entity_factory_ptr->types + id;
}

ENT_TYPE to_id(std::string_view name)
{
    static const std::unordered_map<std::string, uint16_t>* map;
    if (map == nullptr)
    {
        const EntityFactory* entity_factory_ptr = entity_factory();
        if (!entity_factory_ptr)
            return (ENT_TYPE)~0;
        map = &entity_factory_ptr->entity_map;
    }
    auto it = map->find(std::string(name));
    return it != map->end() ? it->second : (ENT_TYPE)~0;
}

std::string_view to_name(ENT_TYPE id)
{
    const EntityFactory* entity_factory_ptr = entity_factory();
    if (entity_factory_ptr)
    {
        for (const auto& [name, type_id] : entity_factory_ptr->entity_map)
        {
            if (type_id == id)
                return name;
        }
    }
    return {};
}
