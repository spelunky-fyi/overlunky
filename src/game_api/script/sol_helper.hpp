#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <tuple>

#include "member_function.hpp"

template <class T>
struct ZeroIndexArray
{
    using value_type = T;
    using iterator_category = std::contiguous_iterator_tag;
    using difference_type = size_t;
    using pointer = T*;
    using reference = T&;
    using iterator = T*;

    ZeroIndexArray(T* data, size_t size)
        : data(data), data_size(size){};

    ZeroIndexArray(std::span<T> arr)
    {
        data = arr.data();
        data_size = arr.size();
    };
    ~ZeroIndexArray()
    {
        data_size = 0;
    };

    T& operator[](int index)
    {
        return data[index];
    }
    T& operator=(T&& other) noexcept
    {
        // Guard self assignment
        if (this == &other)
            return *this;

        data = other.data;
        data_size = other.data_size;
        return *this;
    }
    iterator begin() const
    {
        return iterator(data);
    }
    iterator end() const
    {
        return iterator(data + data_size);
    }
    bool empty() const
    {
        return data_size == 0;
    }
    size_t size() const
    {
        return data_size;
    }
    T* data;
    size_t data_size{0};
};

namespace sol
{
template <class T>
struct is_container<ZeroIndexArray<T>> : std::true_type
{
};

template <class T>
struct usertype_container<ZeroIndexArray<T>>
{
    static int size(lua_State* L)
    {
        ZeroIndexArray<T>& v = sol::stack::get<ZeroIndexArray<T>&>(L, 1);
        return sol::stack::push(L, v.size());
    }
    // Used by default implementation
    static auto begin(lua_State*, ZeroIndexArray<T>& self)
    {
        return self.begin();
    }
    static auto end(lua_State*, ZeroIndexArray<T>& self)
    {
        return self.end();
    }
    static std::ptrdiff_t index_adjustment(lua_State*, ZeroIndexArray<T>&)
    {
        return 0;
    }
};
} // namespace sol

namespace
{
template <typename, typename, auto>
struct proFunc;

template <template <typename...> class C, typename... Ts, typename T, auto fun>
struct proFunc<C<Ts...>, T, fun>
{
    static auto func(sol::object obj, Ts... args)
    {
        (obj.as<T&>().*fun)(std::forward<Ts>(args)...);
        return obj;
    };
};
} // namespace

template <auto fun>
auto self_return()
{
    using FunT = decltype(fun);
    static_assert(std::is_member_function_pointer_v<FunT>);
    using T = typename MemberFun_t<FunT>::BaseType;
    using _tuple = typename MemberFun_t<FunT>::Parameters_Tuple;

    return proFunc<_tuple, T, fun>::func;
}
