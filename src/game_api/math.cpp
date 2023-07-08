#include "math.hpp"

Vec2 intersection(const Vec2 A, const Vec2 B, const Vec2 C, const Vec2 D)
{
    float a = B.y - A.y;
    float b = A.x - B.x;
    float c = a * (A.x) + b * (A.y);
    // Line CD represented as a2x + b2y = c2
    float a1 = D.y - C.y;
    float b1 = C.x - D.x;
    float c1 = a1 * (C.x) + b1 * (C.y);

    float det = a * b1 - a1 * b;

    if (det == 0)
        return {INFINITY, INFINITY};

    return Vec2{(b1 * c - b * c1) / det, (a * c1 - a1 * c) / det};
}
