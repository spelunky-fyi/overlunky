#include "vtable_hook.hpp"

#include "memory.hpp"

#include <algorithm>
#include <vector>

struct VFunctionHook
{
    std::size_t vtable_index;
    void* original_function;
};
struct VTableHook
{
    void** vtable;
    std::vector<VFunctionHook> functions;
};
std::vector<VTableHook> g_TableHooks;

VTableHook* get_vtable_hook(void** vtable)
{
    auto it = std::find_if(g_TableHooks.begin(), g_TableHooks.end(), [vtable](const VTableHook& hook)
                           { return hook.vtable == vtable; });
    if (it != g_TableHooks.end())
    {
        return &*it;
    }
    return nullptr;
}
VFunctionHook* get_vfunction_hook(VTableHook& vtable_hook, size_t index)
{
    auto it = std::find_if(vtable_hook.functions.begin(), vtable_hook.functions.end(), [index](const VFunctionHook& hook)
                           { return hook.vtable_index == index; });
    if (it != vtable_hook.functions.end())
    {
        return &*it;
    }
    return nullptr;
}

void* register_hook_function(void*** vtable, size_t index, void* hook_function)
{
    VTableHook* vtable_hook = get_vtable_hook(*vtable);
    if (vtable_hook == nullptr)
    {
        g_TableHooks.push_back({*vtable});
        vtable_hook = &g_TableHooks.back();
    }
    if (get_vfunction_hook(*vtable_hook, index))
    {
        DEBUG("Multiple hooks to the same function are not allowed...");
    }
    else
    {
        void*& vtable_ptr = vtable_find<void*>(vtable, index);

        DWORD oldProtect;
        if (!VirtualProtect(reinterpret_cast<LPVOID>(reinterpret_cast<uintptr_t>(&vtable_ptr) & ~0xFFF), 0x1000, PAGE_READWRITE, &oldProtect))
        {
            PANIC("VirtualProtect error: {:#x}\n", GetLastError());
        }

        void* original_function = vtable_ptr;
        vtable_ptr = hook_function;
        vtable_hook->functions.push_back({index, original_function});
        return original_function;
    }
}
void unregister_hook_function(void*** vtable, size_t index)
{
    if (VTableHook* vtable_hook = get_vtable_hook(*vtable))
    {
        if (VFunctionHook* vfunction_hook = get_vfunction_hook(*vtable_hook, index))
        {
            void*& vtable_ptr = vtable_find<void*>(vtable, index);
            vtable_ptr = vfunction_hook->original_function;
            vtable_hook->functions.erase(vtable_hook->functions.begin() + (vfunction_hook - &vtable_hook->functions.front()));
            return;
        }
    }
}
void* get_hook_function(void*** vtable, size_t index)
{
    if (VTableHook* vtable_hook = get_vtable_hook(*vtable))
    {
        if (VFunctionHook* vfunction_hook = get_vfunction_hook(*vtable_hook, index))
        {
            return vfunction_hook->original_function;
        }
    }
    return nullptr;
}
