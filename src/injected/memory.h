#pragma once

#include "search.h"
#include <unordered_map>
using namespace std::string_literals;

namespace {
    size_t round_up(size_t i, size_t div) {
        return ((i + div - 1) / div) * div;
    }

    void write_mem_prot(size_t addr, std::string payload, bool prot) {
        DWORD old_protect = 0;
        auto page = addr & ~0xFFF;
        auto size = round_up((addr + payload.size() - page), 0x1000);
        if (prot) {
            VirtualProtect(
                    (void *) page,
                    size,
                    PAGE_EXECUTE_READWRITE,
                    &old_protect);
        }
        memcpy((void *) addr, payload.data(), payload.size());
        if (prot) {
            VirtualProtect((LPVOID) page, size, old_protect, &old_protect);
        }
    }

    void write_mem(size_t addr, std::string payload) {
        write_mem_prot(addr, payload, false);
    }

#define DEFINE_ACCESSOR(name, type) \
    type read_##name(size_t addr)   \
    {                               \
        return *(type *)(addr);     \
    }

    DEFINE_ACCESSOR(u8, uint8_t);

    DEFINE_ACCESSOR(u32, uint32_t);

    DEFINE_ACCESSOR(u64, uint64_t);

    DEFINE_ACCESSOR(f32, float);
}; // namespace

struct Memory {
    size_t exe_ptr;
    size_t after_bundle;

    static Memory &get() {
        static Memory MEMORY = Memory{};
        static bool INIT = false;

        if (!INIT) {
            auto exe = (size_t) GetModuleHandleA("Spel2.exe");

            // Skipping bundle for faster memory search
            auto after_bundle = find_after_bundle(exe);

            MEMORY = Memory{
                    exe,
                    after_bundle,
            };
        }

        return MEMORY;
    }

    size_t at_exe(size_t offset) {
        return exe_ptr + offset;
    }

    char *exe() {
        return (char *) exe_ptr;
    }
};

static size_t
decode_call(size_t off) {
    auto memory = Memory::get();
    return off + (*(int32_t *) (&memory.exe()[off + 1])) + 5;
}

#define ONCE(type) static bool once = false; static type res; if(once) return res; else if(once=true)

