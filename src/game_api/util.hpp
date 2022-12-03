#pragma once

#include <type_traits> // for true_type, is_invocable_r_v

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

inline std::string_view trim(std::string_view str)
{
    constexpr std::string_view white_spaces = " \t\r\n\v\f";
    const auto begin = str.find_first_not_of(white_spaces);
    const auto end = str.find_last_not_of(white_spaces);
    return str.substr(begin, end - begin + 1);
}

template <class T, auto Mutex>
class GlobalMutexProtectedResource
{
  public:
    GlobalMutexProtectedResource() = delete;
    explicit GlobalMutexProtectedResource(const GlobalMutexProtectedResource& rhs)
        : Object{rhs}
    {
    }
    explicit GlobalMutexProtectedResource(const T& rhs)
        : Object{rhs}
    {
    }
    explicit GlobalMutexProtectedResource(T&& rhs) noexcept
        : Object{std::move(rhs)}
    {
    }
    explicit GlobalMutexProtectedResource(GlobalMutexProtectedResource&& rhs) noexcept
        : Object{std::move(rhs.Object)}
    {
    }

    template <class... Types>
    explicit GlobalMutexProtectedResource(Types&&... Args) noexcept
        : Object{std::forward<Types>(Args)...}
    {
    }

    using MutexT = std::decay_t<decltype(*Mutex)>;
    using ValueT = std::remove_pointer_t<T>;

    template <class U>
    class LockedResource
    {
        friend class GlobalMutexProtectedResource;

      public:
        LockedResource(LockedResource&& rhs)
            : ObjectRef{rhs.ObjectRef}, Lock{std::move(rhs.Lock)}
        {
        }

        U& operator*()
        {
            return ObjectRef;
        }
        U* operator->()
        {
            return &ObjectRef;
        }
        U* get()
        {
            return &ObjectRef;
        }

      private:
        LockedResource(U& object, MutexT& mutex)
            : ObjectRef{object}, Lock{mutex}
        {
        }

        U& ObjectRef;
        std::unique_lock<MutexT> Lock;
    };

    LockedResource<ValueT> Lock()
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return LockedResource<ValueT>{*Object, *Mutex};
        }
        else
        {
            return LockedResource<ValueT>{Object, *Mutex};
        }
    }
    template <class U>
    LockedResource<U> LockAs()
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return LockedResource<U>{static_cast<U&>(*Object), *Mutex};
        }
        else
        {
            return LockedResource<U>{static_cast<U&>(Object), *Mutex};
        }
    }

  private:
    T Object;
};

// clang-format off
template <template <typename...> class Template, typename T>
struct is_instantiation_of : std::false_type {};
template <template <typename...> class Template, typename... Args>
struct is_instantiation_of<Template, Template<Args...>> : std::true_type {};
template <template <typename...> class Template, typename T>
inline constexpr auto is_instantiation_of_v = is_instantiation_of<Template, T>::value;
template <typename T, template <typename...> class Template>
concept instance_of = is_instantiation_of_v<Template, T>;
// clang-format on

template <class T>
struct unwrap_optional
{
    using type = T;
};
template <class T>
struct unwrap_optional<std::optional<T>>
{
    using type = typename unwrap_optional<T>::type;
};
template <class T>
struct unwrap_optional<sol::optional<T>>
{
    using type = typename unwrap_optional<T>::type;
};
template <class T>
using unwrap_optional_t = typename unwrap_optional<T>::type;

template <class T>
inline constexpr auto is_optional_v = is_instantiation_of_v<std::optional, T> || is_instantiation_of_v<sol::optional, T>;

template <class LhsSignature, class RhsSignature>
requires(std::is_function_v<LhsSignature> && std::is_function_v<RhsSignature>)
struct is_same_function : std::false_type
{
};
template <class RetT, class... ArgsT>
struct is_same_function<RetT(ArgsT...), RetT(ArgsT...)> : std::true_type
{
};
template <class LhsSignature, class RhsSignature>
inline constexpr auto is_same_function_v = is_same_function<LhsSignature, RhsSignature>::value;

template <class LhsSignature, class RhsSignature>
requires(std::is_function_v<LhsSignature> && std::is_function_v<RhsSignature>)
struct is_same_args : std::false_type
{
};
template <class LhsRetT, class RhsRetT, class... ArgsT>
struct is_same_args<LhsRetT(ArgsT...), RhsRetT(ArgsT...)> : std::true_type
{
};
template <class LhsSignature, class RhsSignature>
inline constexpr auto is_same_args_v = is_same_args<LhsSignature, RhsSignature>::value;

template <class FunT>
concept function_signature = std::is_function_v<FunT>;

template <std::size_t N>
struct LiteralString
{
    consteval LiteralString() = default;
    consteval LiteralString(const char (&str)[N])
    {
        std::copy(std::begin(str), std::end(str), std::begin(Str));
    }

    template <class T>
    constexpr T to() const
    {
        if constexpr (requires { T{Str, Size}; })
        {
            return T{Str, Size};
        }
        else if constexpr (requires { T{std::begin(Str), std::end(Str)}; })
        {
            return T{std::begin(Str), std::end(Str)};
        }
    }

    char Str[N]{};
    size_t Size{N - 1};
};

template <LiteralString Left, LiteralString Right>
consteval auto JoinLiteralStrings()
{
    LiteralString<Left.Size + Right.Size + 1> result{};
    std::copy(std::begin(Left.Str), std::end(Left.Str), std::begin(result.Str));
    std::copy(std::begin(Right.Str), std::end(Right.Str), std::begin(result.Str) + Left.Size);
    return result;
}
