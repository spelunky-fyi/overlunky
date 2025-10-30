#include "entity_lookup.hpp"

#include <cstdint>
#include <vector>

#include "custom_types.hpp"
#include "entity.hpp"
#include "layer.hpp"
#include "state.hpp"

bool entity_type_check(const std::vector<ENT_TYPE>& types_array, const ENT_TYPE find)
{
    return (types_array.empty() || types_array[0] == 0 || std::find(types_array.begin(), types_array.end(), find) != types_array.end());
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
    if (Entity* ent = get_state_ptr()->layer(layer)->get_grid_entity_at(x, y))
        return ent->uid;

    return -1;
}

std::vector<uint32_t> get_entities_overlapping_grid(float x, float y, LAYER layer)
{
    auto state = get_state_ptr();
    std::vector<uint32_t> uids;
    auto entities = state->layer(layer)->get_entities_overlapping_grid_at(x, y);
    if (entities)
        uids.insert(uids.end(), entities->uids().begin(), entities->uids().end());
    if (layer == LAYER::BOTH)
    {
        // enum_to_layer returns 0 for LAYER::BOTH, so we only need to add entities from second layer
        auto entities2 = state->layers[1]->get_entities_overlapping_grid_at(x, y);
        if (entities2)
            uids.insert(uids.end(), entities2->uids().begin(), entities2->uids().end());
    }
    return uids;
}

template <class FunT>
requires std::is_invocable_v<FunT, const EntityList&>
void foreach_mask(ENTITY_MASK mask, Layer* l, FunT&& fun)
{
    if (mask == ENTITY_MASK::ANY)
    {
        fun(l->all_entities);
    }
    else
    {
        for (uint32_t test_flag = 1U; test_flag < 0x8000; test_flag <<= 1U)
        {
            if (!(mask & static_cast<ENTITY_MASK>(test_flag)))
                continue;

            const auto& it = l->entities_by_mask.find(static_cast<ENTITY_MASK>(test_flag));
            if (it != l->entities_by_mask.end())
                fun(it->second);
        }
    }
}

std::vector<uint32_t> get_entities_by(std::vector<ENT_TYPE> entity_types, ENTITY_MASK mask, LAYER layer)
{
    auto state = get_state_ptr();
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
            if (mask == ENTITY_MASK::ANY) // all entities
            {
                // this exception for small improvements with calling reserve once
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
        if (proper_types.empty() || proper_types[0] == 0) // all types
        {
            foreach_mask(mask, state->layer(layer), insert_all_uids);
        }
        else
        {
            foreach_mask(mask, state->layer(layer), push_matching_types);
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_at(std::vector<ENT_TYPE> entity_types, ENTITY_MASK mask, float x, float y, LAYER layer, float radius)
{
    // TODO: use entity regions?
    auto state = get_state_ptr();
    std::vector<uint32_t> found;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
    auto push_entities_at = [&x, &y, &radius, &proper_types, &found](const EntityList& entities)
    {
        for (auto& item : entities.entities())
        {
            auto [ix, iy] = item->abs_position();
            float distance = (float)std::sqrt(std::pow(x - ix, 2) + std::pow(y - iy, 2));
            if (distance < radius && entity_type_check(proper_types, item->type->id))
            {
                found.push_back(item->uid);
            }
        }
    };
    foreach_mask(mask, state->layer(layer), push_entities_at);
    if (layer == LAYER::BOTH)
    {
        // if it's both, then the actual_layer is 0
        foreach_mask(mask, state->layers[1], push_entities_at);
    }
    return found;
}

std::vector<uint32_t> get_entities_overlapping_hitbox(std::vector<ENT_TYPE> entity_types, ENTITY_MASK mask, AABB hitbox, LAYER layer)
{
    // TODO: use entity regions?
    auto state = get_state_ptr();
    std::vector<uint32_t> result;
    const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));

    result = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state->layer(layer));

    if (layer == LAYER::BOTH)
    {
        // if it's both, then the actual_layer is 0
        auto result2 = get_entities_overlapping_by_pointer(proper_types, mask, hitbox.left, hitbox.bottom, hitbox.right, hitbox.top, state->layers[1]);
        result.insert(result.end(), result2.begin(), result2.end());
    }
    return result;
}

std::vector<uint32_t> get_entities_overlapping_by_pointer(std::vector<ENT_TYPE> entity_types, ENTITY_MASK mask, float sx, float sy, float sx2, float sy2, Layer* layer)
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

std::vector<uint32_t> entity_get_items_by(uint32_t uid, std::vector<ENT_TYPE> entity_types, ENTITY_MASK mask)
{
    std::vector<uint32_t> found;
    Entity* entity = get_entity_ptr(uid);
    if (entity == nullptr)
        return found;
    if (entity->items.size > 0)
    {
        const std::vector<ENT_TYPE> proper_types = get_proper_types(std::move(entity_types));
        if ((!proper_types.size() || !proper_types[0]) && mask == ENTITY_MASK::ANY) // all items
        {
            const auto uids = entity->items.uids();
            found.insert(found.end(), uids.begin(), uids.end());
        }
        else
        {
            for (auto item : entity->items.entities())
            {
                if ((mask == ENTITY_MASK::ANY || !!(item->type->search_flags & mask)) && entity_type_check(proper_types, item->type->id))
                {
                    found.push_back(item->uid);
                }
            }
        }
    }
    return found;
}

std::vector<uint32_t> get_entities_by_draw_depth(std::vector<uint8_t> draw_depths, LAYER l)
{
    auto state = get_state_ptr();
    std::vector<uint32_t> found;
    for (auto draw_depth : draw_depths)
    {
        if (draw_depth > 52)
            continue;

        auto uids_layer1 = state->layer(l)->entities_by_draw_depth[draw_depth].uids();
        found.insert(found.end(), uids_layer1.begin(), uids_layer1.end());

        if (l == LAYER::BOTH)
        {
            // if it's both, then the actual_layer is 0
            auto uids_layer2 = state->layers[1]->entities_by_draw_depth[draw_depth].uids();
            found.insert(found.end(), uids_layer2.begin(), uids_layer2.end());
        }
    }
    return found;
}
