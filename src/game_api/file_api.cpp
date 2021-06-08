#include "file_api.hpp"

#include <atomic>
#include <chrono>

#include <d3d11.h>
#include <detours.h>

#include "logger.h"
#include "memory.hpp"
#include "window_api.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

LoadFileCallback* g_OnLoadFile{nullptr};
GetImageFilePathCallback* g_GetImageFilePath{nullptr};

FileInfo* load_file_as_dds_if_image(const char* file_path, AllocFun alloc_fun)
{
    using namespace std::string_view_literals;
    auto path = std::string_view(file_path);
    auto ext = path.substr(path.find_last_of('.'));
    if (ext == ".png"sv || ext == ".jpeg"sv || ext == ".bmp"sv || ext == ".tga"sv)
    {
        int image_width = 0;
        int image_height = 0;
        unsigned char* image_data = stbi_load(file_path, &image_width, &image_height, NULL, 4);
        if (image_data != nullptr)
        {
            // https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dds-header
            struct DDS_PIXELFORMAT
            {
                DWORD dwSize;
                DWORD dwFlags;
                DWORD dwFourCC;
                DWORD dwRGBBitCount;
                DWORD dwRBitMask;
                DWORD dwGBitMask;
                DWORD dwBBitMask;
                DWORD dwABitMask;
            };
            typedef struct
            {
                DWORD dwSize;
                DWORD dwFlags;
                DWORD dwHeight;
                DWORD dwWidth;
                DWORD dwPitchOrLinearSize;
                DWORD dwDepth;
                DWORD dwMipMapCount;
                DWORD dwReserved1[11];
                DDS_PIXELFORMAT ddspf;
                DWORD dwCaps;
                DWORD dwCaps2;
                DWORD dwCaps3;
                DWORD dwCaps4;
                DWORD dwReserved2;
            } DDS_HEADER;

            DDS_HEADER header{
                124,        // hardcoded
                0x0002100F, // required flags + pitch + mipmapped
                static_cast<DWORD>(image_height),
                static_cast<DWORD>(image_width),
                static_cast<DWORD>(image_width * 4), // aka bytes per line
                1,
                1,
                {},
                // pixel format sub structure
                DDS_PIXELFORMAT{
                    32,   // size of pixel format structure, constant
                    0x41, // uncompressed RGB with alpha channel
                    0,    // compression mode (not used for uncompressed data)
                    32,
                    // bit masks for each channel, here for RGBA
                    0x000000FF,
                    0x0000FF00,
                    0x00FF0000,
                    0xFF000000,
                },
                0x1000, // simple texture with only one surface and no mipmaps
                0,      // additional surface data, unused
                0,      // unused
                0,      // unused
                0,
            };

            auto image_data_size = image_width * image_height * 4;
            auto data_size = 4 + sizeof(DDS_HEADER) + image_data_size;
            auto allocation_size = sizeof(FileInfo) + data_size;
            auto file_buffer = (char*)alloc_fun(allocation_size);

            FileInfo* file_info = new (file_buffer) FileInfo{};
            file_info->Data = file_buffer + sizeof(FileInfo);
            file_info->DataSize = data_size;
            file_info->AllocationSize = allocation_size;

            auto dds_image_data = file_buffer + sizeof(FileInfo);
            memcpy(dds_image_data, "DDS ", 4);
            memcpy(dds_image_data + 4, &header, sizeof(DDS_HEADER));
            memcpy(dds_image_data + 4 + sizeof(DDS_HEADER), image_data, image_data_size);

            stbi_image_free(image_data);

            return file_info;
        }
    }
    return nullptr;
}

using ReadEncryptedFileFun = FileInfo*(const char* file_path, AllocFun alloc_fun);
ReadEncryptedFileFun* g_read_encrypted_file_trampoline{nullptr};
FileInfo* read_encrypted_file(const char* file_path, AllocFun alloc_fun)
{
    if (auto file = g_OnLoadFile(file_path, alloc_fun))
    {
        return file;
    }
    return g_read_encrypted_file_trampoline(file_path, alloc_fun);
}

void register_on_load_file(LoadFileCallback on_load_file)
{
    if (g_read_encrypted_file_trampoline == nullptr && on_load_file != nullptr)
    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        auto off = find_inst(exe, "\x4c\x63\x78\x0c\x49\x8b\xcf\x44\x89\x68\x08"s, after_bundle) - 0x30;
        auto fun_start = decode_pc(exe, find_inst(exe, "\xe8"s, off), 1);

        g_read_encrypted_file_trampoline = (ReadEncryptedFileFun*)memory.at_exe(fun_start);

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_read_encrypted_file_trampoline, read_encrypted_file);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking ReadEncryptedFile: {}\n", error);
        }
    }
    g_OnLoadFile = on_load_file;
}
void register_get_image_file_path(GetImageFilePathCallback get_image_file_path)
{
    g_GetImageFilePath = get_image_file_path;
}

std::string get_image_file_path(std::string root_path, std::string relative_path)
{
    if (g_GetImageFilePath)
    {
        return g_GetImageFilePath(std::move(root_path), std::move(relative_path));
    }
    return root_path + '/' + relative_path;
}

bool create_d3d11_texture_from_file(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    get_device()->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    get_device()->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}
