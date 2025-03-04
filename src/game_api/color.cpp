#include "color.hpp"

#include <charconv>

#include "crc32.hpp"

uColor get_color(const std::string& color_name, std::optional<uint8_t> alpha)
{
    // #RRGGBBAA
    // 0xAABBGGRR
    // treating non even number of hex characters as user error = behavior is undefined

    if (color_name.size() == 0)
        return 0;

    if (color_name[0] == '#' && color_name.size() > 6)
    {
        std::string hex_string{color_name.begin() + 1, color_name.end()}; // skip #
        uint32_t html_color = std::stoul(hex_string, nullptr, 16);
        if (color_name.size() < 8)
        {
            html_color <<= 8;
            html_color |= alpha.value_or(0xFF);
        }

        char* ptr = (char*)&html_color;
        std::reverse(ptr, ptr + 4);

        return html_color;
    }
    else if (color_name.size() > 2 && color_name[0] == '0' && color_name[1] == 'x')
    {
        uint32_t color = std::stoul(color_name, nullptr, 16);
        if (color_name.size() < 9)
            color |= alpha.value_or(0xFF) << 24;

        return color;
    }

    uint32_t color = alpha.value_or(0xFF);
    color <<= 24;
    switch (crc32str(color_name))
    {
    case crc32str("IndianRed"):
    {
        color |= 0x005C5CCD;
        break;
    }
    case crc32str("LightCoral"):
    {
        color |= 0x008080F0;
        break;
    }
    case crc32str("Salmon"):
    {
        color |= 0x007280FA;
        break;
    }
    case crc32str("DarkSalmon"):
    {
        color |= 0x007A96E9;
        break;
    }
    case crc32str("LightSalmon"):
    {
        color |= 0x007AA0FF;
        break;
    }
    case crc32str("Crimson"):
    {
        color |= 0x003C14DC;
        break;
    }
    case crc32str("Red"):
    {
        color |= 0x000000FF;
        break;
    }
    case crc32str("FireBrick"):
    {
        color |= 0x002222B2;
        break;
    }
    case crc32str("DarkRed"):
    {
        color |= 0x0000008B;
        break;
    }
    case crc32str("Pink"):
    {
        color |= 0x00CBC0FF;
        break;
    }
    case crc32str("LightPink"):
    {
        color |= 0x00C1B6FF;
        break;
    }
    case crc32str("HotPink"):
    {
        color |= 0x00B469FF;
        break;
    }
    case crc32str("DeepPink"):
    {
        color |= 0x009314FF;
        break;
    }
    case crc32str("MediumVioletRed"):
    {
        color |= 0x008515C7;
        break;
    }
    case crc32str("PaleVioletRed"):
    {
        color |= 0x009370DB;
        break;
    }
    case crc32str("Coral"):
    {
        color |= 0x00507FFF;
        break;
    }
    case crc32str("Tomato"):
    {
        color |= 0x004763FF;
        break;
    }
    case crc32str("OrangeRed"):
    {
        color |= 0x000045FF;
        break;
    }
    case crc32str("DarkOrange"):
    {
        color |= 0x00008CFF;
        break;
    }
    case crc32str("Orange"):
    {
        color |= 0x0000A5FF;
        break;
    }
    case crc32str("Gold"):
    {
        color |= 0x0000D7FF;
        break;
    }
    case crc32str("Yellow"):
    {
        color |= 0x0000FFFF;
        break;
    }
    case crc32str("LightYellow"):
    {
        color |= 0x00E0FFFF;
        break;
    }
    case crc32str("LemonChiffon"):
    {
        color |= 0x00CDFAFF;
        break;
    }
    case crc32str("LightGoldenrodYellow"):
    {
        color |= 0x00D2FAFA;
        break;
    }
    case crc32str("PapayaWhip"):
    {
        color |= 0x00D5EFFF;
        break;
    }
    case crc32str("Moccasin"):
    {
        color |= 0x00B5E4FF;
        break;
    }
    case crc32str("PeachPuff"):
    {
        color |= 0x00B9DAFF;
        break;
    }
    case crc32str("PaleGoldenrod"):
    {
        color |= 0x00AAE8EE;
        break;
    }
    case crc32str("Khaki"):
    {
        color |= 0x008CE6F0;
        break;
    }
    case crc32str("DarkKhaki"):
    {
        color |= 0x006BB7BD;
        break;
    }
    case crc32str("Lavender"):
    {
        color |= 0x00FAE6E6;
        break;
    }
    case crc32str("Thistle"):
    {
        color |= 0x00D8BFD8;
        break;
    }
    case crc32str("Plum"):
    {
        color |= 0x00DDA0DD;
        break;
    }
    case crc32str("Violet"):
    {
        color |= 0x00EE82EE;
        break;
    }
    case crc32str("Orchid"):
    {
        color |= 0x00D670DA;
        break;
    }
    case crc32str("Fuchsia"):
    {
        color |= 0x00FF00FF;
        break;
    }
    case crc32str("Magenta"):
    {
        color |= 0x00FF00FF;
        break;
    }
    case crc32str("MediumOrchid"):
    {
        color |= 0x00D355BA;
        break;
    }
    case crc32str("MediumPurple"):
    {
        color |= 0x00DB7093;
        break;
    }
    case crc32str("RebeccaPurple"):
    {
        color |= 0x00993366;
        break;
    }
    case crc32str("BlueViolet"):
    {
        color |= 0x00E22B8A;
        break;
    }
    case crc32str("DarkViolet"):
    {
        color |= 0x00D30094;
        break;
    }
    case crc32str("DarkOrchid"):
    {
        color |= 0x00CC3299;
        break;
    }
    case crc32str("DarkMagenta"):
    {
        color |= 0x008B008B;
        break;
    }
    case crc32str("Purple"):
    {
        color |= 0x00800080;
        break;
    }
    case crc32str("Indigo"):
    {
        color |= 0x0082004B;
        break;
    }
    case crc32str("SlateBlue"):
    {
        color |= 0x00CD5A6A;
        break;
    }
    case crc32str("DarkSlateBlue"):
    {
        color |= 0x008B3D48;
        break;
    }
    case crc32str("MediumSlateBlue"):
    {
        color |= 0x00EE687B;
        break;
    }
    case crc32str("GreenYellow"):
    {
        color |= 0x002FFFAD;
        break;
    }
    case crc32str("Chartreuse"):
    {
        color |= 0x0000FF7F;
        break;
    }
    case crc32str("LawnGreen"):
    {
        color |= 0x0000FC7C;
        break;
    }
    case crc32str("Lime"):
    {
        color |= 0x0000FF00;
        break;
    }
    case crc32str("LimeGreen"):
    {
        color |= 0x0032CD32;
        break;
    }
    case crc32str("PaleGreen"):
    {
        color |= 0x0098FB98;
        break;
    }
    case crc32str("LightGreen"):
    {
        color |= 0x0090EE90;
        break;
    }
    case crc32str("MediumSpringGreen"):
    {
        color |= 0x009AFA00;
        break;
    }
    case crc32str("SpringGreen"):
    {
        color |= 0x007FFF00;
        break;
    }
    case crc32str("MediumSeaGreen"):
    {
        color |= 0x0071B33C;
        break;
    }
    case crc32str("SeaGreen"):
    {
        color |= 0x00578B2E;
        break;
    }
    case crc32str("ForestGreen"):
    {
        color |= 0x00228B22;
        break;
    }
    case crc32str("Green"):
    {
        color |= 0x00008000;
        break;
    }
    case crc32str("DarkGreen"):
    {
        color |= 0x00006400;
        break;
    }
    case crc32str("YellowGreen"):
    {
        color |= 0x0032CD9A;
        break;
    }
    case crc32str("OliveDrab"):
    {
        color |= 0x00238E6B;
        break;
    }
    case crc32str("Olive"):
    {
        color |= 0x00008080;
        break;
    }
    case crc32str("DarkOliveGreen"):
    {
        color |= 0x002F6B55;
        break;
    }
    case crc32str("MediumAquamarine"):
    {
        color |= 0x00AACD66;
        break;
    }
    case crc32str("DarkSeaGreen"):
    {
        color |= 0x008BBC8F;
        break;
    }
    case crc32str("LightSeaGreen"):
    {
        color |= 0x00AAB220;
        break;
    }
    case crc32str("DarkCyan"):
    {
        color |= 0x008B8B00;
        break;
    }
    case crc32str("Teal"):
    {
        color |= 0x00808000;
        break;
    }
    case crc32str("Aqua"):
    {
        color |= 0x00FFFF00;
        break;
    }
    case crc32str("Cyan"):
    {
        color |= 0x00FFFF00;
        break;
    }
    case crc32str("LightCyan"):
    {
        color |= 0x00FFFFE0;
        break;
    }
    case crc32str("PaleTurquoise"):
    {
        color |= 0x00EEEEAF;
        break;
    }
    case crc32str("Aquamarine"):
    {
        color |= 0x00D4FF7F;
        break;
    }
    case crc32str("Turquoise"):
    {
        color |= 0x00D0E040;
        break;
    }
    case crc32str("MediumTurquoise"):
    {
        color |= 0x00CCD148;
        break;
    }
    case crc32str("DarkTurquoise"):
    {
        color |= 0x00D1CE00;
        break;
    }
    case crc32str("CadetBlue"):
    {
        color |= 0x00A09E5F;
        break;
    }
    case crc32str("SteelBlue"):
    {
        color |= 0x00B48246;
        break;
    }
    case crc32str("LightSteelBlue"):
    {
        color |= 0x00DEC4B0;
        break;
    }
    case crc32str("PowderBlue"):
    {
        color |= 0x00E6E0B0;
        break;
    }
    case crc32str("LightBlue"):
    {
        color |= 0x00E6D8AD;
        break;
    }
    case crc32str("SkyBlue"):
    {
        color |= 0x00EBCE87;
        break;
    }
    case crc32str("LightSkyBlue"):
    {
        color |= 0x00FACE87;
        break;
    }
    case crc32str("DeepSkyBlue"):
    {
        color |= 0x00FFBF00;
        break;
    }
    case crc32str("DodgerBlue"):
    {
        color |= 0x00FF901E;
        break;
    }
    case crc32str("CornflowerBlue"):
    {
        color |= 0x00ED9564;
        break;
    }
    case crc32str("RoyalBlue"):
    {
        color |= 0x00E16941;
        break;
    }
    case crc32str("Blue"):
    {
        color |= 0x00FF0000;
        break;
    }
    case crc32str("MediumBlue"):
    {
        color |= 0x00CD0000;
        break;
    }
    case crc32str("DarkBlue"):
    {
        color |= 0x008B0000;
        break;
    }
    case crc32str("Navy"):
    {
        color |= 0x00800000;
        break;
    }
    case crc32str("MidnightBlue"):
    {
        color |= 0x00701919;
        break;
    }
    case crc32str("Cornsilk"):
    {
        color |= 0x00DCF8FF;
        break;
    }
    case crc32str("BlanchedAlmond"):
    {
        color |= 0x00CDEBFF;
        break;
    }
    case crc32str("Bisque"):
    {
        color |= 0x00C4E4FF;
        break;
    }
    case crc32str("NavajoWhite"):
    {
        color |= 0x00ADDEFF;
        break;
    }
    case crc32str("Wheat"):
    {
        color |= 0x00B3DEF5;
        break;
    }
    case crc32str("BurlyWood"):
    {
        color |= 0x0087B8DE;
        break;
    }
    case crc32str("Tan"):
    {
        color |= 0x008CB4D2;
        break;
    }
    case crc32str("RosyBrown"):
    {
        color |= 0x008F8FBC;
        break;
    }
    case crc32str("SandyBrown"):
    {
        color |= 0x0060A4F4;
        break;
    }
    case crc32str("Goldenrod"):
    {
        color |= 0x0020A5DA;
        break;
    }
    case crc32str("DarkGoldenrod"):
    {
        color |= 0x000B86B8;
        break;
    }
    case crc32str("Peru"):
    {
        color |= 0x003F85CD;
        break;
    }
    case crc32str("Chocolate"):
    {
        color |= 0x001E69D2;
        break;
    }
    case crc32str("SaddleBrown"):
    {
        color |= 0x0013458B;
        break;
    }
    case crc32str("Sienna"):
    {
        color |= 0x002D52A0;
        break;
    }
    case crc32str("Brown"):
    {
        color |= 0x002A2AA5;
        break;
    }
    case crc32str("Maroon"):
    {
        color |= 0x00000080;
        break;
    }
    case crc32str("White"):
    {
        color |= 0x00FFFFFF;
        break;
    }
    case crc32str("Snow"):
    {
        color |= 0x00FAFAFF;
        break;
    }
    case crc32str("HoneyDew"):
    {
        color |= 0x00F0FFF0;
        break;
    }
    case crc32str("MintCream"):
    {
        color |= 0x00FAFFF5;
        break;
    }
    case crc32str("Azure"):
    {
        color |= 0x00FFFFF0;
        break;
    }
    case crc32str("AliceBlue"):
    {
        color |= 0x00FFF8F0;
        break;
    }
    case crc32str("GhostWhite"):
    {
        color |= 0x00FFF8F8;
        break;
    }
    case crc32str("WhiteSmoke"):
    {
        color |= 0x00F5F5F5;
        break;
    }
    case crc32str("SeaShell"):
    {
        color |= 0x00EEF5FF;
        break;
    }
    case crc32str("Beige"):
    {
        color |= 0x00DCF5F5;
        break;
    }
    case crc32str("OldLace"):
    {
        color |= 0x00E6F5FD;
        break;
    }
    case crc32str("FloralWhite"):
    {
        color |= 0x00F0FAFF;
        break;
    }
    case crc32str("Ivory"):
    {
        color |= 0x00F0FFFF;
        break;
    }
    case crc32str("AntiqueWhite"):
    {
        color |= 0x00D7EBFA;
        break;
    }
    case crc32str("Linen"):
    {
        color |= 0x00E6F0FA;
        break;
    }
    case crc32str("LavenderBlush"):
    {
        color |= 0x00F5F0FF;
        break;
    }
    case crc32str("MistyRose"):
    {
        color |= 0x00E1E4FF;
        break;
    }
    case crc32str("Gainsboro"):
    {
        color |= 0x00DCDCDC;
        break;
    }
    case crc32str("LightGray"):
    {
        color |= 0x00D3D3D3;
        break;
    }
    case crc32str("Silver"):
    {
        color |= 0x00C0C0C0;
        break;
    }
    case crc32str("DarkGray"):
    {
        color |= 0x00A9A9A9;
        break;
    }
    case crc32str("Gray"):
    {
        color |= 0x00808080;
        break;
    }
    case crc32str("DimGray"):
    {
        color |= 0x00696969;
        break;
    }
    case crc32str("LightSlateGray"):
    {
        color |= 0x00998877;
        break;
    }
    case crc32str("SlateGray"):
    {
        color |= 0x00908070;
        break;
    }
    case crc32str("DarkSlateGray"):
    {
        color |= 0x004F4F2F;
        break;
    }
    case crc32str("Black"):
    {
        color |= 0x00000000;
        break;
    }
    }
    return color;
}
