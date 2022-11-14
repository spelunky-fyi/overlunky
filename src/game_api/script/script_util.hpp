#pragma once

#include <cstddef> // for size_t
#include <imgui.h> // for ImVec2, ImGuiInputTextCallback, ImDrawList (ptr ...
#include <string>  // for string

float screenify(float dis);

ImVec2 screenify(ImVec2 pos);
ImVec2 screenify_fix(ImVec2 pos);

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
