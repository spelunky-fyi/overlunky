#pragma once

struct AABB
{
    /// Create a new axis aligned bounding box - defaults to all zeroes
    AABB() = default;

    /// Copy an axis aligned bounding box
    AABB(const AABB&) = default;

    /// Create a new axis aligned bounding box by specifying its values
    AABB(float left_, float top_, float right_, float bottom_)
        : left(left_), top(top_), right(right_), bottom(bottom_){};

    bool overlaps_with(const AABB& other) const
    {
        return left < other.right && other.left < right && bottom < other.top && other.bottom < top;
    }

    bool is_valid() const
    {
        return !(left == 0.0f && right == 0.0f && top == 0.0f && bottom == 0.0f);
    }

    /// Fixes the AABB if any of the sides have negative length
    AABB& abs()
    {
        if (left > right)
            std::swap(left, right);
        if (bottom > top)
            std::swap(bottom, top);
        return *this;
    }

    /// Grows or shrinks the AABB by the given amount in all directions.
    /// If `amount < 0` and `abs(amount) > right/top - left/bottom` the respective dimension of the AABB will become `0`.
    AABB& extrude(float amount)
    {
        left -= amount;
        right += amount;
        if (left > right)
        {
            left = (left + right) / 2.0f;
            right = left;
        }

        bottom -= amount;
        top += amount;
        if (bottom > top)
        {
            bottom = (bottom + top) / 2.0f;
            top = bottom;
        }
        return *this;
    }
    /// Offsets the AABB by the given offset.
    AABB& offset(float off_x, float off_y)
    {
        left += off_x;
        bottom += off_y;
        right += off_x;
        top += off_y;
        return *this;
    }
    /// Compute area of the AABB, can be zero if one dimension is zero or negative if one dimension is inverted.
    float area() const
    {
        return width() * height();
    }
    /// Short for `(aabb.left + aabb.right) / 2.0f, (aabb.top + aabb.bottom) / 2.0f`.
    std::pair<float, float> center() const
    {
        return {(left + right) / 2.0f, (top + bottom) / 2.0f};
    }
    /// Short for `aabb.right - aabb.left`.
    float width() const
    {
        return (right - left);
    }
    /// Short for `aabb.top - aabb.bottom`.
    float height() const
    {
        return (top - bottom);
    }

    float left{0};
    float top{0};
    float right{0};
    float bottom{0};
};

struct Quad
{
    Quad() = default;

    Quad(const Quad&) = default;

    Quad(float _bottom_left_x, float _bottom_left_y, float _bottom_right_x, float _bottom_right_y, float _top_right_x, float _top_right_y, float _top_left_x, float _top_left_y)
        : bottom_left_x(_bottom_left_x), bottom_left_y(_bottom_left_y), bottom_right_x(_bottom_right_x), bottom_right_y(_bottom_right_y), top_right_x(_top_right_x), top_right_y(_top_right_y), top_left_x(_top_left_x), top_left_y(_top_left_y){};

    Quad(const AABB& aabb)
        : bottom_left_x(aabb.left), bottom_left_y(aabb.bottom), bottom_right_x(aabb.right), bottom_right_y(aabb.bottom), top_right_x(aabb.right), top_right_y(aabb.top), top_left_x(aabb.left), top_left_y(aabb.top){};

    /// Returns the max/min values of the Quad
    AABB get_AABB() const
    {
        AABB result;
        result.right = std::max({bottom_left_x, bottom_right_x, top_right_x, top_left_x});
        result.left = std::min({bottom_left_x, bottom_right_x, top_right_x, top_left_x});
        result.top = std::max({bottom_left_y, bottom_right_y, top_right_y, top_left_y});
        result.bottom = std::min({bottom_left_y, bottom_right_y, top_right_y, top_left_y});
        return result;
    }

    Quad& offset(float off_x, float off_y)
    {
        bottom_left_x += off_x;
        bottom_right_x += off_x;
        top_right_x += off_x;
        top_left_x += off_x;

        bottom_left_y += off_y;
        bottom_right_y += off_y;
        top_right_y += off_y;
        top_left_y += off_y;
        return *this;
    }

    /// Rotates a Quad by an angle, px/py are not offsets, use `:get_AABB():center()` to get approximated center for simetrical quadrangle
    Quad& rotate(float angle, float px, float py)
    {
        const float sin_a{std::sin(angle)};
        const float cos_a{std::cos(angle)};

        using vec = std::pair<float, float>;
        const vec p{ px, py };
        const vec mp{ -px, -py };

        auto rotate_around_pivot = [=](vec in) -> vec
        {
            auto add = [](vec lhs, vec rhs) -> vec
            {
                return {
                    lhs.first + rhs.first,
                    lhs.second + rhs.second,
                };
            };
            in = add(in, mp);
            {
                auto [x, y] = in;
                in = {
                    x * cos_a - y * sin_a,
                    y * cos_a + x * sin_a,
                };
            }
            in = add(in, p);
            return in;
        };

        std::tie(bottom_left_x, bottom_left_y) = rotate_around_pivot({ bottom_left_x, bottom_left_y });
        std::tie(bottom_right_x, bottom_right_y) = rotate_around_pivot({ bottom_right_x, bottom_right_y });
        std::tie(top_left_x, top_left_y) = rotate_around_pivot({ top_left_x, top_left_y });
        std::tie(top_right_x, top_right_y) = rotate_around_pivot({ top_right_x, top_right_y });

        return *this;
    }

    float bottom_left_x{0};
    float bottom_left_y{0};
    float bottom_right_x{0};
    float bottom_right_y{0};
    float top_right_x{0};
    float top_right_y{0};
    float top_left_x{0};
    float top_left_y{0};
};
