#pragma once

#include <cstddef>       // for size_t
#include <functional>    // for equal_to, function, _Func_class
#include <new>           // for operator new
#include <type_traits>   // for forward
#include <unordered_map> // for unordered_map, _Umap_traits<>::allocator_type
#include <utility>       // for min, max
#include <vector>        // for vector, _Vector_iterator, allocator, _Vecto...

#include "util.hpp" // for function_signature

void* register_hook_function(void*** vtable, size_t index, void* hook_function);
void unregister_hook_function(void*** vtable, size_t index);
void* get_hook_function(void*** vtable, size_t index);

struct VDestructorDetour
{
    using VFunT = void(void*, bool);
    using DtorTaskT = std::function<void(void*)>;

    static void detour(void* self, bool destroy)
    {
        if (s_Tasks.contains(self))
        {
            for (auto& task : s_Tasks[self])
            {
                task(self);
            }
            s_Tasks.erase(self);
        }
        s_OriginalDtors[*(void***)self](self, destroy);
    }

    inline static std::unordered_map<void**, VFunT*> s_OriginalDtors{};
    inline static std::unordered_map<void*, std::vector<DtorTaskT>> s_Tasks{};
};

template <function_signature VFunT, size_t Index>
struct VTableDetour;

template <class RetT, class ClassT, class... ArgsT, std::size_t Index>
struct VTableDetour<RetT(ClassT*, ArgsT...), Index>
{
    using VFunT = RetT(ClassT*, ArgsT...);
    using DetourFunT = std::function<RetT(ClassT*, ArgsT..., VFunT*)>;

    static RetT detour(ClassT* self, ArgsT... args)
    {
        void** vtable = *(void***)self;
        if constexpr (std::is_void_v<RetT>)
        {
            if (s_Functions.contains(self))
            {
                s_Functions[self](self, args..., s_Originals[vtable]);
                return;
            }
            s_Originals[vtable](self, std::move(args)...);
        }
        else
        {
            if (s_Functions.contains(self))
            {
                return s_Functions[self](self, args..., s_Originals[vtable]);
            }
            return s_Originals[vtable](self, std::move(args)...);
        }
    }

    inline static std::unordered_map<void**, VFunT*> s_Originals{};
    inline static std::unordered_map<ClassT*, DetourFunT> s_Functions{};
};

template <class HookFunT>
void hook_dtor(void* obj, HookFunT&& hook_fun, std::size_t dtor_index = 0)
{
    using DestructorDetourT = VDestructorDetour;
    using DtorT = DestructorDetourT::VFunT;
    void*** vtable = (void***)obj;
    if (!get_hook_function(vtable, dtor_index))
    {
        DestructorDetourT::s_OriginalDtors[*vtable] = (DtorT*)register_hook_function(vtable, dtor_index, (void*)&DestructorDetourT::detour);
    }
    DestructorDetourT::s_Tasks[obj].push_back(std::forward<HookFunT>(hook_fun));
}

template <class VTableFunT, std::size_t VTableIndex, class T, class HookFunT>
void hook_vtable(T* obj, HookFunT&& hook_fun, std::size_t dtor_index = 0)
{
    using DetourT = VTableDetour<VTableFunT, VTableIndex>;
    void*** vtable = (void***)obj;
    if (!get_hook_function(vtable, VTableIndex))
    {
        DetourT::s_Originals[*vtable] = (VTableFunT*)register_hook_function(vtable, VTableIndex, (void*)&DetourT::detour);
    }
    DetourT::s_Functions[obj] = hook_fun;

    hook_dtor(
        obj, [](void* self)
        { DetourT::s_Functions.erase((T*)self); },
        dtor_index);
}

template <class VTableFunT, std::size_t VTableIndex, class T, class HookFunT>
void hook_vtable_no_dtor(T* obj, HookFunT&& hook_fun)
{
    using DetourT = VTableDetour<VTableFunT, VTableIndex>;
    void*** vtable = (void***)obj;
    if (!get_hook_function(vtable, VTableIndex))
    {
        DetourT::s_Originals[*vtable] = (VTableFunT*)register_hook_function(vtable, VTableIndex, (void*)&DetourT::detour);
    }
    DetourT::s_Functions[obj] = hook_fun;
}

// Stupid runtime wrappers, we can think about something better in the future 🤷‍♀️
template <class VTableFunT, class T, class HookFunT>
void hook_vtable(T* obj, HookFunT&& hook_fun, std::size_t vtable_index, std::size_t dtor_index = 0)
{
#define STUPID_WRAPPER(i)                                                              \
    case i:                                                                            \
        hook_vtable<VTableFunT, i>(obj, std::forward<HookFunT>(hook_fun), dtor_index); \
        return
    switch (vtable_index)
    {
        STUPID_WRAPPER(0);
        STUPID_WRAPPER(1);
        STUPID_WRAPPER(2);
        STUPID_WRAPPER(3);
        STUPID_WRAPPER(4);
        STUPID_WRAPPER(5);
        STUPID_WRAPPER(6);
        STUPID_WRAPPER(7);
        STUPID_WRAPPER(8);
        STUPID_WRAPPER(9);
        STUPID_WRAPPER(10);
        STUPID_WRAPPER(11);
        STUPID_WRAPPER(12);
        STUPID_WRAPPER(13);
        STUPID_WRAPPER(14);
        STUPID_WRAPPER(15);
        STUPID_WRAPPER(16);
        STUPID_WRAPPER(17);
        STUPID_WRAPPER(18);
        STUPID_WRAPPER(19);
        STUPID_WRAPPER(20);
        STUPID_WRAPPER(21);
        STUPID_WRAPPER(22);
        STUPID_WRAPPER(23);
        STUPID_WRAPPER(24);
        STUPID_WRAPPER(25);
        STUPID_WRAPPER(26);
        STUPID_WRAPPER(27);
        STUPID_WRAPPER(28);
        STUPID_WRAPPER(29);
        STUPID_WRAPPER(30);
        STUPID_WRAPPER(31);
        STUPID_WRAPPER(32);
        STUPID_WRAPPER(33);
        STUPID_WRAPPER(34);
        STUPID_WRAPPER(35);
        STUPID_WRAPPER(36);
        STUPID_WRAPPER(37);
        STUPID_WRAPPER(38);
        STUPID_WRAPPER(39);
        STUPID_WRAPPER(40);
        STUPID_WRAPPER(41);
        STUPID_WRAPPER(42);
        STUPID_WRAPPER(43);
        STUPID_WRAPPER(44);
        STUPID_WRAPPER(45);
        STUPID_WRAPPER(46);
        STUPID_WRAPPER(47);
        STUPID_WRAPPER(48);
        STUPID_WRAPPER(49);
        STUPID_WRAPPER(50);
        STUPID_WRAPPER(51);
        STUPID_WRAPPER(52);
        STUPID_WRAPPER(53);
        STUPID_WRAPPER(54);
        STUPID_WRAPPER(55);
        STUPID_WRAPPER(56);
        STUPID_WRAPPER(57);
        STUPID_WRAPPER(58);
        STUPID_WRAPPER(59);
    default:
        std::abort();
    }
#undef STUPID_WRAPPER
}

template <class VTableFunT, class T, class HookFunT>
void hook_vtable_no_dtor(T* obj, HookFunT&& hook_fun, std::size_t vtable_index)
{
#define STUPID_WRAPPER(i)                                                          \
    case i:                                                                        \
        hook_vtable_no_dtor<VTableFunT, i>(obj, std::forward<HookFunT>(hook_fun)); \
        return
    switch (vtable_index)
    {
        STUPID_WRAPPER(0);
        STUPID_WRAPPER(1);
        STUPID_WRAPPER(2);
        STUPID_WRAPPER(3);
        STUPID_WRAPPER(4);
        STUPID_WRAPPER(5);
        STUPID_WRAPPER(6);
        STUPID_WRAPPER(7);
        STUPID_WRAPPER(8);
        STUPID_WRAPPER(9);
        STUPID_WRAPPER(10);
        STUPID_WRAPPER(11);
        STUPID_WRAPPER(12);
        STUPID_WRAPPER(13);
        STUPID_WRAPPER(14);
        STUPID_WRAPPER(15);
        STUPID_WRAPPER(16);
        STUPID_WRAPPER(17);
        STUPID_WRAPPER(18);
        STUPID_WRAPPER(19);
        STUPID_WRAPPER(20);
        STUPID_WRAPPER(21);
        STUPID_WRAPPER(22);
        STUPID_WRAPPER(23);
        STUPID_WRAPPER(24);
        STUPID_WRAPPER(25);
        STUPID_WRAPPER(26);
        STUPID_WRAPPER(27);
        STUPID_WRAPPER(28);
        STUPID_WRAPPER(29);
        STUPID_WRAPPER(30);
        STUPID_WRAPPER(31);
        STUPID_WRAPPER(32);
        STUPID_WRAPPER(33);
        STUPID_WRAPPER(34);
        STUPID_WRAPPER(35);
        STUPID_WRAPPER(36);
        STUPID_WRAPPER(37);
        STUPID_WRAPPER(38);
        STUPID_WRAPPER(39);
        STUPID_WRAPPER(40);
        STUPID_WRAPPER(41);
        STUPID_WRAPPER(42);
        STUPID_WRAPPER(43);
        STUPID_WRAPPER(44);
        STUPID_WRAPPER(45);
        STUPID_WRAPPER(46);
        STUPID_WRAPPER(47);
        STUPID_WRAPPER(48);
        STUPID_WRAPPER(49);
        STUPID_WRAPPER(50);
        STUPID_WRAPPER(51);
        STUPID_WRAPPER(52);
        STUPID_WRAPPER(53);
        STUPID_WRAPPER(54);
        STUPID_WRAPPER(55);
        STUPID_WRAPPER(56);
        STUPID_WRAPPER(57);
        STUPID_WRAPPER(58);
        STUPID_WRAPPER(59);
    default:
        std::abort();
    }
#undef STUPID_WRAPPER
}
