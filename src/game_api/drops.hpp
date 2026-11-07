#pragma once

#include <cstddef>     // for size_t
#include <cstdint>     // for uint32_t, uint8_t, int32_t
#include <span>        // for span
#include <string_view> // for string_view

#include "aliases.hpp"       // for ENT_TYPE
#include "virtual_table.hpp" // for VTABLE_OFFSET

namespace Drops
{
constexpr const auto MAX_OCCURRENCE = 3u;
};

struct DropEntry
{
    std::string_view caption;
    std::string_view pattern;
    VTABLE_OFFSET vtable_offset; // some patterns are not found in the vtables, use NONE to look for pattern in entire exe
    uint32_t vtable_rel_offset;
    uint8_t value_offset;      // the offset of the value to be replaced within the pattern
    uint8_t vtable_occurrence; // when a value occurs more than once in the same virtual table function, choose how many times to replace

    consteval DropEntry(std::string_view cap, std::string_view pat, VTABLE_OFFSET v_off, uint32_t v_rel, uint8_t val_off = 0, uint8_t v_occ = 1)
        : caption(cap), pattern(pat), vtable_offset(v_off), vtable_rel_offset(v_rel), value_offset(val_off), vtable_occurrence(v_occ)
    {
        if (vtable_occurrence < 1 || vtable_occurrence > Drops::MAX_OCCURRENCE)
            throw std::out_of_range("vtable_occurrence must be between 1 and MAX_OCCURRENCE");
    }
};

struct DropChanceEntry
{
    std::string_view caption;
    std::string_view pattern;
    VTABLE_OFFSET vtable_offset;
    uint32_t vtable_rel_offset;
    uint8_t chance_sizeof;
    consteval DropChanceEntry(std::string_view cap, std::string_view pat, VTABLE_OFFSET v_offset, uint32_t v_rel_offset, uint8_t c_sizeof = 4)
        : caption(cap), pattern(pat), vtable_offset(v_offset), vtable_rel_offset(v_rel_offset), chance_sizeof(c_sizeof)
    {
        if (chance_sizeof != 4 && chance_sizeof != 1)
            throw std::out_of_range("chance_sizeof must be 1 or 4");
    }
};
void resolve_drop_addresses();

using DROPCHANCE = int32_t;
void set_drop_chance(DROPCHANCE dropchance_id, uint32_t new_drop_chance);
using DROP = int32_t;
void replace_drop(DROP drop_id, ENT_TYPE new_drop_entity_type);
void replace_drop_by_name(std::string_view name, ENT_TYPE new_drop_entity_type);
std::span<const DropEntry> get_drops();
std::span<const DropChanceEntry> get_chances();
