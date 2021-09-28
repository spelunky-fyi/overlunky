#include "entities_chars.hpp"
#include "character_def.hpp"

void PowerupCapable::remove_powerup(ENT_TYPE powerup_type)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("remove_powerup");
    }

    if (offset != 0)
    {
        typedef void internal_remove_powerup_func(PowerupCapable*, uint32_t);
        static internal_remove_powerup_func* irpf = (internal_remove_powerup_func*)(offset);
        irpf(this, powerup_type);
    }
}

void PowerupCapable::give_powerup(ENT_TYPE powerup_type)
{
    static size_t offset = 0;
    if (offset == 0)
    {
        offset = get_address("give_powerup");
    }

    if (offset != 0)
    {
        typedef void internal_give_powerup_func(PowerupCapable*, uint32_t);
        static internal_give_powerup_func* igpf = (internal_give_powerup_func*)(offset);
        igpf(this, powerup_type);
    }
}

bool PowerupCapable::has_powerup(ENT_TYPE powerup_type)
{
    return powerups.find(powerup_type) != powerups.end();
}

std::u16string Player::get_name()
{
    return ::get_character_full_name(get_character_index(type->id));
}
std::u16string Player::get_short_name()
{
    return ::get_character_short_name(get_character_index(type->id));
}
Color Player::get_heart_color()
{
    return ::get_character_heart_color(get_character_index(type->id));
}
bool Player::is_female()
{
    return ::get_character_gender(get_character_index(type->id));
}

void Player::set_heart_color(Color hcolor)
{
    ::set_character_heart_color(get_character_index(type->id), hcolor);
}

std::u16string get_character_name(ENT_TYPE type_id)
{
    return get_character_full_name(get_character_index(type_id));
}
std::u16string get_character_short_name(ENT_TYPE type_id)
{
    return get_character_shortname(get_character_index(type_id));
}
Color get_character_heart_color(ENT_TYPE type_id)
{
    return get_character_heartcolor(get_character_index(type_id));
}
bool is_character_female(ENT_TYPE type_id)
{
    return get_character_gender(get_character_index(type_id));
}

void set_character_heart_color(ENT_TYPE type_id, Color color)
{
    set_character_heartcolor(get_character_index(type_id), color);
}
