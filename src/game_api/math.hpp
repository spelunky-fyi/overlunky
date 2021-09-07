#pragma once

struct AABB
{
    /// Create a new axis aligned bounding box - defaults to all zeroes
    AABB() = default;

    /// Create a new axis aligned bounding box by specifying its values
    AABB(float left_, float top_, float right_, float bottom_)
        : left(left_), top(top_), right(right_), bottom(bottom_)
    {
    }

    bool overlaps_with(const AABB& other) const
    {
        return left < other.right && other.left < right && bottom < other.top && other.bottom < top;
    }

    bool is_valid() const
    {
        return !(left == 0.0f && right == 0.0f && top == 0.0f && bottom == 0.0f);
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
