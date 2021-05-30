#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

#include <Windows.h>

using AllocFun = decltype(malloc);

struct FileInfo
{
    void* Data{nullptr};
    int _member_1{0};
    int DataSize{0};
    int AllocationSize{0};
    int _member_4{0};
};
using LoadFileCallback = FileInfo*(const char* file_path, AllocFun alloc_fun);
using GetImageFilePathCallback = std::string(std::string root_path, std::string relative_path);

FileInfo* load_file_as_dds_if_image(const char* file_path, AllocFun alloc_fun);

void register_on_load_file(LoadFileCallback on_load_file);
void register_get_image_file_path(GetImageFilePathCallback get_image_file_path);

std::string get_image_file_path(std::string root_path, std::string relative_path);

bool create_d3d11_texture_from_file(const char* filename, struct ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
