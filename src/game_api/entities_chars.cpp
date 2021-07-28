#include "entities_chars.hpp"
#include "character_def.hpp"

void PowerupCapable::remove_powerup(uint32_t powerup_type)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        std::string pattern = "\x48\x8B\xCB\xFF\x90\xF0\x02\x00\x00\x48\x8B\x5C\x24\x30"s;
        offset = function_start(memory.at_exe(find_inst(exe, pattern, memory.after_bundle)));
    }

    if (offset != 0)
    {
        typedef void internal_remove_powerup_func(PowerupCapable*, uint32_t);
        static internal_remove_powerup_func* irpf = (internal_remove_powerup_func*)(offset);
        irpf(this, powerup_type);
    }
}

void PowerupCapable::give_powerup(uint32_t powerup_type)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        std::string pattern = "\x83\x8A\xF4\x09\x00\x00\x40"s;
        offset = function_start(memory.at_exe(find_inst(exe, pattern, memory.after_bundle)));
    }

    if (offset != 0)
    {
        typedef void internal_give_powerup_func(PowerupCapable*, uint32_t);
        static internal_give_powerup_func* igpf = (internal_give_powerup_func*)(offset);
        igpf(this, powerup_type);
    }
}

bool PowerupCapable::has_powerup(uint32_t powerup_type)
{
    return powerups.find(powerup_type) != powerups.end();
}

std::u16string Player::get_name()
{
    return ::get_character_name(type->id);
}
std::u16string Player::get_short_name()
{
    return ::get_character_short_name(type->id);
}
Color Player::get_heart_color()
{
    return ::get_character_heart_color(type->id);
}
bool Player::is_female()
{
    return ::is_character_female(type->id);
}

void Player::set_heart_color(Color color)
{
    ::set_character_heart_color(type->id, color);
}

std::u16string get_character_name(int32_t type_id)
{
    return get_character_full_name(get_character_index(type_id));
}
std::u16string get_character_short_name(int32_t type_id)
{
    return get_character_short_name(get_character_index(type_id));
}
Color get_character_heart_color(int32_t type_id)
{
    return get_character_heart_color(get_character_index(type_id));
}
bool is_character_female(int32_t type_id)
{
    return get_character_gender(get_character_index(type_id));
}

void set_character_heart_color(int32_t type_id, Color color)
{
    set_character_heart_color(get_character_index(type_id), color);
}
