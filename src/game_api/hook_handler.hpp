#pragma once

#include <cstdint>    // for uint32_t
#include <functional> // function
#include <vector>     // vector

enum class CallbackType
{
    None,
    Normal,
    Entity,
    Screen,
    Theme,
    HotKey
};

template <
    class SelfT,
    CallbackType CbType>
struct HookHandler
{
    struct Hook
    {
        std::uint32_t callback_id;
        std::uint32_t aux_id;

        bool operator==(const Hook&) const = default;
    };
    std::vector<Hook> hooks;
    std::vector<Hook> dtor_hooks;
    std::vector<Hook> cleared_hooks;

    void add_hook(std::uint32_t callback_id, std::uint32_t aux_id)
    {
        hook_dtor(aux_id);
        hooks.push_back(Hook{callback_id, aux_id});
    }
    void clear_hook(std::uint32_t callback_id, std::uint32_t aux_id)
    {
        cleared_hooks.push_back(Hook{callback_id, aux_id});
    }

    bool is_hook_cleared(std::uint32_t callback_id, std::uint32_t aux_id) const
    {
        return std::find(cleared_hooks.begin(), cleared_hooks.end(), Hook{callback_id, aux_id}) != cleared_hooks.end();
    }

    void clear_pending()
    {
        for (auto [callback_id, aux_id] : cleared_hooks)
        {
            if (std::find(hooks.begin(), hooks.end(), Hook{callback_id, aux_id}) != hooks.end())
            {
                unhook(callback_id, aux_id);
            }
        }
        cleared_hooks.clear();
    }
    void clear_all_hooks()
    {
        cleared_hooks = std::move(hooks);
        for (auto [callback_id, aux_id] : cleared_hooks)
        {
            unhook(callback_id, aux_id);
        }
        hooks.clear();
        cleared_hooks.clear();
    }

    template <class CallableT>
    static void set_hook_dtor_impl(CallableT&& callable)
    {
        assert(hook_dtor_impl == nullptr);
        hook_dtor_impl = std::forward<CallableT>(callable);
    }
    template <class CallableT>
    static void set_unhook_impl(CallableT&& callable)
    {
        assert(unhook_impl == nullptr);
        unhook_impl = std::forward<CallableT>(callable);
    }

  private:
    void pre_dtor(std::uint32_t aux_id)
    {
        const auto aux_id_equal = [aux_id](Hook& hook)
        { return hook.aux_id == aux_id; };

        [[maybe_unused]] auto num_erased_hooks = std::erase_if(hooks, aux_id_equal);
        assert(num_erased_hooks != 0);
        [[maybe_unused]] auto num_erased_dtors = std::erase_if(dtor_hooks, aux_id_equal);
        assert(num_erased_dtors == 1);
        std::erase_if(cleared_hooks, aux_id_equal);
    }
    void hook_dtor(std::uint32_t aux_id)
    {
        if (hook_dtor_impl != nullptr)
        {
            const auto aux_id_equal = [aux_id](Hook& hook)
            { return hook.aux_id == aux_id; };

            if (std::find_if(dtor_hooks.begin(), dtor_hooks.end(), aux_id_equal) == dtor_hooks.end())
            {
                std::uint32_t callback_id = hook_dtor_impl(aux_id, std::bind_front(&HookHandler::pre_dtor, this));
                dtor_hooks.push_back(Hook{callback_id, aux_id});
            }
        }
    }
    inline static std::function<std::uint32_t(std::uint32_t, std::function<void(std::uint32_t)>)> hook_dtor_impl{};

    void unhook(std::uint32_t callback_id, std::uint32_t aux_id)
    {
        unhook_impl(callback_id, aux_id);

        const auto aux_id_equal = [aux_id](Hook& hook)
        { return hook.aux_id == aux_id; };

        std::erase(hooks, Hook{callback_id, aux_id});
        if (std::count_if(hooks.begin(), hooks.end(), aux_id_equal) == 0)
        {
            auto it = std::find_if(dtor_hooks.begin(), dtor_hooks.end(), aux_id_equal);
            if (it != dtor_hooks.end())
            {
                unhook_impl(it->callback_id, aux_id);
                dtor_hooks.erase(it);
            }
        }
    }
    inline static std::function<void(std::uint32_t, std::uint32_t)> unhook_impl{};
};
