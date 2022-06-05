#pragma once

#include "custom_allocator.hpp"

#include <string>

using custom_string = std::basic_string<char, std::char_traits<char>, custom_allocator<char>>;
