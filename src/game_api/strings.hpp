#pragma once
#include "aliases.hpp"

#include <string>
#include <string_view>

extern const std::map<uint32_t, STRINGID> string_hashes;

void strings_init();
const char16_t** get_strings_table();
STRINGID hash_to_stringid(uint32_t hash);
const char16_t* get_string(STRINGID string_id);
void change_string(STRINGID string_id, std::u16string_view str);
STRINGID add_string(std::u16string str);
void clear_custom_shopitem_names();
void add_custom_name(uint32_t uid, std::u16string name);
void clear_custom_name(uint32_t uid);
STRINGID pointer_to_stringid(size_t ptr);
