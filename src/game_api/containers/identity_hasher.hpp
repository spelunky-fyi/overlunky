#pragma once

#include <type_traits>
#include <cstddef>

template <class T = void>
struct identity_hasher
{
    [[nodiscard]] size_t operator()(const T& val) const noexcept
    {
        return static_cast<size_t>(val);
    }
};

template <>
struct identity_hasher<void>
{
    template <class T>
    [[nodiscard]] size_t operator()(const T& val) const noexcept
    {
        return static_cast<size_t>(val);
    }
};
