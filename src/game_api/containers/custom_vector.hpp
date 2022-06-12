#pragma once

#include "custom_allocator.hpp"

#include <vector>

template <class T>
using custom_vector = std::vector<T, custom_allocator<T>>;
