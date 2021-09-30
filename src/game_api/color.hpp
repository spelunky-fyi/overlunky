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
};
