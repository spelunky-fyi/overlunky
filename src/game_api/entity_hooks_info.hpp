#pragma once

#include <cstdint>    // for uint32_t
#include <functional> // for function

template <class FunT>
struct HookWithId
{
    uint32_t id;
    std::function<FunT> fun;
};
