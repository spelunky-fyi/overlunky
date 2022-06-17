#pragma once

#include "game_allocator.hpp"

#include <string>

using game_string = std::basic_string<char, std::char_traits<char>, game_allocator<char>>;
