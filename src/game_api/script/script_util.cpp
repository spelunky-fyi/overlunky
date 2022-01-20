#include "script_util.hpp"
#include "memory.hpp"
#include "prng.hpp"

#include <regex>

Toast get_toast()
{
    static Toast toast = (Toast)get_address("toast");
    return toast;
}

Say get_say()
{
    static Say say = (Say)get_address("speech_bubble_fun");
    return say;
}

size_t get_say_context()
{
    static size_t say_context = get_address("say_context");
    return say_context;
}

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

int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data;
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = callback;
    cb_user_data.ChainCallbackUserData = user_data;
    return ImGui::InputText(label, str, flags, InputTextCallback, &cb_user_data);
}
