#pragma once

#include <algorithm> // for copy
#include <cstddef>   // for size_t
#include <cstdint>   // for uint32_t

#include "hook_handler.hpp"   // for CallbackType
#include "script/safe_cb.hpp" // for FrontBinder, BackBinder
#include "util.hpp"           // for function_signature, LiteralString

// For usage examples see vtables_lua.cpp
// Note: Can bind types that are default constructible

template <
    LiteralString Name,
    std::uint32_t Index,
    function_signature Signature,
    instance_of<BackBinder> BindBack = BackBinder<>,
    bool DoHooks = true>
struct VTableEntry;

template <
    class SelfT,
    CallbackType CbType,
    class... VTableEntries>
struct HookableVTable;

#include "hookable_vtable.inl"
