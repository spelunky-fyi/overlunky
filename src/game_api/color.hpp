#pragma once

struct Color
{
    /// Create a new color - defaults to black
    constexpr Color()
        : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
    {
    }

    /// Create a new color by specifying its values
    constexpr Color(float r_, float g_, float b_, float a_)
        : r(r_), g(g_), b(b_), a(a_)
    {
    }

    static Color white()
    {
        return Color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    static Color silver()
    {
        return Color(0.75f, 0.75f, 0.75f, 1.0f);
    }

    static Color gray()
    {
        return Color(0.5f, 0.5f, 0.5f, 1.0f);
    }

    static Color black()
    {
        return Color();
    }

    static Color red()
    {
        return Color(1.0f, 0.0f, 0.0f, 1.0f);
    }

    static Color maroon()
    {
        return Color(0.5f, 0.0f, 0.0f, 1.0f);
    }

    static Color yellow()
    {
        return Color(1.0f, 1.0f, 0.0f, 1.0f);
    }

    static Color olive()
    {
        return Color(0.5f, 0.5f, 0.0f, 1.0f);
    }

    static Color lime()
    {
        return Color(0.0f, 1.0f, 0.0f, 1.0f);
    }

    static Color green()
    {
        return Color(0.0f, 0.5f, 0.0f, 1.0f);
    }

    static Color aqua()
    {
        return Color(0.0f, 1.0f, 1.0f, 1.0f);
    }

    static Color teal()
    {
        return Color(0.0f, 0.5f, 0.5f, 1.0f);
    }

    static Color blue()
    {
        return Color(0.0f, 0.0f, 1.0f, 1.0f);
    }

    static Color navy()
    {
        return Color(0.0f, 0.0f, 0.5f, 1.0f);
    }

    static Color fuchsia()
    {
        return Color(1.0f, 0.0f, 1.0f, 1.0f);
    }

    static Color purple()
    {
        return Color(0.5f, 0.0f, 0.5f, 1.0f);
    }

    float r;
    float g;
    float b;
    float a;

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
