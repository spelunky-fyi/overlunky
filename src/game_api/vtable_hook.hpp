#pragma once

#include <any>
#include <functional>
#include <optional>
#include <unordered_map>

void* register_hook_function(void*** vtable, size_t index, void* hook_function);
void unregister_hook_function(void*** vtable, size_t index);
void* get_hook_function(void*** vtable, size_t index);

template <class ClassT>
struct VDestructorDetour
{
    using VFunT = void(void*, bool);
    using DtorTaskT = std::function<void(void*)>;

    static void detour(ClassT* self, bool destroy)
    {
        if (s_Tasks.contains(self))
        {
            for (auto& task : s_Tasks[self])
            {
                task(self);
            }
        }
        s_Original(self, destroy);
    }

    inline static VFunT* s_Original = nullptr;
    inline static std::unordered_map<void*, std::vector<DtorTaskT>> s_Tasks{};
};

template <class VFunT>
requires std::is_function_v<VFunT> struct VTableDetour;
template <class RetT, class ClassT, class... ArgsT>
struct VTableDetour<RetT(ClassT*, ArgsT...)>
{
    using VFunT = RetT(ClassT*, ArgsT...);
    using DetourFunT = std::function<RetT(ClassT*, ArgsT..., VFunT*)>;

    static RetT detour(ClassT* self, ArgsT... args)
    {
        if constexpr (std::is_void_v<RetT>)
        {
            if (s_Functions.contains(self))
            {
                s_Functions[self](self, args..., s_Original);
                return;
            }
            s_Original(self, std::move(args)...);
        }
        else
        {
            if (s_Functions.contains(self))
            {
                return s_Functions[self](self, args..., s_Original);
            }
            return s_Original(self, std::move(args)...);
        }
    }

    inline static VFunT* s_Original = nullptr;
    inline static std::unordered_map<ClassT*, DetourFunT> s_Functions{};
};

template <class T, class HookFunT>
void hook_dtor(T* obj, HookFunT&& hook_fun, std::size_t dtor_index = 0)
{
    using DtorT = void(void*, bool);
    using DestructorDetourT = VDestructorDetour<T>;
    if (!get_hook_function((void***)obj, dtor_index))
    {
        DestructorDetourT::s_Original = (DtorT*)register_hook_function((void***)obj, dtor_index, (void*)&DestructorDetourT::detour);
    }
    DestructorDetourT::s_Tasks[obj].push_back(std::forward<HookFunT>(hook_fun));
}

template <class VTableFunT, class T, class HookFunT>
void hook_vtable(T* obj, HookFunT&& hook_fun, std::size_t vtable_index, std::size_t dtor_index = 0)
{
    using DetourT = VTableDetour<VTableFunT>;
    if (!get_hook_function((void***)obj, vtable_index))
    {
        DetourT::s_Original = (VTableFunT*)register_hook_function((void***)obj, vtable_index, (void*)&DetourT::detour);
    }
    DetourT::s_Functions[obj] = hook_fun;

    hook_dtor(obj, [](void* self)
              { DetourT::s_Functions.erase((T*)self); });
}
