#include "entity.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <string>

using Toast = void (*)(wchar_t*);
Toast get_toast();

using Say = void (*)(void*, Entity*, wchar_t*, int unk_type /* 0, 2, 3 */, bool top /* top or bottom */);
Say get_say();
size_t get_say_context();

void seed_prng(int64_t seed);

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
