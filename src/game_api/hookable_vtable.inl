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

#include "hookable_vtable.hpp"

template <
    LiteralString Name,
    std::uint32_t Index,
    function_signature Signature,
    instance_of<BackBinder> BindBack,
    bool DoHooks>
struct VTableEntry
{
    inline static constexpr auto MyRawName{Name};
    inline static constexpr auto MyName{Name.Str};
    inline static constexpr auto MyIndex{Index};
    using MySignature = Signature;
    using MyBindBack = BindBack;
    inline static constexpr auto MyDoHooks{DoHooks};
};

template <class T>
struct IsVTableEntry : std::false_type
{
};
template <
    LiteralString Name,
    std::uint32_t Index,
    function_signature Signature,
    instance_of<BackBinder> BindBack,
    bool DoHooks>
struct IsVTableEntry<VTableEntry<Name, Index, Signature, BindBack, DoHooks>> : std::true_type
{
};
template <class T>
concept vtable_entry = IsVTableEntry<T>::value;

template <class RetT>
using VTablePreRetT = std::conditional_t<std::is_void_v<RetT>, bool, std::optional<RetT>>;

template <vtable_entry OriginalEntry>
using DontHookVTableEntry = VTableEntry<
    OriginalEntry::MyRawName,
    OriginalEntry::MyIndex,
    typename OriginalEntry::MySignature,
    typename OriginalEntry::MyBindBack,
    false>;

template <
    vtable_entry Entry,
    class SelfT,
    CallbackType CbType>
struct VTableEntryImpl;
template <
    LiteralString Name,
    std::uint32_t Index,
    class RetT,
    class... ArgsT,
    instance_of<BackBinder> BindBack,
    bool DoHooks,
    class SelfT,
    CallbackType CbType>
struct VTableEntryImpl<VTableEntry<Name, Index, RetT(ArgsT...), BindBack, DoHooks>, SelfT, CbType>
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

    using FreePreSignature = VTablePreRetT<RetT>(SelfT*, ArgsT...);
    using FreePostSignature = void(SelfT*, ArgsT...);

    template <class VTableImpl>
    static void register_hooks([[maybe_unused]] auto lua_type, [[maybe_unused]] VTableImpl& vtable)
    {
        if constexpr (DoHooks)
        {
            lua_type[JoinLiteralStrings<"set_pre_", Name>().template to<std::string_view>()] = [&vtable](SelfT* self, sol::function fun)
            {
                std::uint32_t callback_id = vtable.reserve_callback_id(self);
                std::uint32_t aux_id = self->get_aux_id();
                vtable.template set_pre<FreeSignature, MyIndex>(
                    self,
                    callback_id,
                    make_safe_clearable_cb<FreePreSignature, MyCallbackType>(
                        std::move(fun),
                        callback_id,
                        aux_id,
                        FrontBinder<>{},
                        BindBack{}));

                auto backend = LuaBackend::get_calling_backend();
                backend->MyHookHandler::add_hook(callback_id, aux_id);
                return callback_id;
            };
            lua_type[JoinLiteralStrings<"set_post_", Name>().template to<std::string_view>()] = [&vtable](SelfT* self, sol::function fun)
            {
                std::uint32_t callback_id = vtable.reserve_callback_id(self);
                std::uint32_t aux_id = self->get_aux_id();
                vtable.template set_post<FreeSignature, MyIndex>(
                    self,
                    callback_id,
                    make_safe_clearable_cb<FreePostSignature, MyCallbackType>(
                        std::move(fun),
                        callback_id,
                        aux_id,
                        FrontBinder<>{},
                        BindBack{}));

                auto backend = LuaBackend::get_calling_backend();
                backend->MyHookHandler::add_hook(callback_id, aux_id);
                return callback_id;
            };
        }
    }
};

template <function_signature Signature>
struct PreHookInfos;
template <function_signature Signature>
struct PostHookInfos;

template <class RetT, class... ArgsT>
struct PreHookInfos<RetT(ArgsT...)>
{
    using FunT = VTablePreRetT<RetT>(ArgsT...);
    std::unordered_map<std::uint32_t, std::vector<HookWithId<FunT>>> hooks;
};

template <class RetT, class... ArgsT>
struct PostHookInfos<RetT(ArgsT...)>
{
    using FunT = void(ArgsT...);
    std::unordered_map<std::uint32_t, std::vector<HookWithId<FunT>>> hooks;
};

template <class... T>
struct type_list;

template <
    function_signature Signature,
    bool Include>
struct VTableHooksSignature
{
    using MySignature = Signature;
    inline static constexpr auto MyInclude{Include};
};

template <class T>
struct IsVTableHooksSignature : std::false_type
{
};
template <
    function_signature Signature,
    bool Include>
struct IsVTableHooksSignature<VTableHooksSignature<Signature, Include>> : std::true_type
{
};
template <class T>
concept vtable_hook_signature = IsVTableHooksSignature<T>::value;

template <class VTableHooksTuple, vtable_hook_signature... Signatures>
struct UniquePreHookInfosImpl;
template <vtable_hook_signature... UniqueSignatures, vtable_hook_signature Signature>
struct UniquePreHookInfosImpl<type_list<UniqueSignatures...>, Signature>
    : PreHookInfos<typename UniqueSignatures::MySignature>...,
      PreHookInfos<typename Signature::MySignature>
{
    using UniqueSignaturesList = type_list<typename UniqueSignatures::MySignature..., typename Signature::MySignature>;
};
template <vtable_hook_signature... UniqueSignatures, vtable_hook_signature Signature, vtable_hook_signature... Signatures>
struct UniquePreHookInfosImpl<type_list<UniqueSignatures...>, Signature, Signatures...>
    : std::conditional_t<
          Signature::MyInclude && !(is_same_function_v<typename Signature::MySignature, typename Signatures::MySignature> || ...),
          UniquePreHookInfosImpl<type_list<UniqueSignatures..., Signature>, Signatures...>,
          UniquePreHookInfosImpl<type_list<UniqueSignatures...>, Signatures...>>
{
};

template <class Signature>
struct make_void_return;
template <class RetT, class... ArgsT>
struct make_void_return<RetT(ArgsT...)>
{
    using type = void(ArgsT...);
};
template <
    function_signature Signature,
    bool Include>
struct make_void_return<VTableHooksSignature<Signature, Include>>
{
    using type = VTableHooksSignature<typename make_void_return<Signature>::type, Include>;
};
template <class Signature>
using make_void_return_t = typename make_void_return<Signature>::type;

template <class VTableHooksTuple, vtable_hook_signature... Signatures>
struct UniquePostHookInfosImpl;
template <vtable_hook_signature... UniqueSignatures, vtable_hook_signature Signature>
struct UniquePostHookInfosImpl<type_list<UniqueSignatures...>, Signature>
    : PostHookInfos<typename UniqueSignatures::MySignature>...,
      PostHookInfos<typename Signature::MySignature>
{
    using UniqueSignaturesList = type_list<typename UniqueSignatures::MySignature..., typename Signature::MySignature>;
};
template <vtable_hook_signature... UniqueSignatures, vtable_hook_signature Signature, vtable_hook_signature... Signatures>
struct UniquePostHookInfosImpl<type_list<UniqueSignatures...>, Signature, Signatures...>
    : std::conditional_t<
          Signature::MyInclude && !(is_same_args_v<typename Signature::MySignature, typename Signatures::MySignature> || ...),
          UniquePostHookInfosImpl<type_list<UniqueSignatures..., Signature>, Signatures...>,
          UniquePostHookInfosImpl<type_list<UniqueSignatures...>, Signatures...>>
{
};

template <vtable_hook_signature... Signatures>
using UniquePreHookInfos = UniquePreHookInfosImpl<type_list<>, Signatures...>;
template <vtable_hook_signature... Signatures>
using UniquePostHookInfos = UniquePostHookInfosImpl<type_list<>, Signatures...>;

template <
    class SelfT,
    vtable_hook_signature... Signatures>
struct VTableHookInfos : UniquePreHookInfos<Signatures...>,
                         UniquePostHookInfos<make_void_return_t<Signatures>...>
{
    using UniquePreSignatures = typename UniquePreHookInfos<Signatures...>::UniqueSignaturesList;
    using UniquePostSignatures = typename UniquePostHookInfos<Signatures...>::UniqueSignaturesList;

    std::uint32_t callback_count{0};

    template <function_signature Signature>
    auto& get_pre()
    {
        return static_cast<PreHookInfos<Signature>*>(this)->hooks;
    }
    template <function_signature Signature>
    auto& get_post()
    {
        return static_cast<PostHookInfos<make_void_return_t<Signature>>*>(this)->hooks;
    }
    void unhook(std::uint32_t callback_id)
    {
        unhook_pre_impl<UniquePreSignatures>::call(*this, callback_id);
        unhook_post_impl<UniquePostSignatures>::call(*this, callback_id);
    }

    std::vector<std::uint32_t> hooks;
    bool is_hooked(std::uint32_t index) const
    {
        return std::find(hooks.begin(), hooks.end(), index) != hooks.end();
    }
    void set_hooked(std::uint32_t index)
    {
        if (!is_hooked(index))
        {
            hooks.push_back(index);
        }
    }

    bool dtor_hooked{false};
    bool is_dtor_hooked() const
    {
        return dtor_hooked;
    }
    void set_dtor_hooked()
    {
        dtor_hooked = true;
    }

  private:
    template <class UniqueSignaturesList>
    struct unhook_pre_impl;
    template <class... UniqueSignatures>
    struct unhook_pre_impl<type_list<UniqueSignatures...>>
    {
        static void call(VTableHookInfos& self, std::uint32_t callback_id)
        {
            auto unhook_impl = [callback_id](auto& pre_hooks)
            {
                for (auto& [index, funcs] : pre_hooks)
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
            auto unhook_impl = [callback_id](auto& post_hooks)
            {
                for (auto& [index, funcs] : post_hooks)
                {
                    std::erase_if(funcs, [callback_id](auto& hook)
                                  { return hook.id == callback_id; });
                }
            };
            (unhook_impl(self.get_post<UniqueSignatures>()), ...);
        }
    };
};

template <class T>
struct IsHookableVTable : std::false_type
{
};
template <
    class SelfT,
    CallbackType CbType,
    vtable_entry... VTableEntries>
struct IsHookableVTable<HookableVTable<SelfT, CbType, VTableEntries...>> : std::true_type
{
};
template <class T>
concept hookable_vtable = IsHookableVTable<T>::value;

template <class T, class FunT>
concept invokable_as_pre_fun = invokable_as<T, typename PreHookInfos<FunT>::FunT>;
template <class T, class FunT>
concept invokable_as_post_fun = invokable_as<T, typename PostHookInfos<FunT>::FunT>;

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

            lua_type["set_pre_virtual"] = [this, lua_type](SelfT* self, std::uint32_t index, sol::function fun) -> sol::optional<std::uint32_t>
            {
                if (index_to_name.contains(index))
                {
                    return lua_type["set_pre_"s + index_to_name[index]](self, std::move(fun));
                }
                return sol::nullopt;
            };
            lua_type["set_post_virtual"] = [this, lua_type](SelfT* self, std::uint32_t index, sol::function fun) -> sol::optional<std::uint32_t>
            {
                if (index_to_name.contains(index))
                {
                    return lua_type["set_post_"s + index_to_name[index]](self, std::move(fun));
                }
                return sol::nullopt;
            };
            lua_type["clear_virtual"] = [](SelfT* self, std::uint32_t callback_id)
            {
                std::uint32_t aux_id = self->get_aux_id();
                auto backend = LuaBackend::get_calling_backend();
                backend->MyHookHandler::clear_hook(callback_id, aux_id);
            };
        }

        if (!table_name.empty())
        {
            auto toupper = [](std::string_view strv)
            {
                std::string str{strv};
                std::transform(str.begin(), str.end(), str.begin(), [](char c)
                               { return static_cast<char>(::toupper(c)); });
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
    void remove_hooks(SelfT* obj)
    {
        my_hooks.erase(obj);
    }
    void hook_dtor_for_hook_info_cleanup(SelfT* obj, MyHookInfos& hook_info)
    {
        if (!hook_info.is_dtor_hooked())
        {
            // We would like to just call
            // hook_vtable_impl<void(SelfT*), dtor_index>::call(*this, obj);
            // but that won't work for derived classes due to how the hookable
            // vtables are setup, so this is instead a workaround for not using
            // the existing code to hook the dtor
            hook_dtor(
                obj,
                [this](void* obj_inner)
                {
                    // If the dtor was additionally hooked we need to delay cleanup
                    // to after all those other hooks have executed, otherwise they
                    // will be lost, aka never executed
                    MyHookInfos& hook_info = get_hooks((SelfT*)obj_inner);
                    if (!hook_info.is_hooked(dtor_index))
                    {
                        remove_hooks((SelfT*)obj_inner);
                    }
                },
                dtor_index);
            hook_info.set_dtor_hooked();
        }
    }
    std::uint32_t reserve_callback_id(SelfT* obj)
    {
        MyHookInfos& hook_info = get_hooks(obj);
        return hook_info.callback_count++;
    }
    template <function_signature Signature, std::uint32_t Index, invokable_as_pre_fun<Signature> CallableT>
    void set_pre(SelfT* obj, std::uint32_t callback_id, CallableT pre_fun)
    {
        // Note: Not using get_hook_function, we assume no one else hooks this (R.I.P. whoever is using this before us)
        MyHookInfos& hook_info = get_hooks(obj);
        hook_dtor_for_hook_info_cleanup(obj, hook_info);
        if (!hook_info.is_hooked(Index))
        {
            hook_vtable_impl<Signature, Index>::call(*this, obj);
            hook_info.set_hooked(Index);
        }
        hook_info.template get_pre<Signature>()[Index].push_back({callback_id, std::move(pre_fun)});
    }
    template <function_signature Signature, std::uint32_t Index, invokable_as_post_fun<Signature> CallableT>
    void set_post(SelfT* obj, std::uint32_t callback_id, CallableT post_fun)
    {
        // Note: Not using get_hook_function, we assume no one else hooks this (R.I.P. whoever is using this before us)
        MyHookInfos& hook_info = get_hooks(obj);
        hook_dtor_for_hook_info_cleanup(obj, hook_info);
        if (!hook_info.is_hooked(Index))
        {
            hook_vtable_impl<Signature, Index>::call(*this, obj);
            hook_info.set_hooked(Index);
        }
        hook_info.template get_post<Signature>()[Index].push_back({callback_id, std::move(post_fun)});
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
            hook_vtable<void(SelfT*, ArgsT...), Index>(
                obj,
                [&self](SelfT* inner_obj, ArgsT... args, void (*original)(SelfT*, ArgsT...))
                {
                    MyHookInfos& hook_info = self.get_hooks(inner_obj);

                    bool skip_orig = false;
                    for (auto& [id, prefun] : hook_info.template get_pre<void(SelfT*, ArgsT...)>()[Index])
                    {
                        skip_orig = prefun(inner_obj, args...);
                    }
                    if (!skip_orig)
                    {
                        original(inner_obj, args...);
                    }
                    for (auto& [id, postfun] : hook_info.template get_post<void(SelfT*, ArgsT...)>()[Index])
                    {
                        postfun(inner_obj, args...);
                    }

                    // cleanup hooks, delayed until all other dtor hooks have run
                    if constexpr (Index == dtor_index)
                    {
                        self.remove_hooks(inner_obj);
                    }
                });
        }
    };
    template <class RetT, class... ArgsT, std::uint32_t Index>
    struct hook_vtable_impl<RetT(SelfT*, ArgsT...), Index>
    {
        static void call(HookableVTable& self, SelfT* obj)
        {
            hook_vtable<RetT(SelfT*, ArgsT...), Index>(
                obj,
                [&self](SelfT* inner_obj, ArgsT... args, RetT (*original)(SelfT*, ArgsT...))
                {
                    MyHookInfos& hook_info = self.get_hooks(inner_obj);

                    std::optional<RetT> return_value;
                    for (auto& [id, prefun] : hook_info.template get_pre<RetT(SelfT*, ArgsT...)>()[Index])
                    {
                        auto ret = prefun(inner_obj, args...);
                        if (ret.has_value() && !return_value.has_value())
                        {
                            return_value = ret.value();
                        }
                    }
                    if (!return_value.has_value())
                    {
                        return_value = original(inner_obj, args...);
                    }
                    for (auto& [id, postfun] : hook_info.template get_post<RetT(SelfT*, ArgsT...)>()[Index])
                    {
                        postfun(inner_obj, args...);
                    }

                    // dtor should never have a return value, so shouldn't reach here
                    assert(Index != dtor_index);

                    return return_value.value();
                });
        }
    };

    std::unordered_map<std::uint32_t, std::string> index_to_name{
        std::pair{VTableEntries::MyIndex, VTableEntries::MyRawName.template to<std::string>()}...,
    };
    inline static constexpr std::uint32_t dtor_index{
        []() -> std::uint32_t
        {
            std::vector index_and_name{
                std::pair{VTableEntries::MyIndex, VTableEntries::MyRawName.template to<std::string>()}...,
            };

            using namespace std::string_view_literals;
            for (const auto& [idx, name] : index_and_name)
            {
                if (name == "dtor"sv)
                {
                    return idx;
                }
            }
            throw "Needs dtor...";
        }()};
};

template <class T>
struct BaseTableEntry;
template <hookable_vtable HookableVTable>
struct BaseTableEntry<HookableVTable>
{
    using type = HookableVTable;
};
template <vtable_entry VTableEntry>
struct BaseTableEntry<VTableEntry>
{
    using type = DontHookVTableEntry<VTableEntry>;
};
template <class T>
using base_table_entry_t = typename BaseTableEntry<T>::type;

template <
    class SelfT,
    CallbackType CbType,
    class BaseSelfT,
    CallbackType BaseCbType,
    class... BaseVTableEntries,
    class... VTableEntries>
requires(std::is_same_v<SelfT, BaseSelfT> && CbType == BaseCbType)
struct HookableVTable<SelfT, CbType, HookableVTable<BaseSelfT, BaseCbType, BaseVTableEntries...>, VTableEntries...>
    : HookableVTable<SelfT, CbType, base_table_entry_t<BaseVTableEntries>..., VTableEntries...>
{
    using MyBase = HookableVTable<SelfT, CbType, base_table_entry_t<BaseVTableEntries>..., VTableEntries...>;
    using MyBase::MyBase;
};
