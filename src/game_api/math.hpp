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

    float left;
    float top;
    float right;
    float bottom;
};
