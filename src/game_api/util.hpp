#pragma once

#include <type_traits>

template<class FunT>
requires std::is_invocable_r_v<void, FunT>
struct OnScopeExit
{
    OnScopeExit(FunT&& fun)
        : Fun{ std::forward<FunT>(fun) }
    {
    }
    ~OnScopeExit()
    {
        Fun();
    }
    FunT Fun;
};
