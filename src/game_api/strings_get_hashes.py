import re

import sys

sys.stdout = open("string_hashes.cpp", "w")

strings_path = "../../docs/game_data/strings00_hashed.str"

print(
    """
// THIS FILE IS AUTO-GENERATED
// If you need to make changes it please change strings_get_hashes.py

#include <cstdint>    // for uint32_t
#include <functional> // for less
#include <map>        // for allocator, map
#include <new>        // for operator new
#include <utility>    // for min

#include "aliases.hpp" // for STRINGID
#include "strings.hpp"

const std::map<uint32_t, STRINGID> string_hashes = {"""
)

data = open(strings_path, "r").read().split("\n")
line_number = 0
for line in data:
    if line == "":
        continue
    if line[0] == "#":
        continue
    print("    {" + line[0:10] + ", " + str(line_number) + "},")
    line_number += 1

print(
    """
};

const std::map<uint32_t, STRINGID>& get_string_hashes()
{
    return string_hashes;
}"""
)
