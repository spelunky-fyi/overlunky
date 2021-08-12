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
};
