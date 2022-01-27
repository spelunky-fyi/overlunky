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

struct QuadTree
{
    QuadTree() = default;

    QuadTree(const QuadTree&) = default;

    QuadTree(float _bottom_left_x, float _bottom_left_y, float _bottom_right_x, float _bottom_right_y, float _top_right_x, float _top_right_y, float _top_left_x, float _top_left_y)
        : bottom_left_x(_bottom_left_x), bottom_left_y(_bottom_left_y), bottom_right_x(_bottom_right_x), bottom_right_y(_bottom_right_y), top_right_x(_top_right_x), top_right_y(_top_right_y), top_left_x(_top_left_x), top_left_y(_top_left_y){};

    QuadTree(const AABB& aabb)
    {
        QuadTree{aabb.left, aabb.bottom, aabb.right, aabb.bottom, aabb.right, aabb.top, aabb.left, aabb.top};
    }
    /// Short for `(quad.bottom_left_x + quad.top_right_x) / 2.0f, (quad.bottom_left_y + quad.top_right_y) / 2.0f`.
    std::pair<float, float> center() const
    {
        float detL1 = bottom_left_x * bottom_right_y - bottom_left_y * bottom_right_x;
        float detL2 = top_right_x * top_left_y - top_right_y * top_left_x;
        float x1mx2 = bottom_left_x - bottom_right_x;
        float x3mx4 = top_right_x - top_left_x;
        float y1my2 = bottom_left_y - bottom_right_y;
        float y3my4 = top_right_y - top_left_y;

        float xnom = detL1 * x3mx4 - x1mx2 * detL2;
        float ynom = detL1 * y3my4 - y1my2 * detL2;
        float denom = x1mx2 * y3my4 - y1my2 * x3mx4;
        if (denom == 0.0) //Lines don't seem to cross
        {
            return {NAN, NAN};
        }

        return {xnom / denom, ynom / denom};
    }

    /// Returns the max/min values of the Quad
    AABB get_AABB()
    {
        AABB result;
        result.right = std::max(std::max(std::max(bottom_left_x, bottom_right_x), top_right_x), top_left_x);
        result.left = std::min(std::min(std::min(bottom_left_x, bottom_right_x), top_right_x), top_left_x);
        result.top = std::max(std::max(std::max(bottom_left_y, bottom_right_y), top_right_y), top_left_y);
        result.bottom = std::min(std::min(std::min(bottom_left_y, bottom_right_y), top_right_y), top_left_y);
        return result;
    }

    QuadTree& offset(float off_x, float off_y)
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

    float bottom_left_x{0};
    float bottom_left_y{0};
    float bottom_right_x{0};
    float bottom_right_y{0};
    float top_right_x{0};
    float top_right_y{0};
    float top_left_x{0};
    float top_left_y{0};
};
