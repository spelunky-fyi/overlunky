#pragma once

#include <type_traits>

template <class FunT>
requires std::is_invocable_r_v<void, FunT> struct OnScopeExit
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
