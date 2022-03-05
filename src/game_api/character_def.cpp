#include "character_def.hpp"

#include "memory.hpp"
#include "strings.hpp"

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
    STRINGID full_name_string_id;
    STRINGID short_name_string_id;
    std::uint32_t _uint0;
    float bed_position_x;
    float bed_position_y;
    SomeCharEnum some_enum;
};
static constexpr auto sizeof_CharacterDB = sizeof(CharacterDB);
static constexpr auto expect_CharacterDB = 0x2c;
static_assert(sizeof_CharacterDB == expect_CharacterDB);

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

namespace NCharacterDB
{
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
    auto& char_def = get_character_definition(character_index);
    return get_string(char_def.full_name_string_id);
}
const char16_t* get_character_short_name(std::uint32_t character_index)
{
    auto& char_def = get_character_definition(character_index);
    return get_string(char_def.short_name_string_id);
}
Color get_character_heart_color(std::uint32_t character_index)
{
    auto& char_def = get_character_definition(character_index);
    return char_def.heart_color;
}
bool get_character_gender(std::uint32_t character_index)
{
    auto& char_def = get_character_definition(character_index);
    return char_def.gender == CharGender::Female;
}
void set_character_full_name(std::uint32_t character_index, std::u16string_view name)
{
    auto& char_def = get_character_definition(character_index);
    change_string(char_def.full_name_string_id, name);
}
void set_character_short_name(std::uint32_t character_index, std::u16string_view name)
{
    auto& char_def = get_character_definition(character_index);
    change_string(char_def.short_name_string_id, name);
}
void set_character_heart_color(std::uint32_t character_index, Color color)
{
    auto& char_def = get_character_definition(character_index);
    write_mem_prot(&char_def.heart_color, color, true);
}
void set_character_gender(std::uint32_t character_index, bool female)
{
    auto& char_def = get_character_definition(character_index);
    write_mem_prot(&char_def.gender, female ? CharGender::Female : CharGender::Male, true);
}
} // namespace NCharacterDB
