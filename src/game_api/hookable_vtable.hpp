#pragma once

#include <cstddef>       // for size_t
#include <cstdint>       // for uint32_t
#include <string>        // for string, string_literals
#include <string_view>   // for string_view
#include <type_traits>   // false_type, is_function_v, ...
#include <unordered_map> // unordered_map

#include <sol/sol.hpp> // table

#include "entity_hooks_info.hpp" // for HookWithId
#include "hook_handler.hpp"      // for HookHandler
#include "script/safe_cb.hpp"    // for make_safe_clearable_cb
#include "vtable_hook.hpp"       // for get_hook_function, ...

template <std::size_t N>
struct VFunctionName
{
    consteval VFunctionName(const char (&name)[N])
    {
        for (std::size_t i = 0; i < N; i++)
        {
            Name[i] = name[i];
        }
    }

    char Name[N]{};
};

template <
    VFunctionName Name,
    std::uint32_t Index,
    class Signature,
    bool DoHooks = true>
requires(std::is_function_v<Signature>)
struct VTableEntry
{
    inline static constexpr auto MyRawName{Name};
    inline static constexpr auto MyName{Name.Name};
    inline static constexpr auto MyIndex{Index};
    using MySignature = Signature;
    inline static constexpr auto MyDoHooks{DoHooks};
};

template <class T>
struct IsVTableEntry : std::false_type
{
};
template <
    VFunctionName Name,
    std::uint32_t Index,
    class Signature,
    bool DoHooks>
struct IsVTableEntry<VTableEntry<Name, Index, Signature, DoHooks>> : std::true_type
{
};
template <class T>
inline constexpr auto IsVTableEntryV = IsVTableEntry<T>::value;

template <class OriginalEntry>
requires(IsVTableEntryV<OriginalEntry>)
using DontHookVTableEntry = VTableEntry<OriginalEntry::MyRawName, OriginalEntry::MyIndex, typename OriginalEntry::MySignature, false>;

template <
    class Entry,
    class SelfT,
    CallbackType CbType>
requires(IsVTableEntryV<Entry>)
struct VTableEntryImpl;
template <
    VFunctionName Name,
    std::uint32_t Index,
    class RetT,
    class... ArgsT,
    bool DoHooks,
    class SelfT,
    CallbackType CbType>
struct VTableEntryImpl<VTableEntry<Name, Index, RetT(ArgsT...), DoHooks>, SelfT, CbType>
    : VTableEntry<Name, Index, RetT(ArgsT...)>
{
    using MyBase = VTableEntry<Name, Index, RetT(ArgsT...)>;
    using MyBase::MyIndex;
    using MyBase::MyName;
    inline static constexpr auto MyCallbackType{CbType};
    inline static constexpr auto MyDoHooks{DoHooks};

    using MyHookHandler = HookHandler<SelfT, CbType>;
    using MemberSignature = RetT(ArgsT...);
    using FreeSignature = RetT(SelfT*, ArgsT...);

    using PreRetT = std::conditional_t<std::is_void_v<RetT>, bool, std::optional<RetT>>;
    using FreePreSignature = PreRetT(SelfT*, ArgsT...);
    using FreePostSignature = void(SelfT*, ArgsT...);

    template <class VTableImpl>
    static void register_hooks([[maybe_unused]] auto lua_type, [[maybe_unused]] VTableImpl& vtable)
    {
        if constexpr (DoHooks)
        {
            using namespace std::string_literals;

            lua_type["set_pre_"s + MyName] = [&vtable](SelfT* self, sol::function fun)
            {
                std::uint32_t callback_id = vtable.reserve_callback_id(self);
                std::uint32_t aux_id = self->get_aux_id();
                vtable.set_pre<FreeSignature, MyIndex>(
                    self,
                    callback_id,
                    make_safe_clearable_cb<FreePreSignature, MyCallbackType>(std::move(fun), callback_id, aux_id));

                auto backend = LuaBackend::get_calling_backend();
                backend->MyHookHandler::add_hook(callback_id, aux_id);
                return callback_id;
            };
            lua_type["set_post_"s + MyName] = [&vtable](SelfT* self, sol::function fun)
            {
                std::uint32_t callback_id = vtable.reserve_callback_id(self);
                std::uint32_t aux_id = self->get_aux_id();
                vtable.set_post<FreeSignature, MyIndex>(
                    self,
                    callback_id,
                    make_safe_clearable_cb<FreePostSignature, MyCallbackType>(std::move(fun), callback_id, aux_id));

                auto backend = LuaBackend::get_calling_backend();
                backend->MyHookHandler::add_hook(callback_id, aux_id);
                return callback_id;
            };
        }
    }
};

template <class Signature>
requires(std::is_function_v<Signature>)
struct PreHookInfos;
template <class Signature>
requires(std::is_function_v<Signature>)
struct PostHookInfos;

template <class... ArgsT>
struct PreHookInfos<void(ArgsT...)>
{
    std::unordered_map<std::uint32_t, std::vector<HookWithId<bool(ArgsT...)>>> hooks;
};
template <class RetT, class... ArgsT>
struct PreHookInfos<RetT(ArgsT...)>
{
    using PreRetT = std::conditional_t<std::is_void_v<RetT>, bool, std::optional<RetT>>;
    std::unordered_map<std::uint32_t, std::vector<HookWithId<PreRetT(ArgsT...)>>> hooks;
};

template <class RetT, class... ArgsT>
struct PostHookInfos<RetT(ArgsT...)>
{
    std::unordered_map<std::uint32_t, std::vector<HookWithId<void(ArgsT...)>>> hooks;
};

template <class... T>
struct type_list;

template <typename HookInfosTuple, typename... Signatures>
struct UniquePreHookInfosImpl;
template <typename... UniqueSignatures, typename Signature>
struct UniquePreHookInfosImpl<type_list<UniqueSignatures...>, Signature>
    : PreHookInfos<typename UniqueSignatures::MySignature>...,
      PreHookInfos<typename Signature::MySignature>
{
    using UniqueSignaturesList = type_list<typename UniqueSignatures::MySignature..., typename Signature::MySignature>;
};
template <typename... UniqueSignatures, typename Signature, typename... Signatures>
struct UniquePreHookInfosImpl<type_list<UniqueSignatures...>, Signature, Signatures...>
    : std::conditional_t<
          Signature::MyInclude && !(is_same_function_v<typename Signature::MySignature, typename Signatures::MySignature> || ...),
          UniquePreHookInfosImpl<type_list<UniqueSignatures..., Signature>, Signatures...>,
          UniquePreHookInfosImpl<type_list<UniqueSignatures...>, Signatures...>>
{
};

template <typename HookInfosTuple, typename... Signatures>
struct UniquePostHookInfosImpl;
template <typename... UniqueSignatures, typename Signature>
struct UniquePostHookInfosImpl<type_list<UniqueSignatures...>, Signature>
    : PostHookInfos<typename UniqueSignatures::MySignature>...,
      PostHookInfos<typename Signature::MySignature>
{
    using UniqueSignaturesList = type_list<typename UniqueSignatures::MySignature..., typename Signature::MySignature>;
};
template <typename... UniqueSignatures, typename Signature, typename... Signatures>
struct UniquePostHookInfosImpl<type_list<UniqueSignatures...>, Signature, Signatures...>
    : std::conditional_t<
          Signature::MyInclude && !(is_same_args_v<typename Signature::MySignature, typename Signatures::MySignature> || ...),
          UniquePostHookInfosImpl<type_list<UniqueSignatures..., Signature>, Signatures...>,
          UniquePostHookInfosImpl<type_list<UniqueSignatures...>, Signatures...>>
{
};

template <class... Signatures>
using UniquePreHookInfos = UniquePreHookInfosImpl<type_list<>, Signatures...>;
template <class... Signatures>
using UniquePostHookInfos = UniquePostHookInfosImpl<type_list<>, Signatures...>;

template <
    class Signature,
    bool Include>
struct VTableHooksSignature
{
    using MySignature = Signature;
    inline static constexpr auto MyInclude{Include};
};

template <
    class SelfT,
    class... Signatures>
struct VTableHookInfos : UniquePreHookInfos<Signatures...>,
                         UniquePostHookInfos<Signatures...>
{
    using UniquePreSignatures = UniquePreHookInfos<Signatures...>::UniqueSignaturesList;
    using UniquePostSignatures = UniquePostHookInfos<Signatures...>::UniqueSignaturesList;

    std::uint32_t callback_count{0};

    template <class Signature>
    requires(std::is_function_v<Signature>)
    auto& get_pre()
    {
        return static_cast<PreHookInfos<Signature>*>(this)->hooks;
    }
    template <class Signature>
    requires(std::is_function_v<Signature>)
    auto& get_post()
    {
        return get_post_impl<Signature>::call(*this);
    }
    void unhook(std::uint32_t callback_id)
    {
        unhook_pre_impl<UniquePreSignatures>::call(*this, callback_id);
        unhook_post_impl<UniquePostSignatures>::call(*this, callback_id);
    }

  private:
    template <class Signature>
    struct get_post_impl;
    template <class RetT, class... ArgsT>
    struct get_post_impl<RetT(ArgsT...)>
    {
        static auto& call(VTableHookInfos& self)
        {
            return static_cast<PostHookInfos<void(ArgsT...)>&>(self).hooks;
        }
    };

    template <class UniqueSignaturesList>
    struct unhook_pre_impl;
    template <class... UniqueSignatures>
    struct unhook_pre_impl<type_list<UniqueSignatures...>>
    {
        static void call(VTableHookInfos& self, std::uint32_t callback_id)
        {
            auto unhook_impl = [callback_id](auto& hooks)
            {
                for (auto& [index, funcs] : hooks)
                {
                    std::erase_if(funcs, [callback_id](auto& hook)
                                  { return hook.id == callback_id; });
                }
            };
            (unhook_impl(self.get_pre<UniqueSignatures>()), ...);
        }
    };

    template <class UniqueSignaturesList>
    struct unhook_post_impl;
    template <class... UniqueSignatures>
    struct unhook_post_impl<type_list<UniqueSignatures...>>
    {
        static void call(VTableHookInfos& self, std::uint32_t callback_id)
        {
            auto unhook_impl = [callback_id](auto& hooks)
            {
                for (auto& [index, funcs] : hooks)
                {
                    std::erase_if(funcs, [callback_id](auto& hook)
                                  { return hook.id == callback_id; });
                }
            };
            (unhook_impl(self.get_post<UniqueSignatures>()), ...);
        }
    };
};

template <
    class SelfT,
    CallbackType CbType,
    class... VTableEntries>
struct HookableVTable
{
    HookableVTable() = delete;
    HookableVTable(const HookableVTable&) = delete;
    HookableVTable(HookableVTable&&) = delete;
    HookableVTable& operator=(const HookableVTable&) = delete;
    HookableVTable& operator=(HookableVTable&&) = delete;

    HookableVTable(
        sol::state& lua,
        auto lua_type,
        std::string table_name = "")
    {
        (VTableEntryImpl<VTableEntries, SelfT, CbType>::register_hooks(lua_type, *this), ...);

        {
            using namespace std::string_literals;

            lua_type["set_pre_virtual"] = [lua_type](SelfT* self, std::uint32_t index, sol::function fun) -> sol::optional<std::uint32_t>
            {
                const char* virtual_name;
                bool exists = (get_name_from_index_impl<VTableEntries>(index, &virtual_name) || ...);
                if (exists)
                {
                    return lua_type["set_pre_"s + virtual_name](self, std::move(fun));
                }
                return sol::nullopt;
            };
            lua_type["set_post_virtual"] = [lua_type](SelfT* self, std::uint32_t index, sol::function fun) -> sol::optional<std::uint32_t>
            {
                const char* virtual_name;
                bool exists = (get_name_from_index_impl<VTableEntries>(index, &virtual_name) || ...);
                if (exists)
                {
                    return lua_type["set_post_"s + virtual_name](self, std::move(fun));
                }
                return sol::nullopt;
            };
            lua_type["clear_virtual"] = [](std::int32_t aux_id, std::uint32_t callback_id)
            {
                auto backend = LuaBackend::get_calling_backend();
                backend->MyHookHandler::clear_hook(callback_id, aux_id);
            };
        }

        if (!table_name.empty())
        {
            auto toupper = [](std::string_view strv)
            {
                std::string str{strv};
                std::transform(str.begin(), str.end(), str.begin(), ::toupper);
                return str;
            };
            auto table = lua[table_name];
            if (table == sol::nil)
            {
                table = lua.create_named_table(table_name);
            }
            ((table[toupper(VTableEntries::MyName)] = VTableEntries::MyIndex), ...);
        }
    }

    // clang-format off
    using MyHookInfos = VTableHookInfos<SelfT, 
        VTableHooksSignature<
            typename VTableEntryImpl<VTableEntries, SelfT, CbType>::FreeSignature, 
            VTableEntries::MyDoHooks
        >...>;
    // clang-format on
    std::unordered_map<SelfT*, MyHookInfos> my_hooks;

    using MyHookHandler = HookHandler<SelfT, CbType>;

    MyHookInfos& get_hooks(SelfT* obj)
    {
        return my_hooks[obj];
    }
    std::uint32_t reserve_callback_id(SelfT* obj)
    {
        MyHookInfos& hook_info = get_hooks(obj);
        return hook_info.callback_count++;
    }
    template <class Signature, std::uint32_t Index, class CallableT>
    requires(std::is_function_v<Signature>)
    void set_pre(SelfT* obj, std::uint32_t callback_id, CallableT pre_fun)
    {
        if (!get_hook_function((void***)obj, (uint8_t)Index))
        {
            hook_vtable_impl<Signature, Index>::call(*this, obj);
        }

        MyHookInfos& hook_info = get_hooks(obj);
        hook_info.get_pre<Signature>()[Index].push_back({callback_id, std::move(pre_fun)});
    }
    template <class Signature, std::uint32_t Index, class CallableT>
    requires(std::is_function_v<Signature>)
    void set_post(SelfT* obj, std::uint32_t callback_id, CallableT post_fun)
    {
        if (!get_hook_function((void***)obj, (uint8_t)Index))
        {
            hook_vtable_impl<Signature, Index>::call(*this, obj);
        }

        MyHookInfos& hook_info = get_hooks(obj);
        hook_info.get_post<Signature>()[Index].push_back({callback_id, std::move(post_fun)});
    }
    void unhook(SelfT* obj, std::uint32_t callback_id)
    {
        MyHookInfos& hook_info = get_hooks(obj);
        hook_info.unhook(callback_id);
    }

  private:
    template <class Signature, std::uint32_t Index>
    struct hook_vtable_impl;
    template <class... ArgsT, std::uint32_t Index>
    struct hook_vtable_impl<void(SelfT*, ArgsT...), Index>
    {
        static void call(HookableVTable& self, SelfT* obj)
        {
            hook_vtable<void(SelfT*, ArgsT...)>(
                obj,
                [&self](SelfT* inner_obj, ArgsT... args, void (*original)(SelfT*, ArgsT...))
                {
                    MyHookInfos& hook_info = self.get_hooks(inner_obj);

                    bool skip_orig = false;
                    for (auto& [id, prefun] : hook_info.get_pre<void(SelfT*, ArgsT...)>()[Index])
                    {
                        skip_orig = prefun(inner_obj, args...);
                    }
                    if (!skip_orig)
                    {
                        original(inner_obj, args...);
                    }
                    for (auto& [id, postfun] : hook_info.get_post<void(SelfT*, ArgsT...)>()[Index])
                    {
                        postfun(inner_obj, args...);
                    }
                },
                (uint8_t)Index);
        }
    };
    template <class RetT, class... ArgsT, std::uint32_t Index>
    struct hook_vtable_impl<RetT(SelfT*, ArgsT...), Index>
    {
        static void call(HookableVTable self, SelfT* obj)
        {
            hook_vtable<RetT(SelfT*, ArgsT...)>(
                obj,
                [&self](SelfT* inner_obj, ArgsT... args, RetT (*original)(SelfT*, ArgsT...))
                {
                    MyHookInfos& hook_info = self.get_hooks(inner_obj);

                    std::optional<RetT> return_value;
                    for (auto& [id, prefun] : hook_info.get_pre<RetT(SelfT*, ArgsT...)>()[Index])
                    {
                        auto ret = prefun(inner_obj);
                        if (ret.has_value() && !return_value.has_value())
                        {
                            return_value = ret.value();
                        }
                    }
                    if (!return_value.has_value())
                    {
                        return_value = original(inner_obj);
                    }
                    for (auto& [id, postfun] : hook_info.get_post<RetT(SelfT*, ArgsT...)>()[Index])
                    {
                        postfun(inner_obj);
                    }
                    return return_value.value();
                },
                (uint8_t)Index);
        }
    };

    template <class Entry>
    static bool get_name_from_index_impl(std::uint32_t index, const char** out)
    {
        if (index == Entry::MyIndex)
        {
            *out = Entry::MyName;
            return true;
        }
        return false;
    }
};

template <class HookableVTable>
struct BaseTableEntry
{
    using type = HookableVTable;
};
template <class VTableEntry>
requires(IsVTableEntryV<VTableEntry>)
struct BaseTableEntry<VTableEntry>
{
    using type = DontHookVTableEntry<VTableEntry>;
};
template <class T>
using BaseTableEntryT = typename BaseTableEntry<T>::type;

template <
    class SelfT,
    CallbackType CbType,
    class BaseSelfT,
    CallbackType BaseCbType,
    class... BaseVTableEntries,
    class... VTableEntries>
requires(std::is_same_v<SelfT, BaseSelfT> && CbType == BaseCbType)
struct HookableVTable<SelfT, CbType, HookableVTable<BaseSelfT, BaseCbType, BaseVTableEntries...>, VTableEntries...>
    : HookableVTable<SelfT, CbType, BaseTableEntryT<BaseVTableEntries>..., VTableEntries...>
{
    using MyBase = HookableVTable<SelfT, CbType, BaseTableEntryT<BaseVTableEntries>..., VTableEntries...>;
    using MyBase::MyBase;
};
