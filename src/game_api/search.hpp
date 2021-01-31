#pragma once
#include <Windows.h>

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <algorithm>

namespace {
    size_t decode_pc(char *exe, size_t offset) {
        off_t rel = *(int32_t *) (&exe[offset + 3]);
        return offset + rel + 7;
    }

    size_t decode_imm(char *exe, size_t offset) {
        return *(uint32_t *) (&exe[offset + 3]);
    }

    size_t find_inst(char *exe, std::string needle, size_t start) {
        // Find the location of the instruction (needle) using memmem()
        return (char *)std::search(&exe[start], exe + 0x1000000000, needle.begin(), needle.end()) - exe;
    }

    size_t find_after_bundle(size_t exe) {
        auto offset = 0x1000;

        while (true) {
            uint32_t *cur = (uint32_t *) (exe + offset);
            uint32_t l0 = cur[0], l1 = cur[1];
            if (l0 == 0 && l1 == 0) {
                break;
            }
            offset += (8 + l0 + l1);
        }

        uint8_t delim[] = {0x48, 0x81, 0xEC, 0xE8, 0x00, 0x00, 0x00};
        std::string delim_s = std::string((char *) delim, sizeof(delim));
        return find_inst((char *)exe, delim_s, offset);
    }
}; // namespace