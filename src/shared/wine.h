#include <windows.h>

bool detect_wine()
{
    static const HMODULE hntdll = GetModuleHandle("ntdll.dll");
    if (!hntdll)
        return false;
    static const void* wgv = GetProcAddress(hntdll, "wine_get_version");
    if (!wgv)
        return false;
    return true;
}
