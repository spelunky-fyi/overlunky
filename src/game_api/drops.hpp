#pragma once

#include <cstddef> // for size_t
#include <cstdint> // for uint32_t, uint8_t, int32_t
#include <string>  // for string, allocator
#include <vector>  // for vector

#include "aliases.hpp"       // for ENT_TYPE
#include "virtual_table.hpp" // for VTABLE_OFFSET

struct DropEntry
{
    std::string caption;
    std::string_view pattern;
    VTABLE_OFFSET vtable_offset; // some patterns are not found in the vtables, use NONE to look for pattern in entire exe
    uint32_t vtable_rel_offset;
    uint8_t value_offset = 0;      // the offset of the value to be replaced within the pattern
    uint8_t vtable_occurrence = 1; // when a value occurs more than once in the same virtual table function, choose how many times to replace
    size_t offsets[3] = {0};
};

struct DropChanceEntry
{
    std::string caption;
    std::string_view pattern;
    VTABLE_OFFSET vtable_offset;
    uint32_t vtable_rel_offset;
    uint8_t chance_sizeof = 4;
    size_t offset = 0;
};
using DROPCHANCE = int32_t;
void set_drop_chance(DROPCHANCE dropchance_id, uint32_t new_drop_chance);
using DROP = int32_t;
void replace_drop(DROP drop_id, ENT_TYPE new_drop_entity_type);
void replace_drop_by_name(std::string_view name, ENT_TYPE new_drop_entity_type);

extern std::vector<DropEntry> drop_entries;

extern std::vector<DropChanceEntry> dropchance_entries;

// #define PERFORM_DROPS_TEST
#ifdef PERFORM_DROPS_TEST
void test_drops();
#endif
