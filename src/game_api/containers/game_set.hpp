#pragma once

#include "game_allocator.hpp"

#include <set>

template <class V, class Pred = std::less<V>>
using game_set = std::set<V, Pred, game_allocator<V>>;
