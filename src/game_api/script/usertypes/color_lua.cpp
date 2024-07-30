#include "color_lua.hpp"

#include <cstdint>
#include <sol/sol.hpp>

#include "aliases.hpp"
#include "color.hpp"
#include "script/sol_helper.hpp" // for self_return

namespace NColor
{
void register_usertypes(sol::state& lua)
{
    /// Converts a color to int to be used in drawing functions. Use values from `0..255`.
    lua["rgba"] = [](int r, int g, int b, int a) -> uColor
    {
        return (uColor)(a << 24) + (b << 16) + (g << 8) + (r);
    };

    lua["get_color"] = get_color;

    auto color_type = lua.new_usertype<Color>("Color", sol::constructors<Color(), Color(const Color&), Color(float, float, float, float), Color(const std::string&, std::optional<uint8_t>)>{}, //
                                              sol::meta_function::equal_to,
                                              &Color::operator==);

    color_type["r"] = &Color::r;
    color_type["g"] = &Color::g;
    color_type["b"] = &Color::b;
    color_type["a"] = &Color::a;
    color_type["white"] = &Color::white;
    color_type["silver"] = &Color::silver;
    color_type["gray"] = &Color::gray;
    color_type["black"] = &Color::black;
    color_type["red"] = &Color::red;
    color_type["maroon"] = &Color::maroon;
    color_type["yellow"] = &Color::yellow;
    color_type["olive"] = &Color::olive;
    color_type["lime"] = &Color::lime;
    color_type["green"] = &Color::green;
    color_type["aqua"] = &Color::aqua;
    color_type["teal"] = &Color::teal;
    color_type["blue"] = &Color::blue;
    color_type["navy"] = &Color::navy;
    color_type["fuchsia"] = &Color::fuchsia;
    color_type["purple"] = &Color::purple;
    color_type["get_rgba"] = &Color::get_rgba;
    color_type["set_rgba"] = self_return<&Color::set_rgba>();
    color_type["get_ucolor"] = &Color::get_ucolor;
    color_type["set_ucolor"] = self_return<&Color::set_ucolor>();
    color_type["set"] = self_return<&Color::set>();
}
} // namespace NColor
