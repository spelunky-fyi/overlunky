#include "level_api.hpp"

#include "layer.hpp"
#include "logger.h"
#include "memory.hpp"
#include "rpc.hpp"

#include <Windows.h>
#include <detours.h>

std::uint32_t g_last_tile_code_id;
std::uint32_t g_current_tile_code_id;

using HandleTileCodeFun = void(LevelGenSystem*, std::uint32_t, std::uint64_t, float, float, std::uint8_t);
HandleTileCodeFun* g_handle_tile_code_trampoline{nullptr};
void handle_tile_code(LevelGenSystem* _this, std::uint32_t tile_code, std::uint64_t _ull_0, float x, float y, std::uint8_t layer)
{
    // TODO: Hook pre-level-gen-spawn here to allow changing/blocking spawns

    if (tile_code > g_last_tile_code_id && tile_code < g_current_tile_code_id)
    {
        // TODO: Handle all new tilecodes here

        // Just spawns a bomb-box for testing when encountering a non-vanilla code
        State::get().layer(layer)->spawn_entity(514, x, y, false, 0.0f, 0.0f, false);
    }
    else
    {
        g_handle_tile_code_trampoline(_this, tile_code, _ull_0, x, y, layer);
    }

    // TODO: Hook post-level-gen-spawn here to all interacting with default spawned things
}

void LevelGenSystem::init()
{
    {
        auto& tile_codes_map = tile_codes();

        // Getting the last id like this in case the game decides to skip some ids so that last_id != tile_codes.size()
        auto max_id = 0u;
        for (auto& [name, def] : tile_codes_map)
        {
            max_id = std::max(def.id, max_id);
        }

        // The game uses last id to check if the tilecode is valid using a != instead of a <
        // So we can safely use anything larger than last tile id
        g_last_tile_code_id = max_id + 1;
    }

    g_current_tile_code_id = g_last_tile_code_id + 1;

    {
        auto memory = Memory::get();
        auto exe = memory.exe();
        auto after_bundle = memory.after_bundle;

        auto off = find_inst(exe, "\x0f\xb6\xbc\x24\x28\x01\x00\x00"s, after_bundle) - 0x30;
        auto fun_start = find_inst(exe, "\x4c\x8b\xdc"s, off);

        g_handle_tile_code_trampoline = (HandleTileCodeFun*)memory.at_exe(fun_start);

        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach((void**)&g_handle_tile_code_trampoline, handle_tile_code);

        const LONG error = DetourTransactionCommit();
        if (error != NO_ERROR)
        {
            DEBUG("Failed hooking HandleTileCode: {}\n", error);
        }
    }
}

std::optional<std::uint32_t> LevelGenSystem::get_tile_code(const std::string& tile_code)
{
    auto& tile_codes_map = tile_codes();
    auto it = tile_codes_map.find(tile_code);
    if (it != tile_codes_map.end())
    {
        return it->second.id;
    }
    return {};
}
std::uint32_t LevelGenSystem::define_tile_code(std::string tile_code)
{
    if (auto existing = get_tile_code(tile_code))
    {
        return existing.value();
    }

    auto& tile_code_map = *(std::unordered_map<std::string, TileCodeDef>*)((size_t)this + 0x88);

    // TODO: This should be forwarded to the instantiation of this operator in Spel2.exe to avoid CRT mismatch in Debug mode
    auto& new_id = tile_code_map[tile_code].id;

    new_id = g_current_tile_code_id;
    g_current_tile_code_id++;
    return new_id;
}
