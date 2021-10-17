#include "aliases.hpp"
#include "game_allocator.hpp"
#include "memory.hpp"

#include <string>
#include <string_view>

void strings_init();
const char16_t** get_strings_table();
STRINGID hash_to_stringid(uint32_t hash);
const char16_t* get_string(STRINGID string_id);
void change_string(STRINGID string_id, const std::u16string str);
STRINGID add_string(const std::u16string str);
