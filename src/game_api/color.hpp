#pragma once

struct Color
{
    /// Create a new color - defaults to black
    constexpr Color() = default;
    constexpr Color(const Color&) = default;
    constexpr Color(Color&&) = default;
    constexpr Color& operator=(const Color&) = default;
    constexpr Color& operator=(Color&&) = default;

    /// Create a new color by specifying its values
    constexpr Color(float r_, float g_, float b_, float a_)
        : r(r_), g(g_), b(b_), a(a_)
    {
    }

    /// Create a color from an array of 4 floats
    constexpr Color(float (&c)[4])
        : r(c[0]), g(c[1]), b(c[2]), a(c[3])
    {
    }

    constexpr void to_float(float (&c)[4]) const
    {
        c[0] = r;
        c[1] = g;
        c[2] = b;
        c[3] = a;
    }

    static constexpr Color white()
    {
        return Color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    static constexpr Color silver()
    {
        return Color(0.75f, 0.75f, 0.75f, 1.0f);
    }

    static constexpr Color gray()
    {
        return Color(0.5f, 0.5f, 0.5f, 1.0f);
    }

    static constexpr Color black()
    {
        return Color();
    }

    static constexpr Color red()
    {
        return Color(1.0f, 0.0f, 0.0f, 1.0f);
    }

    static constexpr Color maroon()
    {
        return Color(0.5f, 0.0f, 0.0f, 1.0f);
    }

    static constexpr Color yellow()
    {
        return Color(1.0f, 1.0f, 0.0f, 1.0f);
    }

    static constexpr Color olive()
    {
        return Color(0.5f, 0.5f, 0.0f, 1.0f);
    }

    static constexpr Color lime()
    {
        return Color(0.0f, 1.0f, 0.0f, 1.0f);
    }

    static constexpr Color green()
    {
        return Color(0.0f, 0.5f, 0.0f, 1.0f);
    }

    static constexpr Color aqua()
    {
        return Color(0.0f, 1.0f, 1.0f, 1.0f);
    }

    static constexpr Color teal()
    {
        return Color(0.0f, 0.5f, 0.5f, 1.0f);
    }

    static constexpr Color blue()
    {
        return Color(0.0f, 0.0f, 1.0f, 1.0f);
    }

    static constexpr Color navy()
    {
        return Color(0.0f, 0.0f, 0.5f, 1.0f);
    }

    static constexpr Color fuchsia()
    {
        return Color(1.0f, 0.0f, 1.0f, 1.0f);
    }

    static constexpr Color purple()
    {
        return Color(0.5f, 0.0f, 0.5f, 1.0f);
    }

    float r{0.0f};
    float g{0.0f};
    float b{0.0f};
    float a{0.0f};

    /// Returns RGBA colors in 0..255 range
    std::tuple<uint8_t, uint8_t, uint8_t, uint8_t> get_rgba()
    {
        return {toRGB(r), toRGB(g), toRGB(b), toRGB(a)};
    }
    /// Changes color based on given RGBA colors in 0..255 range
    void set_rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        r = red / 255.0f;
        g = green / 255.0f;
        b = blue / 255.0f;
        a = alpha / 255.0f;
    }
    /// Returns the `uColor` used in `GuiDrawContext` drawing functions
    uColor get_ucolor()
    {
        return (toRGB(a) << 24) + (toRGB(b) << 16) + (toRGB(g) << 8) + (toRGB(r));
    }
    /// Changes color based on given uColor
    void set_ucolor(uColor color)
    {
        uint8_t red = color & 0xFF;
        uint8_t green = (color >> 8) & 0xFF;
        uint8_t blue = (color >> 16) & 0xFF;
        uint8_t alpha = (color >> 24) & 0xFF;
        set_rgba(red, green, blue, alpha);
    }

  private:
    uint8_t toRGB(float c)
    {
        return static_cast<uint8_t>(round(255 * fmin(fmax(c, 0.0f), 1.0f)));
    }
};
