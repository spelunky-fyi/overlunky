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

template <typename T>
class OnHeapPointer;

EntityDB::EntityDB(const EntityDB& other) = default;
EntityDB::EntityDB(const ENT_TYPE other)
    : EntityDB(*get_type(other))
{
}

using namespace std::chrono_literals;
using EntityMap = std::unordered_map<std::string, uint16_t>;

struct EntityBucket
{
    void** begin;
    void** current; // Note, counts down from end to begin instead of up from begin to end :shrug:
    void** end;
};
struct EntityPool
{
    std::uint32_t slot_size;
    std::uint32_t initial_slots;
    std::uint32_t slots_growth;
    std::uint32_t current_slots;
    std::uint64_t _ulong_0;
    EntityBucket* _some_bucket;
    EntityBucket* bucket;
};
struct EntityFactory
{
    EntityDB types[0x395];
    bool type_set[0x395];
    std::unordered_map<std::uint32_t, OnHeapPointer<EntityPool>> entity_instance_map;
    EntityMap entity_map;
    void* _ptr_7;
};

EntityFactory* entity_factory()
{
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

    const EntityMap& map = entity_factory_ptr->entity_map;

    std::vector<EntityItem> result;
    for (const auto& [name, id] : map)
    {
        result.emplace_back(name, id);
    }
    return result;
}

EntityDB* get_type(uint32_t id)
{
    EntityFactory* entity_factory_ptr = entity_factory();

    // Special case: map_ptr might be 0 if it's not initialized.
    // This only occurs in list_entities; for others, do not check the pointer
    // to see if this assumption works.
    if (!entity_factory_ptr)
        return nullptr;

    return entity_factory_ptr->types + id;
}

ENT_TYPE to_id(std::string_view name)
{
    const EntityFactory* entity_factory_ptr = entity_factory();
    if (!entity_factory_ptr)
        return {};
    const EntityMap& map = entity_factory_ptr->entity_map;
    auto it = map.find(std::string(name));
    return it != map.end() ? it->second : -1;
}

std::string_view to_name(ENT_TYPE id)
{
    const EntityFactory* entity_factory_ptr = entity_factory();
    if (entity_factory_ptr)
    {
        for (const auto& [name, type_id] : entity_factory_ptr->entity_map)
        {
            if (type_id == id)
            {
                return name;
            }
        }
    }

    return {};
}
