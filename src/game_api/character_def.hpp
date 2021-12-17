#pragma once

#include "entity.hpp"

#include <cstdint>
#include <string_view>

namespace NCharacterDB
{
std::uint32_t get_character_index(std::uint32_t entity_type);

const char16_t* get_character_full_name(std::uint32_t character_index);
const char16_t* get_character_short_name(std::uint32_t character_index);
Color get_character_heart_color(std::uint32_t character_index);
bool get_character_gender(std::uint32_t character_index);

void set_character_full_name(std::uint32_t character_index, std::u16string name);
void set_character_short_name(std::uint32_t character_index, std::u16string name);
void set_character_heart_color(std::uint32_t character_index, Color color);
void set_character_gender(std::uint32_t character_index, bool female);
} // namespace NCharacterDB
