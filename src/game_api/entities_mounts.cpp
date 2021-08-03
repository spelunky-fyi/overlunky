#include "entities_mounts.hpp"

using Carry = void (*)(Entity*, Entity*);

Carry get_carry()
{
    ONCE(Carry)
    {
        auto memory = Memory::get();
        size_t off = memory.after_bundle;
        do
        {
            // call load_item; if(rax + 0x10e != 0x0B) { ... }
            off = find_inst(memory.exe(), "\x80\xB8\x0E\x01\x00\x00\x0B"s, off + 1);
        } while (read_u8(memory.at_exe(off - 5)) != 0xE8);
        off = find_inst(memory.exe(), "\xE8"s, off + 1);

        return res = (Carry)memory.at_exe(Memory::decode_call(off));
    }
}

void Mount::carry(Movable* rider)
{
    auto carry = (get_carry());
    rider->move_state = 0x11;
    return carry(this, rider);
}

void Mount::tame(bool value)
{
    write_mem(pointer() + 0x149, to_le_bytes(value));
    flags = flags | 0x20000;
}
