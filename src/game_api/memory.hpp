#pragma once

#include <unordered_map>

#include <Windows.h>

#include "search.hpp"

using namespace std::string_literals;
class ExecutableMemory
{
  public:
    ExecutableMemory(std::string_view raw_code);

    ExecutableMemory() = default;
    ExecutableMemory(const ExecutableMemory&) = delete;
    ExecutableMemory(ExecutableMemory&&) noexcept = default;
    ExecutableMemory& operator=(const ExecutableMemory&) = delete;
    ExecutableMemory& operator=(ExecutableMemory&&) noexcept = default;

    std::byte* get() const
    {
        return code.get();
    }

    template <class Ret, class... Args>
    using func_ptr = Ret (*)(Args...);
    template <class Ret, class... Args>
    explicit operator func_ptr<Ret, Args...>() const
    {
        return (Ret(*)(Args...))(code.get());
    }

  private:
    struct deleter_t
    {
        void operator()(std::byte* mem) const;
    };
    using storage_t = std::unique_ptr<std::byte, deleter_t>;
    storage_t code;
};

struct Memory
{
    size_t exe_ptr;
    size_t after_bundle;

    static Memory& get()
    {
        static Memory MEMORY = Memory{};
        static bool INIT = false;

        if (!INIT)
        {
            auto exe = (size_t)GetModuleHandleA("Spel2.exe");

            // Skipping bundle for faster memory search
            auto after_bundle = find_after_bundle(exe);

            MEMORY = Memory{
                exe,
                after_bundle,
            };
        }

        return MEMORY;
    }

    size_t at_exe(size_t offset)
    {
        return exe_ptr + offset;
    }

    char* exe()
    {
        return (char*)exe_ptr;
    }
    static size_t decode_call(size_t off)
    {
        auto memory = get();
        return off + (*(int32_t*)(&memory.exe()[off + 1])) + 5;
    }
};

namespace
{
size_t round_up(size_t i, size_t div)
{
    return ((i + div - 1) / div) * div;
}

template <typename T>
requires std::is_trivially_copyable_v<T>
    std::string_view to_le_bytes(const T& payload)
{
    return std::string_view{reinterpret_cast<const char*>(&payload), sizeof(payload)};
}

void write_mem_prot(size_t addr, std::string_view payload, bool prot)
{
    DWORD old_protect = 0;
    auto page = addr & ~0xFFF;
    auto size = round_up((addr + payload.size() - page), 0x1000);
    if (prot)
    {
        VirtualProtect((void*)page, size, PAGE_EXECUTE_READWRITE, &old_protect);
    }
    memcpy((void*)addr, payload.data(), payload.size());
    if (prot)
    {
        VirtualProtect((LPVOID)page, size, old_protect, &old_protect);
    }
}

template <class T>
requires(std::is_trivially_copyable_v<T> && !std::is_same_v<T, std::string_view>) void write_mem_prot(size_t addr, const T& payload, bool prot)
{
    write_mem_prot(addr, to_le_bytes(payload), prot);
}

template <class T>
requires std::is_trivially_copyable_v<T> void write_mem_prot(void* addr, const T& payload, bool prot)
{
    write_mem_prot((size_t)addr, to_le_bytes(payload), prot);
}

void write_mem_prot(size_t addr, std::string payload, bool prot)
{
    write_mem_prot(addr, std::string_view{payload}, prot);
}

[[maybe_unused]] void write_mem(size_t addr, std::string payload)
{
    write_mem_prot(addr, payload, false);
}

struct ReversibleMemory
{
    size_t address;
    std::vector<char> old_data;
    bool prot_used;
};

std::unordered_map<std::string, std::vector<ReversibleMemory>> original_memory;

void write_mem_reversible(std::string name, size_t addr, std::string_view payload, bool prot)
{
    if (!original_memory.contains(name))
    {
        std::vector<char> old_data((char*)addr, (char*)addr + payload.size());
        original_memory[name] = std::vector<ReversibleMemory>{{addr, old_data, prot}};
    }
    else
    {
        bool new_addr = true;
        for (auto& it : original_memory[name])
        {
            if (it.address == addr)
            {
                new_addr = false;
                break;
            }
        }
        if (new_addr)
        {
            std::vector<char> old_data((char*)addr, (char*)addr + payload.size());
            original_memory[name].push_back(ReversibleMemory{addr, old_data, prot});
        }
    }
    write_mem_prot(addr, payload, prot);
}
template <class T>
requires(std::is_trivially_copyable_v<T> && !std::is_same_v<T, std::string_view>) void write_mem_reversible(std::string name, size_t addr, const T& payload, bool prot)
{
    write_mem_reversible(name, addr, to_le_bytes(payload), prot);
}
void reverse_mem(std::string name)
{
    if (original_memory.contains(name))
    {
        for (auto& it : original_memory[name])
        {
            write_mem_prot(it.address, std::string_view{it.old_data.data(), it.old_data.size()}, it.prot_used);
        }
    }
}

#define DEFINE_ACCESSOR(name, type)                       \
    [[maybe_unused]] inline type read_##name(size_t addr) \
    {                                                     \
        return *(type*)(addr);                            \
    }

DEFINE_ACCESSOR(u8, uint8_t);

DEFINE_ACCESSOR(u32, uint32_t);

DEFINE_ACCESSOR(u64, uint64_t);

DEFINE_ACCESSOR(i32, int32_t);

DEFINE_ACCESSOR(i64, int64_t);

DEFINE_ACCESSOR(f32, float);

[[maybe_unused]] size_t function_start(size_t off)
{
    off &= ~0xf;
    while (read_u8(off - 1) != 0xcc)
    {
        off -= 0x10;
    }
    return off;
}

template <class FunT, typename T>
FunT* vtable_find(T* obj, size_t index)
{
    void*** ptr = reinterpret_cast<void***>(obj);
    if (!ptr[0])
        return static_cast<FunT*>(nullptr);
    return reinterpret_cast<FunT*>(&ptr[0][index]);
}

LPVOID alloc_mem_rel32(size_t addr, size_t size)
{
    const size_t limit_addr = Memory::get().exe_ptr;
    LPVOID new_array = nullptr;
    size_t test_addr;

    test_addr = addr + 0x10000; // dunno why
    if (test_addr <= INT32_MAX)
        test_addr = 8;
    else
        test_addr -= INT32_MAX;

    for (; test_addr < limit_addr; test_addr += 0x100000)
    {
        new_array = VirtualAlloc((LPVOID)test_addr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (new_array)
            break;
    }
    return new_array;
}
}; // namespace

#define ONCE(type)            \
    static bool once = false; \
    static type res;          \
    if (once)                 \
        return res;           \
    once = true;              \
    if (false)                \
        ;                     \
    else
