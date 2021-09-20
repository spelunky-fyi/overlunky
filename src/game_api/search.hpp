#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

size_t decode_pc(char* exe, size_t offset, uint8_t opcode_offset = 3);
size_t decode_imm(char* exe, size_t offset, uint8_t opcode_offset = 3);

// Find the location of the instruction (needle) with wildcard support
size_t find_inst(char* exe, std::string_view needle, size_t start);

size_t find_after_bundle(size_t exe);
