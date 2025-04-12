#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <type_traits> // for true_type, is_invocable_r_v
#include <vector>

template <class CallableT, class Signature>
requires(std::is_function_v<Signature>)
struct is_invocable_as : std::false_type
{
};
template <class CallableT, class RetT, class... ArgsT>
struct is_invocable_as<CallableT, RetT(ArgsT...)> : std::is_invocable_r<RetT, CallableT, ArgsT...>
{
};
template <class CallableT, class Signature>
inline constexpr auto is_invocable_as_v = is_invocable_as<CallableT, Signature>::value;

template <class FunT>
requires is_invocable_as_v<FunT, void()>
struct OnScopeExit
{
    OnScopeExit(FunT&& fun)
        : Fun{std::forward<FunT>(fun)}
    {
    }
    ~OnScopeExit()
    {
        Fun();
    }
    FunT Fun;
};

#define OL_CONCAT_IMPL(x, y) x##y
#define OL_CONCAT(x, y) OL_CONCAT_IMPL(x, y)
#define ON_SCOPE_EXIT(expr)                         \
    OnScopeExit OL_CONCAT(on_scope_exit_, __LINE__) \
    {                                               \
        [&]() { expr; }                             \
    }

const int OL_KEY_CTRL = 0x100;
const int OL_KEY_SHIFT = 0x200;
const int OL_KEY_ALT = 0x800;
const int OL_BUTTON_MOUSE = 0x400;
const int OL_MOUSE_WHEEL = 0x10;
const int OL_WHEEL_DOWN = 0x11;
const int OL_WHEEL_UP = 0x12;

struct EntityItem;

void create_box(std::vector<EntityItem> items);
void init_ui(struct ImGuiContext* ctx);
void reload_enabled_scripts();
