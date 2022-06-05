#pragma once

#include "custom_allocator.hpp"

#include <unordered_set>

template <class V>
using custom_unordered_set = std::unordered_set<V, std::hash<V>, std::equal_to<V>, custom_allocator<V>>;
