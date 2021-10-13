#include "memory.hpp"

ExecutableMemory::ExecutableMemory(std::string_view raw_code)
{
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);

    auto const page_size = system_info.dwPageSize;
    auto const alloc_size = (raw_code.size() / page_size + 1) * page_size;

    auto const memory = (std::byte*)VirtualAlloc(nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (memory)
    {
        std::memcpy(memory, raw_code.data(), raw_code.size());

        DWORD dummy;
        VirtualProtect(memory, alloc_size, PAGE_EXECUTE_READ, &dummy);

        code = storage_t{memory};
    }
    else
    {
        std::exit(1);
    }
}

void ExecutableMemory::deleter_t::operator()(std::byte* mem) const
{
    VirtualFree(mem, 0, MEM_RELEASE);
}
