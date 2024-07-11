#pragma once

#include "aliases.hpp"

#include <cmath>
#include <tuple>

struct Color
{
    /// Create a new color - defaults to black
    constexpr Color() = default;
    constexpr Color(const Color& other) = default;
    constexpr Color(Color&&) = default;
    constexpr Color& operator=(const Color&) = default;
    constexpr Color& operator=(Color&&) = default;

    /// Comparison using RGB to avoid non-precise float value
    bool operator==(const Color& col) const noexcept
    {
        const auto current = get_rgba();
        const auto compare = col.get_rgba();
        return std::get<0>(current) == std::get<0>(compare) &&
               std::get<1>(current) == std::get<1>(compare) &&
               std::get<2>(current) == std::get<2>(compare) &&
               std::get<3>(current) == std::get<3>(compare);
    }

    /// Create a new color by specifying its values
    constexpr Color(float r_, float g_, float b_, float a_) noexcept
        : r(r_), g(g_), b(b_), a(a_){};

    /// Create a color from an array of 4 floats
    constexpr Color(const float (&c)[4]) noexcept
        : r(c[0]), g(c[1]), b(c[2]), a(c[3]){};

    constexpr void to_float(float (&c)[4]) const noexcept
    {
        c[0] = r;
        c[1] = g;
        c[2] = b;
        c[3] = a;
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

    /// Returns RGBA colors in 0..255 range
    std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> get_rgba() const noexcept
    {
        return {toRGB(r), toRGB(g), toRGB(b), toRGB(a)};
    }
    /// Changes color based on given RGBA colors in 0..255 range
    Color& set_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) noexcept
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
    Color& set_ucolor(const uColor color) noexcept
    {
        uint8_t red = color & 0xFF;
        uint8_t green = (color >> 8U) & 0xFF;
        uint8_t blue = (color >> 16U) & 0xFF;
        uint8_t alpha = (color >> 24U) & 0xFF;
        return set_rgba(red, green, blue, alpha);
    }
    /// Copies the values of different Color to this one
    Color& set(Color& other) noexcept
    {
        *this = other;
        return *this;
    }

    float r{0.0f};
    float g{0.0f};
    float b{0.0f};
    float a{1.0f};

  private:
    uint8_t toRGB(const float c) const noexcept
    {
        return static_cast<uint8_t>(std::round(255 * std::min(std::max(c, 0.0f), 1.0f)));
    }
};
