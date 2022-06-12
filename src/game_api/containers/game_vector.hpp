#pragma once

#include "game_allocator.hpp"

#include <vector>

template <class T>
using game_vector = std::vector<T, game_allocator<T>>;
