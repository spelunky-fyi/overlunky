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
    static Memory& get();

    size_t at_exe(size_t offset) const
    {
        return exe_ptr + offset;
    }

    char* exe()
    {
        return reinterpret_cast<char*>(exe_ptr);
    }
    size_t exe_address() const
    {
        return exe_ptr;
    }
    size_t after_bundle_address() const
    {
        return after_bundle;
    }
    static size_t decode_call(size_t off)
    {
        auto& memory = get();
        return off + (*(int32_t*)(&memory.exe()[off + 1])) + 5;
    }

  private:
    size_t exe_ptr;
    size_t after_bundle;
    Memory(size_t ptr, size_t ab)
        : exe_ptr(ptr), after_bundle(ab){};

    Memory(const Memory&) = delete;
    Memory& operator=(const Memory&) = delete;
    ~Memory(){};
};

[[nodiscard]] LPVOID alloc_mem_rel32(size_t addr, size_t size);
void write_mem_prot(size_t addr, std::string_view payload, bool prot);
void write_mem_prot(size_t addr, std::string payload, bool prot);
void write_mem(size_t addr, std::string payload);
size_t function_start(size_t off, uint8_t outside_byte = '\xcc');
// save copy of the oryginal memory so it can be later recovered via recover_mem
void save_mem_recoverable(std::string name, size_t addr, size_t size, bool prot);
void write_mem_recoverable(std::string name, size_t addr, std::string_view payload, bool prot);
void recover_mem(std::string name, size_t addr = NULL);
bool mem_written(std::string name);
std::string get_nop(size_t size, bool true_nop = false);

// similar to ExecutableMemory but writes automatic jump from and back, moves the code it replaces etc.
// it needs at least 5 bytes to move, use just_nop = true to nuke the oryginal code
// make sure that the first 5 bytes are not a destination for some jump (it's fine if it's exacly at the addr)
size_t patch_and_redirect(size_t addr, size_t replace_size, std::string_view payload, bool just_nop = false, size_t return_to_addr = 0, bool game_code_first = true);

template <typename T>
requires std::is_trivially_copyable_v<T>
std::string_view to_le_bytes(const T& payload)
{
    return std::string_view{reinterpret_cast<const char*>(&payload), sizeof(payload)};
}

template <class T>
requires(std::is_trivially_copyable_v<T> && !std::is_same_v<T, std::string_view>)
void write_mem_recoverable(std::string name, size_t addr, const T& payload, bool prot)
{
    write_mem_recoverable(name, addr, to_le_bytes(payload), prot);
}
template <class T>
requires(std::is_trivially_copyable_v<T> && !std::is_same_v<T, std::string_view>)
void write_mem_prot(size_t addr, const T& payload, bool prot)
{
    write_mem_prot(addr, to_le_bytes(payload), prot);
}
template <class T>
requires std::is_trivially_copyable_v<T>
void write_mem_prot(void* addr, const T& payload, bool prot)
{
    write_mem_prot((size_t)addr, to_le_bytes(payload), prot);
}

template <typename T>
inline T memory_read(size_t addr)
{
    return *(T*)(addr);
}

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
