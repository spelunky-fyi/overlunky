#pragma once

#include <cstddef>     // for size_t
#include <cstdint>     // for uint8_t
#include <optional>    // for optional, nullopt
#include <string>      // for string
#include <string_view> // for operator""sv, string_view, string_view_literals

using namespace std::string_view_literals;

size_t decode_pc(const char* exe, size_t offset, uint8_t opcode_offset = 3, uint8_t opcode_suffix_offset = 0, uint8_t opcode_addr_size = 4);
size_t decode_imm(const char* exe, size_t offset, uint8_t opcode_offset = 3);

// Find the location of the instruction (needle) with wildcard (* or \x2a) support
// Optional pattern_name for better error messages
// If is_required is true the function will call std::terminate when the needle can't be found
// Else it will throw std::logic_error
size_t find_inst(const char* exe, std::string_view needle, size_t start, std::optional<size_t> end = std::nullopt, std::string_view pattern_name = ""sv, bool is_required = true);

size_t find_after_bundle(size_t exe);

void preload_addresses();
size_t get_address(std::string_view address_name);

void register_application_version(std::string s);
