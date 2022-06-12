#pragma once

#include "custom_allocator.hpp"

#include <set>

template <class V, class Pred = std::less<V>>
using custom_set = std::set<V, Pred, custom_allocator<V>>;
