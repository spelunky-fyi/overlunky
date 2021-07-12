#pragma once

#include <string>
#include <string_view>

#include <sol/forward.hpp>

struct AABB
{
    float left;
    float bottom;
    float right;
    float top;
};

namespace NHitbox
{
void register_usertypes(sol::state& lua);
}
