#include "entity_lookup.hpp"

#include <cstdint>
#include <vector>

#include "aliases.hpp"
#include "custom_types.hpp"
#include "entity.hpp"
#include "layer.hpp"
#include "math.hpp"
#include "state.hpp"

bool entity_type_check(const std::vector<ENT_TYPE>& types_array, const ENT_TYPE find)
{
    if (types_array.empty() || types_array[0] == 0 || std::find(types_array.begin(), types_array.end(), find) != types_array.end())
        return true;

    return false;
}

std::vector<ENT_TYPE> get_proper_types(std::vector<ENT_TYPE> ent_types)
{
    for (size_t i = 0; i < ent_types.size(); ++i)
    {
        if (ent_types[i] >= (uint32_t)CUSTOM_TYPE::ACIDBUBBLE)
        {
            auto extra_types = get_custom_entity_types(static_cast<CUSTOM_TYPE>(ent_types[i]));
            if (extra_types.size() == 1)
            {
                ent_types[i] = extra_types[0];
            }
            else if (!extra_types.empty())
            {
                auto it = ent_types.begin() + i;
                *it = extra_types[0];
                ent_types.insert(++it, extra_types.begin() + 1, extra_types.end());
                i += extra_types.size() - 1;
            }
        }
    }
    return ent_types;
}

int32_t get_grid_entity_at(float x, float y, LAYER layer)
{
    StateMemory* state = State::ptr();
    uint8_t actual_layer = enum_to_layer(layer);

    if (Entity* ent = state->layers[actual_layer]->get_grid_entity_at(x, y))
        return ent->uid;

    return -1;
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
    return get_entities_by(std::move(entity_types), 0, LAYER::BOTH);
}
std::vector<uint32_t> get_entities_by_type(ENT_TYPE entity_type)
{
    return get_entities_by(std::vector<ENT_TYPE>{entity_type}, 0, LAYER::BOTH);
}

std::vector<uint32_t> get_entities_by_mask(uint32_t mask)
{
    return get_entities_by({}, mask, LAYER::BOTH);
}

template <class FunT>
requires std::is_invocable_v<FunT, const EntityList&>
void foreach_mask(uint32_t mask, Layer* l, FunT&& fun)
{
    if (mask == 0)
    {
        fun(l->all_entities);
    }
    else
    {
        for (uint32_t test_flag = 1U; test_flag < 0x8000; test_flag <<= 1U)
        {
            if (mask & test_flag)
            {
                const auto& it = l->entities_by_mask.find(test_flag);
                if (it != l->entities_by_mask.end())
                {
                    fun(it->second);
                }
            }
        }
    }
}

std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, uint32_t mask, LAYER layer)
{
    auto state = State::ptr();
    std::vector<uint32_t> found;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));

    auto push_matching_types = [&proper_types, &found](const EntityList& entities)
    {
        for (auto& item : entities.entities())
        {
            if (entity_type_check(proper_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    };
    auto insert_all_uids = [&found](const EntityList& entities)
    {
        const auto uids = entities.uids();
        found.insert(found.end(), uids.begin(), uids.end());
    };

    if (layer == LAYER::BOTH)
    {
        auto layer_front = state->layers[0];
        auto layer_back = state->layers[1];
        if (proper_types.empty() || proper_types[0] == 0)
        {
            if (mask == 0) // all entities
            {
                // this exception for small improvments with calling reserve once
                found.reserve(found.size() + (size_t)layer_front->all_entities.size + (size_t)layer_back->all_entities.size);
                found.insert(found.end(), layer_front->all_entities.uids().begin(), layer_front->all_entities.uids().end());
                found.insert(found.end(), layer_back->all_entities.uids().begin(), layer_back->all_entities.uids().end());
            }
            else // all types
            {
                foreach_mask(mask, layer_front, insert_all_uids);
                foreach_mask(mask, layer_back, insert_all_uids);
            }
        }
        else
        {
            foreach_mask(mask, layer_front, push_matching_types);
            foreach_mask(mask, layer_back, push_matching_types);
        }
    }
    else
    {
        uint8_t correct_layer = enum_to_layer(layer);
        if (proper_types.empty() || proper_types[0] == 0) // all types
        {
            foreach_mask(mask, state->layers[correct_layer], insert_all_uids);
        }
        else
        {
            foreach_mask(mask, state->layers[correct_layer], push_matching_types);
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_by(ENT_TYPE entity_type, uint32_t mask, LAYER layer)
{
    return get_entities_by(std::vector<ENT_TYPE>{entity_type}, mask, layer);
}

std::vector<uint32_t> get_entities_at(std::vector<ENT_TYPE> entity_types, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    // TODO: use entity regions?
    StateMemory* state = State::ptr();
    std::vector<uint32_t> found;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
    auto push_entities_at = [&x, &y, &radius, &proper_types, &found](const EntityList& entities)
    {
        for (auto& item : entities.entities())
        {
            auto [ix, iy] = item->position();
            float distance = (float)std::sqrt(std::pow(x - ix, 2) + std::pow(y - iy, 2));
            if (distance < radius && entity_type_check(proper_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    };
    if (layer == LAYER::BOTH)
    {
        foreach_mask(mask, state->layers[0], push_entities_at);
        foreach_mask(mask, state->layers[1], push_entities_at);
    }
    else
    {
        foreach_mask(mask, state->layers[enum_to_layer(layer)], push_entities_at);
    }
    return found;
}

std::vector<uint32_t> get_entities_at(ENT_TYPE entity_type, uint32_t mask, float x, float y, LAYER layer, float radius)
{
    return get_entities_at(std::vector<ENT_TYPE>{entity_type}, mask, x, y, layer, radius);
}

std::vector<uint32_t> get_entities_overlapping_hitbox(std::vector<ENT_TYPE> entity_types, uint32_t mask, AABB hitbox, LAYER layer)
{
    // TODO: use entity regions?
    StateMemory* state = State::ptr();
    std::vector<uint32_t> result;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
    if (layer == LAYER::BOTH)
    {
        std::vector<uint32_t> result2;
        result = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state->layers[0]);
        result2 = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state->layers[1]);
        result.insert(result.end(), result2.begin(), result2.end());
    }
    else
    {
        uint8_t actual_layer = enum_to_layer(layer);
        result = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state->layers[actual_layer]);
    }
    return result;
}
std::vector<uint32_t> get_entities_overlapping_hitbox(ENT_TYPE entity_type, uint32_t mask, AABB hitbox, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, hitbox, layer);
}

std::vector<uint32_t> get_entities_overlapping(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::move(entity_types), mask, {sx, sy2, sx2, sy}, layer);
}
std::vector<uint32_t> get_entities_overlapping(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, LAYER layer)
{
    return get_entities_overlapping_hitbox(std::vector<ENT_TYPE>{entity_type}, mask, {sx, sy2, sx2, sy}, layer);
}

std::vector<uint32_t> get_entities_overlapping_by_pointer(std::vector<ENT_TYPE> entity_types, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    std::vector<uint32_t> found;
    foreach_mask(mask, layer, [&entity_types, &found, &sx, &sy, &sx2, &sy2](const EntityList& entities)
                 {
                     for (auto& item : entities.entities())
                     {
                         if (entity_type_check(std::move(entity_types), item->type->id) && item->overlaps_with(sx, sy, sx2, sy2))
                         {
                             found.push_back(item->uid);
                         }
                     } });

    return found;
}
std::vector<uint32_t> get_entities_overlapping_by_pointer(ENT_TYPE entity_type, uint32_t mask, float sx, float sy, float sx2, float sy2, Layer* layer)
{
    return get_entities_overlapping_by_pointer(std::vector<ENT_TYPE>{entity_type}, mask, sx, sy, sx2, sy2, layer);
}

bool entity_has_item_uid(uint32_t uid, uint32_t item_uid)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;

    return entity->items.contains(item_uid);
};

bool entity_has_item_type(uint32_t uid, std::vector<ENT_TYPE> entity_types)
{
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return false;
    if (entity->items.size > 0)
    {
        const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
        for (auto item : entity->items.entities())
        {
            if (entity_type_check(proper_types, item->type->id))
                return true;
        }
    }
    return false;
}
bool entity_has_item_type(uint32_t uid, ENT_TYPE entity_type)
{
    return entity_has_item_type(uid, std::vector<ENT_TYPE>{entity_type});
}

std::vector<uint32_t> entity_get_items_by(uint32_t uid, std::vector<ENT_TYPE> entity_types, uint32_t mask)
{
    std::vector<uint32_t> found;
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return found;
    if (entity->items.size > 0)
    {
        const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
        if ((!proper_types.size() || !proper_types[0]) && !mask) // all items
        {
            const auto uids = entity->items.uids();
            found.insert(found.end(), uids.begin(), uids.end());
        }
        else
        {
            for (auto item : entity->items.entities())
            {
                if ((mask == 0 || (item->type->search_flags & mask)) && entity_type_check(proper_types, item->type->id))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    return found;
}
std::vector<uint32_t> entity_get_items_by(uint32_t uid, ENT_TYPE entity_type, uint32_t mask)
{
    return entity_get_items_by(uid, std::vector<ENT_TYPE>{entity_type}, mask);
}

std::vector<uint32_t> get_entities_by_draw_depth(uint8_t draw_depth, LAYER l)
{
    return get_entities_by_draw_depth(std::vector<uint8_t>{draw_depth}, l);
}

std::vector<uint32_t> get_entities_by_draw_depth(std::vector<uint8_t> draw_depths, LAYER l)
{
    auto state = State::ptr_local();
    std::vector<uint32_t> found;
    auto actual_layer = enum_to_layer(l);
    for (auto draw_depth : draw_depths)
    {
        if (draw_depth > 52)
            continue;

        auto uids_layer1 = state->layers[actual_layer]->entities_by_draw_depth[draw_depth].uids();
        found.insert(found.end(), uids_layer1.begin(), uids_layer1.end());

        if (l == LAYER::BOTH) // if it's both, then the actual_layer is 0
        {
            auto uids_layer2 = state->layers[1]->entities_by_draw_depth[draw_depth].uids();
            found.insert(found.end(), uids_layer2.begin(), uids_layer2.end());
        }
    }
    return found;
}
