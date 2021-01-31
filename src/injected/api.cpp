#include "memory.h"
#include "thread_utils.h"
#include "logger.h"
#include "entity.hpp"
#include "api.h"

#include <string>
#include <cstddef>

size_t API::find_api(Memory memory) {
    auto exe = memory.exe();
    auto after_bundle = memory.after_bundle;
    auto off = find_inst(exe, "\x48\x8B\x50\x10\x48\x89"s, after_bundle) - 5;
    off = off + (*(int32_t *) (&exe[off + 1])) + 5;

    return memory.at_exe(decode_pc(exe, off + 6));
}

API API::get(Memory memory) {
    auto api = (find_api(memory));
    auto off = decode_imm(
            memory.exe(),
            find_inst(
                    memory.exe(),
                    "\xBA\xF0\xFF\xFF\xFF\x41\xB8\x00\x00\x00\x90"s,
                    memory.after_bundle) +
            17);

    return API{
            (size_t *) api,
            off};
}
