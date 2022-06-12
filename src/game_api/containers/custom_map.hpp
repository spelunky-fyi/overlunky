#pragma once

#include "custom_allocator.hpp"

#include <map>

template <class K, class V, class Pred = std::less<K>>
using custom_map = std::map<K, V, Pred, custom_allocator<std::pair<const K, V>>>;
