#pragma once

#include <Windows.h>   // for GetModuleHandleA, LPVOID
#include <cstddef>     // for size_t, byte, NULL
#include <cstdint>     // for int32_t, int64_t, uint32_t, uint64_t, uint8_t
#include <memory>      // for unique_ptr
#include <string>      // for string, string_literals
#include <string_view> // for string_view

#include "search.hpp" // for find_after_bundle

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
        static Memory mem{ []() {
                auto exe = (size_t)GetModuleHandleA("Spel2.exe");

                // Skipping bundle for faster memory search
                auto after_bundle = find_after_bundle(exe);

                return Memory{
                    exe,
                    after_bundle,
                };
            }()
        };
        return mem;
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
struct RecoverableMemory
{
    size_t address;
    char* old_data;
    size_t size;
    bool prot_used;
};

LPVOID alloc_mem_rel32(size_t addr, size_t size);
void write_mem_prot(size_t addr, std::string_view payload, bool prot);
void write_mem_prot(size_t addr, std::string payload, bool prot);
void write_mem(size_t addr, std::string payload);
size_t function_start(size_t off, uint8_t outside_byte = '\xcc');
void write_mem_recoverable(std::string name, size_t addr, std::string_view payload, bool prot);
void recover_mem(std::string name, size_t addr = NULL);

template <typename T>
requires std::is_trivially_copyable_v<T>
    std::string_view to_le_bytes(const T& payload)
{
    return std::string_view{reinterpret_cast<const char*>(&payload), sizeof(payload)};
}

template <class T>
requires(std::is_trivially_copyable_v<T> && !std::is_same_v<T, std::string_view>) void write_mem_recoverable(std::string name, size_t addr, const T& payload, bool prot)
{
    write_mem_recoverable(name, addr, to_le_bytes(payload), prot);
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

template <class FunT, typename T>
FunT* vtable_find(T* obj, size_t index)
{
    void*** ptr = reinterpret_cast<void***>(obj);
    if (!ptr[0])
        return static_cast<FunT*>(nullptr);
    return reinterpret_cast<FunT*>(&ptr[0][index]);
}

#define ONCE(type)            \
    static bool once = false; \
    static type res;          \
    if (once)                 \
        return res;           \
    once = true;              \
    if (false)                \
        ;                     \
    else
