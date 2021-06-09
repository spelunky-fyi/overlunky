#include "character_def.hpp"

#include "logger.h"
#include "memory.hpp"

#include <array>
#include <cassert>
#include <span>

enum class CharGender
{
    Male,
    Female,
};
enum class SomeCharEnum
{
    Variant_1,
    Variant_2,
    Variant_3,
};
struct CharacterDefinition
{
    CharGender gender;
    Color heart_color;
    char16_t full_name[0x18];
    char16_t short_name[0xc];
    std::uint32_t _uint0;
    float bed_position_x;
    float bed_position_y;
    SomeCharEnum some_enum;
};
static constexpr auto sizeof_CharacterDefinition = sizeof(CharacterDefinition);
static constexpr auto expect_CharacterDefinition = 0x36 * sizeof(char16_t);
static_assert(sizeof_CharacterDefinition == expect_CharacterDefinition);

std::span<CharacterDefinition> GetCharacterDefinitions()
{
    static std::span<CharacterDefinition> static_character_table = []()
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        const auto static_init_strings = find_inst(exe, "\x48\x8b\xc4\x48\x81\xec\xe8\x00\x00\x00\x0f\x29\x70\xe8\x0f\x29\x78\xd8\x44\x0f\x29\x40\xc8\x44\x0f\x29\x48\xb8\x44\x0f\x29\x50\xa8"s, after_bundle);
        const auto load_ana_strings = find_inst(exe, "\xf3\x44\x0f\x10\xc8"s, static_init_strings);
        // load_ana_strings = (char*)load_ana_strings + 12;
        const auto write_ana_strings = find_inst(exe, "\x0f\x11"s, load_ana_strings);

        auto ana_strings = memory.at_exe(decode_pc(exe, write_ana_strings));
        static constexpr auto start_offset = sizeof(CharacterDefinition::short_name) + 0x10;
        auto string_table_first_element = ana_strings + sizeof(CharacterDefinition::full_name) + start_offset;
        auto string_table_start = string_table_first_element - sizeof(CharacterDefinition);
        std::span<CharacterDefinition> character_table{(CharacterDefinition*)(string_table_start), 20};
        [[maybe_unused]] constexpr std::array known_tables{
            CharacterDefinition{.full_name{u"Ana Spelunky"}, .short_name{u"Ana"}},
            CharacterDefinition{.full_name{u"Margaret Tunnel"}, .short_name{u"Margaret"}},
            CharacterDefinition{.full_name{u"Colin Northward"}, .short_name{u"Colin"}},
            CharacterDefinition{.full_name{u"Roffy D. Sloth"}, .short_name{u"Roffy"}},
            CharacterDefinition{.full_name{u"Alto Singh"}, .short_name{u"Alto"}},
            CharacterDefinition{.full_name{u"Liz Mutton"}, .short_name{u"Liz"}},
            CharacterDefinition{.full_name{u"Nekka The Eagle"}, .short_name{u"Nekka"}},
            CharacterDefinition{.full_name{u"LISE Project"}, .short_name{u"LISE"}},
            CharacterDefinition{.full_name{u"Coco Von Diamonds"}, .short_name{u"Coco"}},
            CharacterDefinition{.full_name{u"Manfred Tunnel"}, .short_name{u"Manfred"}},
            CharacterDefinition{.full_name{u"Little Jay"}, .short_name{u"Jay"}},
            CharacterDefinition{.full_name{u"Tina Flan"}, .short_name{u"Tina"}},
            CharacterDefinition{.full_name{u"Valerie Crump"}, .short_name{u"Valerie"}},
            CharacterDefinition{.full_name{u"Au"}, .short_name{u"Au"}},
            CharacterDefinition{.full_name{u"Demi Von Diamonds"}, .short_name{u"Demi"}},
            CharacterDefinition{.full_name{u"Pilot"}, .short_name{u"Pilot"}},
            CharacterDefinition{.full_name{u"Princess Airyn"}, .short_name{u"Airyn"}},
            CharacterDefinition{.full_name{u"Dirk Yamaoka"}, .short_name{u"Dirk"}},
            CharacterDefinition{.full_name{u"Guy Spelunky"}, .short_name{u"Guy"}},
            CharacterDefinition{.full_name{u"Classic Guy"}, .short_name{u"Classic Guy"}},
        };
        for (size_t i = 0; i < 20; i++)
        {
            [[maybe_unused]] CharacterDefinition& string_table = character_table[i];
            [[maybe_unused]] const CharacterDefinition& known_table = known_tables[i];
            assert(memcmp(string_table.full_name, known_table.full_name, sizeof(CharacterDefinition::full_name)) == 0);
            assert(memcmp(string_table.short_name, known_table.short_name, sizeof(CharacterDefinition::short_name)) == 0);
        }

        return character_table;
    }();
    return static_character_table;
}

CharacterDefinition& get_character_definition(std::uint32_t character_index)
{
    return GetCharacterDefinitions()[std::clamp(character_index, 0u, 19u)];
}

std::uint32_t get_character_index(std::uint32_t entity_type)
{
    return (std::uint32_t)std::clamp((int)entity_type - 194, 0, 19);
}

const char16_t* get_character_full_name(std::uint32_t character_index)
{
    return get_character_definition(character_index).full_name;
}
const char16_t* get_character_short_name(std::uint32_t character_index)
{
    return get_character_definition(character_index).short_name;
}
Color get_character_heart_color(std::uint32_t character_index)
{
    return get_character_definition(character_index).heart_color;
}
bool get_character_gender(std::uint32_t character_index)
{
    return get_character_definition(character_index).gender != CharGender::Female;
}

void set_character_full_name(std::uint32_t character_index, std::u16string_view name)
{
    const auto max_size = sizeof(CharacterDefinition::full_name) / sizeof(char16_t);
    if (name.size() > max_size)
    {
        DEBUG("Character name is too long, max supported size is {}", max_size);
    }
    auto& char_def = get_character_definition(character_index);
    memset(char_def.full_name, 0, max_size * sizeof(char16_t));
    memcpy(char_def.full_name, name.data(), std::min(name.size(), max_size) * sizeof(char16_t));
}
void set_character_short_name(std::uint32_t character_index, std::u16string_view name)
{
    const auto max_size = sizeof(CharacterDefinition::short_name) / sizeof(char16_t);
    if (name.size() > max_size)
    {
        DEBUG("Character name is too long, max supported size is {}", max_size);
    }
    auto& char_def = get_character_definition(character_index);
    memset(char_def.short_name, 0, max_size * sizeof(char16_t));
    memcpy(char_def.short_name, name.data(), std::min(name.size(), max_size) * sizeof(char16_t));
}
void set_character_heart_color(std::uint32_t character_index, Color color)
{
    get_character_definition(character_index).heart_color = color;
}
void set_character_gender(std::uint32_t character_index, bool female)
{
    get_character_definition(character_index).gender = female ? CharGender::Female : CharGender::Male;
}
