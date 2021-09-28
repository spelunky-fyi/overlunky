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
struct CharacterDB
{
    CharGender gender;
    Color heart_color;
    uint32_t full_name_string_id;
    uint32_t short_name_string_id;
    std::uint32_t _uint0;
    float bed_position_x;
    float bed_position_y;
    SomeCharEnum some_enum;
};

std::span<CharacterDB> GetCharacterDefinitions()
{
    static std::span<CharacterDB> static_character_table = []()
    {
        const auto character_db = get_address("character_db");
        std::span<CharacterDB> character_table{(CharacterDB*)character_db, 20};

        struct KnownCharacter
        {
            std::u16string_view full_name;
            std::u16string_view short_name;
        };
        [[maybe_unused]] constexpr std::array known_tables{
            KnownCharacter{.full_name{u"Ana Spelunky"}, .short_name{u"Ana"}},
            KnownCharacter{.full_name{u"Margaret Tunnel"}, .short_name{u"Margaret"}},
            KnownCharacter{.full_name{u"Colin Northward"}, .short_name{u"Colin"}},
            KnownCharacter{.full_name{u"Roffy D. Sloth"}, .short_name{u"Roffy"}},
            KnownCharacter{.full_name{u"Alto Singh"}, .short_name{u"Alto"}},
            KnownCharacter{.full_name{u"Liz Mutton"}, .short_name{u"Liz"}},
            KnownCharacter{.full_name{u"Nekka The Eagle"}, .short_name{u"Nekka"}},
            KnownCharacter{.full_name{u"LISE Project"}, .short_name{u"LISE"}},
            KnownCharacter{.full_name{u"Coco Von Diamonds"}, .short_name{u"Coco"}},
            KnownCharacter{.full_name{u"Manfred Tunnel"}, .short_name{u"Manfred"}},
            KnownCharacter{.full_name{u"Little Jay"}, .short_name{u"Jay"}},
            KnownCharacter{.full_name{u"Tina Flan"}, .short_name{u"Tina"}},
            KnownCharacter{.full_name{u"Valerie Crump"}, .short_name{u"Valerie"}},
            KnownCharacter{.full_name{u"Au"}, .short_name{u"Au"}},
            KnownCharacter{.full_name{u"Demi Von Diamonds"}, .short_name{u"Demi"}},
            KnownCharacter{.full_name{u"Pilot"}, .short_name{u"Pilot"}},
            KnownCharacter{.full_name{u"Princess Airyn"}, .short_name{u"Airyn"}},
            KnownCharacter{.full_name{u"Dirk Yamaoka"}, .short_name{u"Dirk"}},
            KnownCharacter{.full_name{u"Guy Spelunky"}, .short_name{u"Guy"}},
            KnownCharacter{.full_name{u"Classic Guy"}, .short_name{u"Classic Guy"}},
        };

        [[maybe_unused]] const auto string_table = (const char16_t**)get_address("string_table");
        for (size_t i = 0; i < 20; i++)
        {
            [[maybe_unused]] const CharacterDB& character = character_table[i];
            [[maybe_unused]] const KnownCharacter& known_name = known_tables[i];
            assert(memcmp(string_table[character.full_name_string_id], known_name.full_name.data(), known_name.full_name.size()) == 0);
            assert(memcmp(string_table[character.short_name_string_id], known_name.short_name.data(), known_name.short_name.size()) == 0);
        }

        return character_table;
    }();
    return static_character_table;
}

CharacterDB& get_character_definition(std::uint32_t character_index)
{
    return GetCharacterDefinitions()[std::clamp(character_index, 0u, 19u)];
}

std::uint32_t get_character_index(std::uint32_t entity_type)
{
    return (std::uint32_t)std::clamp((int)entity_type - 194, 0, 19);
}

const char16_t* get_character_full_name(std::uint32_t character_index)
{
    static const auto string_table = (const char16_t**)get_address("string_table");
    return string_table[get_character_definition(character_index).full_name_string_id];
}
const char16_t* get_character_shortname(std::uint32_t character_index)
{
    static const auto string_table = (const char16_t**)get_address("string_table");
    return string_table[get_character_definition(character_index).short_name_string_id];
}
Color get_character_heartcolor(std::uint32_t character_index)
{
    return get_character_definition(character_index).heart_color;
}
bool get_character_gender(std::uint32_t character_index)
{
    return get_character_definition(character_index).gender == CharGender::Female;
}

void set_character_full_name(std::uint32_t /*character_index*/, std::u16string_view /*name*/)
{
    //const auto max_size = sizeof(CharacterDB::full_name) / sizeof(char16_t);
    //if (name.size() > max_size)
    //{
    //    DEBUG("Character name is too long, max supported size is {}", max_size);
    //}
    //auto& char_def = get_character_definition(character_index);
    //memset(char_def.full_name, 0, max_size * sizeof(char16_t));
    //memcpy(char_def.full_name, name.data(), std::min(name.size(), max_size) * sizeof(char16_t));
}
void set_character_short_name(std::uint32_t /*character_index*/, std::u16string_view /*name*/)
{
    //const auto max_size = sizeof(CharacterDB::short_name) / sizeof(char16_t);
    //if (name.size() > max_size)
    //{
    //    DEBUG("Character name is too long, max supported size is {}", max_size);
    //}
    //auto& char_def = get_character_definition(character_index);
    //memset(char_def.short_name, 0, max_size * sizeof(char16_t));
    //memcpy(char_def.short_name, name.data(), std::min(name.size(), max_size) * sizeof(char16_t));
}
void set_character_heartcolor(std::uint32_t character_index, Color color)
{
    get_character_definition(character_index).heart_color = color;
}
void set_character_gender(std::uint32_t character_index, bool female)
{
    get_character_definition(character_index).gender = female ? CharGender::Female : CharGender::Male;
}
