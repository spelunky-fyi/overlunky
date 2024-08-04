#include "math.hpp"

bool Triangle::is_point_inside(const Vec2 p, float epsilon) const noexcept
{
    // you can compare it either by area or by angle
    // not sure if one if faster then the order, so i left code for both

    // float area1 = Triangle(A, B, p).area();
    // float area2 = Triangle(A, C, p).area();
    // float area3 = Triangle(B, C, p).area();
    // return std::abs(this->area() - (area1 + area2 + area3)) < epsilon;

    float angle1 = two_lines_angle(A, p, B);
    float angle2 = two_lines_angle(B, p, C);
    float angle3 = two_lines_angle(C, p, A);
    const float pi = 3.14159f;
    return std::abs(pi - (angle1 + angle2 + angle3)) < epsilon;
}

Vec2 intersection(const Vec2 A, const Vec2 B, const Vec2 C, const Vec2 D) noexcept
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

float two_lines_angle(const Vec2 A, const Vec2 common, const Vec2 B) noexcept
{
    Vec2 ab = common - B;
    Vec2 bc = A - common;
    return std::atan2((bc.y * ab.x - bc.x * ab.y), (bc.x * ab.x + bc.y * ab.y));
};

float two_lines_angle(const Vec2 line1_A, const Vec2 line1_B, const Vec2 line2_A, const Vec2 line2_B) noexcept
{
    return two_lines_angle(line1_A, intersection(line1_A, line1_B, line2_A, line2_B), line2_B);
};

bool Quad::is_point_inside(const Vec2 p, float epsilon) const noexcept
{
    std::tuple<Vec2, Vec2, Vec2, Vec2> points = *this;

    const Vec2 extra_point{999.0f, p.y};
    int count = 0;
    AABB cd{p, Vec2{INFINITY, p.y}};
    cd.abs().extrude(0, epsilon);

    auto check_if_on_line = [&cd, &epsilon](Vec2& A, Vec2& B, Vec2& ints) // this only makes sense together with intersection function
    {
        AABB ab{A, B};
        ab.abs().extrude(A.x == B.x ? epsilon : 0, A.y == B.y ? epsilon : 0);

        if (ab.is_point_inside(ints) && cd.is_point_inside(ints))
            return true;

        return false;
    };
    auto first = intersection(std::get<0>(points), std::get<1>(points), p, extra_point);
    if (!isinf(first.x) && check_if_on_line(std::get<0>(points), std::get<1>(points), first))
    {
        ++count;
    }
    auto second = intersection(std::get<1>(points), std::get<2>(points), p, extra_point);
    if (!isinf(second.x) && check_if_on_line(std::get<1>(points), std::get<2>(points), second))
    {
        ++count;
    }
    auto third = intersection(std::get<2>(points), std::get<3>(points), p, extra_point);
    if (!isinf(third.x) && check_if_on_line(std::get<2>(points), std::get<3>(points), third))
    {
        ++count;
    }
    auto fourth = intersection(std::get<3>(points), std::get<0>(points), p, extra_point);
    if (!isinf(fourth.x) && check_if_on_line(std::get<3>(points), std::get<0>(points), fourth))
    {
        ++count;
    }
    return (count & 1); // check if not even
}
