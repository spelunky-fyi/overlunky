#pragma once

struct AABB
{
    /// Create a new axis aligned bounding box - defaults to all zeroes
    AABB()
        : left(0.0), top(0.0), right(0.0), bottom(0.0)
    {
    }

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

    float left;
    float top;
    float right;
    float bottom;
};
