#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include "rust/cxx.h"

enum RepeatType : uint8_t
{
    NoRepeat,
    Linear,
    BackAndForth,
};

struct Animation
{
    int32_t texture;
    int32_t count;
    int32_t interval;
    uint8_t key;
    RepeatType repeat;
};

/*
struct EntityInstance;

// Creates an instance of this entity
using EntityCreate = EntityInstance *(*)();
using EntityDestroy = void (*)(Entity *);
using AnimationMap = std::unordered_map<uint8_t, Animation>;
*/

struct EntityItem;
rust::Vec<EntityItem> list_entities(size_t map_ptr);
