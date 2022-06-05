#pragma once

#include "custom_allocator.hpp"

#include <unordered_map>

template <class K, class V>
using custom_unordered_map = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, custom_allocator<std::pair<const K, V>>>;
