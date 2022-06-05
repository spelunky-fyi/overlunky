#pragma once

#include "game_allocator.hpp"

#include <map>

template <class K, class V, class Pred = std::less<K>>
using game_map = std::map<K, V, Pred, game_allocator<std::pair<const K, V>>>;
