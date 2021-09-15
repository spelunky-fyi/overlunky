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

inline std::string_view trim(std::string_view str)
{
    constexpr std::string_view white_spaces = " \t\r\n\v\f";
    const auto begin = str.find_first_not_of(white_spaces);
    const auto end = str.find_last_not_of(white_spaces);
    return str.substr(begin, end - begin + 1);
}
