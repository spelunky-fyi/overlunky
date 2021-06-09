#include "entity.hpp"

#include <imgui.h>

#include <string>

using Toast = void (*)(void*, wchar_t*);
Toast get_toast();

using Say = void (*)(void*, Entity*, wchar_t*, int unk_type /* 0, 2, 3 */, bool top /* top or bottom */);
Say get_say();

using Prng = void (*)(int64_t seed);
Prng get_seed_prng();

float screenify(float dis);

ImVec2 screenify(ImVec2 pos);

ImVec2 normalize(ImVec2 pos);

void AddImageRotated(ImDrawList* draw_list, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float angle, const ImVec2& rel_pivot);

std::string sanitize(std::string data);

struct InputTextCallback_UserData
{
    std::string* Str;
    ImGuiInputTextCallback ChainCallback;
    void* ChainCallbackUserData;
};

int InputTextCallback(ImGuiInputTextCallbackData* data);

bool InputString(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data);
