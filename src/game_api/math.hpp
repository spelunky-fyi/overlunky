#pragma once

struct AABB
{
    /// Create a new axis aligned bounding box - defaults to all zeroes
    AABB()
        : left(0.0), bottom(0.0), right(0.0), top(0.0)
    {
    }

    /// Create a new axis aligned bounding box by specifying its values
    AABB(float left_, float bottom_, float right_, float top_)
        : left(left_), bottom(bottom_), right(right_), top(top_)
    {
    }

    float left;
    float bottom;
    float right;
    float top;
};
