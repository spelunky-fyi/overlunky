#pragma once

#include "game_allocator.hpp"

#include <unordered_map>

template <class K, class V, class Hasher = std::hash<K>, class Equal = std::equal_to<K>>
using game_unordered_map = std::unordered_map<K, V, Hasher, Equal, game_allocator<std::pair<const K, V>>>;
