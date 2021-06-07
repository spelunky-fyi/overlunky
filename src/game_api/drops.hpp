#pragma once

#include "virtual_table.hpp"

#include <cstdint>
#include <string>
#include <vector>

struct DropEntry
{
    std::string caption;
    std::string pattern;
    VTABLE_OFFSET vtable_offset; // some patterns are not found in the vtables, use NONE to look for pattern in entire exe
    uint32_t vtable_rel_offset;
    uint8_t value_offset = 0;      // the offset of the value to be replaced within the pattern
    uint8_t vtable_occurrence = 1; // when a value occurs more than once in the same virtual table function, choose how many times to replace
    size_t offsets[3] = {0};
};

struct DropChanceEntry
{
    std::string caption;
    std::string pattern;
    VTABLE_OFFSET vtable_offset;
    uint32_t vtable_rel_offset;
    uint8_t chance_sizeof = 4;
    size_t offset = 0;
};

extern std::vector<DropEntry> drop_entries;

extern std::vector<DropChanceEntry> dropchance_entries;
