#pragma once

#include "aliases.hpp"

#include <cmath>
#include <optional>
#include <string>
#include <tuple>

/// Convert a string to a color, you can use the HTML color names, or even HTML color codes, just prefix them with '#' symbol
/// You can also convert hex string into a color, prefix it with '0x', but use it only if you need to since lua allows for hex values directly too.
/// Default alpha value will be 0xFF, unless it's specified
/// Format: [name], #RRGGBB, #RRGGBBAA, 0xBBGGRR, 0xAABBGGRR
uColor get_color(const std::string& color_name, std::optional<uint8_t> alpha = std::nullopt);

struct Color
{
    /// Create a new color - defaults to black
    constexpr Color() = default;
    constexpr Color(const Color& other) = default;
    constexpr Color(Color&&) = default;
    constexpr Color& operator=(const Color&) = default;
    constexpr Color& operator=(Color&&) = default;

    /// Create a new color by specifying its values
    constexpr Color(float r_, float g_, float b_, float a_) noexcept
        : r(r_), g(g_), b(b_), a(a_){};

    /// Create a color from an array of 4 floats
    constexpr Color(const float (&c)[4]) noexcept
        : r(c[0]), g(c[1]), b(c[2]), a(c[3]){};

    Color(const std::string& color_name, std::optional<uint8_t> alpha = std::nullopt)
    {
        set_ucolor(get_color(color_name, alpha));
    }

    static constexpr Color white() noexcept
    {
        return Color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    static constexpr Color silver() noexcept
    {
        return Color(0.75f, 0.75f, 0.75f, 1.0f);
    }

    static constexpr Color gray() noexcept
    {
        return Color(0.5f, 0.5f, 0.5f, 1.0f);
    }

    static constexpr Color black() noexcept
    {
        return Color();
    }

    static constexpr Color red() noexcept
    {
        return Color(1.0f, 0.0f, 0.0f, 1.0f);
    }

    static constexpr Color maroon() noexcept
    {
        return Color(0.5f, 0.0f, 0.0f, 1.0f);
    }

    static constexpr Color yellow() noexcept
    {
        return Color(1.0f, 1.0f, 0.0f, 1.0f);
    }

    static constexpr Color olive() noexcept
    {
        return Color(0.5f, 0.5f, 0.0f, 1.0f);
    }

    static constexpr Color lime() noexcept
    {
        return Color(0.0f, 1.0f, 0.0f, 1.0f);
    }

    static constexpr Color green() noexcept
    {
        return Color(0.0f, 0.5f, 0.0f, 1.0f);
    }

    static constexpr Color aqua() noexcept
    {
        return Color(0.0f, 1.0f, 1.0f, 1.0f);
    }

    static constexpr Color teal() noexcept
    {
        return Color(0.0f, 0.5f, 0.5f, 1.0f);
    }

    static constexpr Color blue() noexcept
    {
        return Color(0.0f, 0.0f, 1.0f, 1.0f);
    }

    static constexpr Color navy() noexcept
    {
        return Color(0.0f, 0.0f, 0.5f, 1.0f);
    }

    static constexpr Color fuchsia() noexcept
    {
        return Color(1.0f, 0.0f, 1.0f, 1.0f);
    }

    static constexpr Color purple() noexcept
    {
        return Color(0.5f, 0.0f, 0.5f, 1.0f);
    }

    constexpr void to_float(float (&c)[4]) const noexcept
    {
        c[0] = r;
        c[1] = g;
        c[2] = b;
        c[3] = a;
    }

    /// Returns RGBA colors in 0..255 range
    std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> get_rgba() const noexcept
    {
        return {toRGB(r), toRGB(g), toRGB(b), toRGB(a)};
    }
    /// Changes color based on given RGBA colors in 0..255 range
    Color& set_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        r = red / 255.0f;
        g = green / 255.0f;
        b = blue / 255.0f;
        a = alpha / 255.0f;
        return *this;
    }
    /// Returns the `uColor` used in `GuiDrawContext` drawing functions
    uColor get_ucolor() const noexcept
    {
        return (toRGB(a) << 24) + (toRGB(b) << 16) + (toRGB(g) << 8) + (toRGB(r));
    }
    /// Changes color based on given uColor
    Color& set_ucolor(const uColor color)
    {
        uint8_t red = color & 0xFF;
        uint8_t green = (color >> 8U) & 0xFF;
        uint8_t blue = (color >> 16U) & 0xFF;
        uint8_t alpha = (color >> 24U) & 0xFF;
        return set_rgba(red, green, blue, alpha);
    }

    /// Copies the values of different Color to this one
    Color& set(Color& other)
    {
        *this = other;
        return *this;
    }

    /// Comparison using RGB to avoid non-precise float value
    bool operator==(const Color& col) const noexcept
    {
        const auto current = get_rgba();
        const auto compare = col.get_rgba();
        return current == compare;
    }

    float r{0.0f};
    float g{0.0f};
    float b{0.0f};
    float a{1.0f};

  private:
    uint8_t toRGB(const float c) const noexcept
    {
        // this is not how the game does it, but not sure how to make it 1 : 1
        return static_cast<uint8_t>(std::round(255 * std::min(std::max(c, 0.0f), 1.0f)));
    }
};
