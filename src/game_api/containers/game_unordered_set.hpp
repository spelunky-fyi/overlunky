#pragma once

#include "game_allocator.hpp"

#include <unordered_set>

template <class V>
using game_unordered_set = std::unordered_set<V, std::hash<V>, std::equal_to<V>, game_allocator<V>>;
