#include "script_util.hpp"

#include <algorithm>               // for transform
#include <cctype>                  // for tolower
#include <cmath>                   // for cos, sin
#include <cstdlib>                 // for free
#include <cstring>                 // for memchr
#include <misc/cpp/imgui_stdlib.h> // IWYU pragma: keep
#include <new>                     // for operator new
#include <regex>                   // for regex_replace, optimize, regex
#include <type_traits>             // for move
#include <utility>                 // for min, max, _Adl_verify_range

float screenify(float dis)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    return dis / (1.0f / (res.x / 2));
}

ImVec2 screenify(ImVec2 pos)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    ImVec2 bar = {0.0, 0.0};
    if (res.x / res.y > 1.78)
    {
        bar.x = (res.x - res.y / 9 * 16) / 2;
        res.x = res.y / 9 * 16;
    }
    else if (res.x / res.y < 1.77)
    {
        bar.y = (res.y - res.x / 16 * 9) / 2;
        res.y = res.x / 16 * 9;
    }
    ImVec2 screened = ImVec2(pos.x / (1.0f / (res.x / 2)) + res.x / 2 + bar.x, res.y - (res.y / 2 * pos.y) - res.y / 2 + bar.y);
    return screened;
}

ImVec2 screenify_fix(ImVec2 pos)
{
    ImGuiIO& io = ImGui::GetIO();
    auto base = ImGui::GetMainViewport();

    ImVec2 res = io.DisplaySize;
    ImVec2 bar = {0.0, 0.0};
    if (res.x / res.y > 1.78)
    {
        bar.x = (res.x - res.y / 9 * 16) / 2;
        res.x = res.y / 9 * 16;
    }
    else if (res.x / res.y < 1.77)
    {
        bar.y = (res.y - res.x / 16 * 9) / 2;
        res.y = res.x / 16 * 9;
    }
    ImVec2 screened = ImVec2(pos.x / (1.0f / (res.x / 2)) + res.x / 2 + bar.x + base->Pos.x, res.y - (res.y / 2 * pos.y) - res.y / 2 + bar.y + base->Pos.y);
    return screened;
}

ImVec2 normalize(ImVec2 pos)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 res = io.DisplaySize;
    if (res.x / res.y > 1.78)
    {
        pos.x -= (res.x - res.y / 9 * 16) / 2;
        res.x = res.y / 9 * 16;
    }
    else if (res.x / res.y < 1.77)
    {
        pos.y -= (res.y - res.x / 16 * 9) / 2;
        res.y = res.x / 16 * 9;
    }
    ImVec2 normal = ImVec2((pos.x - res.x / 2) * (1.0f / (res.x / 2)), -(pos.y - res.y / 2) * (1.0f / (res.y / 2)));
    return normal;
}

void AddImageRotated(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float angle, const ImVec2& rel_pivot)
{
    if ((col & IM_COL32_A_MASK) == 0)
        return;

    auto add = [](const ImVec2& lhs, const ImVec2& rhs) -> ImVec2
    {
        return {lhs.x + rhs.x, lhs.y + rhs.y};
    };
    auto sub = [](const ImVec2& lhs, const ImVec2& rhs) -> ImVec2
    {
        return {lhs.x - rhs.x, lhs.y - rhs.y};
    };
    auto mul = [](const ImVec2& lhs, float rhs) -> ImVec2
    {
        return {lhs.x * rhs, lhs.y * rhs};
    };

    const float sin_a = std::sin(angle);
    const float cos_a = std::cos(angle);
    auto rot = [sin_a, cos_a, add, sub](const ImVec2& vec, const ImVec2& pivot)
    {
        const ImVec2 off = sub(vec, pivot);
        const ImVec2 _rot{off.x * cos_a - off.y * sin_a, off.x * sin_a + off.y * cos_a};
        return add(_rot, pivot);
    };

    const ImVec2 center = mul(add(p_min, p_max), 0.5f);
    const ImVec2 pivot = add(center, rel_pivot);

    const ImVec2 a = rot(p_min, pivot);
    const ImVec2 b = rot(ImVec2{p_max.x, p_min.y}, pivot);
    const ImVec2 c = rot(p_max, pivot);
    const ImVec2 d = rot(ImVec2{p_min.x, p_max.y}, pivot);

    const ImVec2 uv_a = uv_min;
    const ImVec2 uv_b = ImVec2{uv_max.x, uv_min.y};
    const ImVec2 uv_c = uv_max;
    const ImVec2 uv_d = ImVec2{uv_min.x, uv_max.y};

    const bool push_texture_id = user_texture_id != draw_list->_CmdHeader.TextureId;
    if (push_texture_id)
        draw_list->PushTextureID(user_texture_id);

    draw_list->PrimReserve(6, 4);
    draw_list->PrimQuadUV(a, b, c, d, uv_a, uv_b, uv_c, uv_d, col);

    if (push_texture_id)
        draw_list->PopTextureID();
}

std::string sanitize(std::string data)
{
    std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c)
                   { return (unsigned char)std::tolower(c); });
    static std::regex reg("[^a-z/]*", std::regex_constants::optimize);
    data = std::regex_replace(data, reg, "");
    return data;
}

bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    return ImGui::InputText(label, str, flags, callback, user_data);
}
