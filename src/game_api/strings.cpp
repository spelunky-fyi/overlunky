#include "strings.hpp"

#include <Windows.h>     // for GetCurrentThread, LONG, NO_...
#include <cstdio>        // for swprintf_s, NULL, size_t
#include <cstring>       // for memcpy
#include <functional>    // for equal_to
#include <limits>        // for numeric_limits
#include <list>          // for _List_iterator, _List_const...
#include <new>           // for operator new
#include <type_traits>   // for hash, move
#include <unordered_map> // for unordered_map, _Umap_traits...
#include <utility>       // for max, min, pair

#include "containers/game_allocator.hpp" // for game_free, game_malloc
#include "detours.h"                     // for DetourAttach, DetourTransac...
#include "entity.hpp"                    // for get_type, Entity, EntityDB
#include "logger.h"                      // for DEBUG
#include "memory.hpp"                    // for Memory
#include "script/events.hpp"             // for pre_speach_bubble, pre_toast
#include "search.hpp"                    // for get_address
#include "virtual_table.hpp"             // for get_virtual_function_address

static STRINGID g_original_string_ids_end{std::numeric_limits<STRINGID>::max()};
std::unordered_map<STRINGID, std::u16string> g_custom_strings;
std::unordered_map<uint32_t, STRINGID> g_custom_shopitem_names;

using OnShopItemNameFormatFun = void(Entity*, char16_t*);
OnShopItemNameFormatFun* g_on_shopnameformat_trampoline{nullptr};
void on_shopitemnameformat(Entity* item, char16_t* buffer)
{
    STRINGID items_stringid = item->type->description;
    auto it = g_custom_shopitem_names.find(item->uid);
    if (it != g_custom_shopitem_names.end())
    {
        items_stringid = it->second;
    }

    if (items_stringid >= g_original_string_ids_end)
    {
        const STRINGID buy_stringid = hash_to_stringid(0x21683743); // get id of the "Buy %s" text
        constexpr auto buffer_size = 0x800;                         // maybe TODO: add check if the buffer size is to small?

        swprintf_s((wchar_t*)buffer, buffer_size, (wchar_t*)get_string(buy_stringid), get_string(items_stringid));
        return;
    }

    g_on_shopnameformat_trampoline(item, buffer);
}

using OnNPCDialogueFun = void(size_t, Entity*, char16_t*, int, bool);
OnNPCDialogueFun* g_speach_bubble_trampoline{nullptr};
void OnNPCDialogue(size_t func, Entity* NPC, char16_t* buffer, int shoppie_sound_type, bool top)
{
    std::u16string str = pre_speach_bubble(NPC, buffer);
    char16_t* new_string = NULL;
    if (str != u"~[:NO_RETURN:]#")
    {
        if (str.empty())
        {
            return;
        }

        const auto data_size = str.size() * sizeof(char16_t);
        new_string = (char16_t*)game_malloc(data_size + sizeof(char16_t));
        new_string[str.size()] = u'\0';
        memcpy(new_string, str.data(), data_size);

        buffer = new_string;
    }
    g_speach_bubble_trampoline(func, NPC, buffer, shoppie_sound_type, top);
    game_free((void*)new_string);
}

using OnToastFun = void(char16_t*);
OnToastFun* g_toast_trampoline{nullptr};
void OnToast(char16_t* buffer)
{
    std::u16string str = pre_toast(buffer);
    char16_t* new_string = NULL;
    if (str != u"~[:NO_RETURN:]#")
    {
        if (str.empty())
        {
            return;
        }

        const auto data_size = str.size() * sizeof(char16_t);
        new_string = (char16_t*)game_malloc(data_size + sizeof(char16_t));
        new_string[str.size()] = u'\0';
        memcpy(new_string, str.data(), data_size);

        buffer = new_string;
    }
    g_toast_trampoline(buffer);
    game_free((void*)new_string);
}

void strings_init()
{
    g_original_string_ids_end = get_type(1)->description; // get wrong stringid from bordertile
    // fix_entity_descriptions(g_original_string_ids_end); // disabled until someone wants to work on this

    auto addr_format_shopitem = Memory::get().at_exe(get_virtual_function_address(VTABLE_OFFSET::ITEM_PICKUP_ROPEPILE, 7));
    auto addr_npcdialogue = get_address("speech_bubble_fun");
    auto addr_toastfun = get_address("toast");

    g_on_shopnameformat_trampoline = (OnShopItemNameFormatFun*)addr_format_shopitem;
    g_speach_bubble_trampoline = (OnNPCDialogueFun*)addr_npcdialogue;
    g_toast_trampoline = (OnToastFun*)addr_toastfun;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&g_on_shopnameformat_trampoline, &on_shopitemnameformat);
    DetourAttach((void**)&g_speach_bubble_trampoline, &OnNPCDialogue);
    DetourAttach((void**)&g_toast_trampoline, &OnToast);

    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking strings stuff: {}\n", error);
    }
}

const char16_t** get_strings_table()
{
    static const char16_t** string_table = (const char16_t**)get_address("string_table");
    return string_table;
}

STRINGID hash_to_stringid(uint32_t hash)
{
    const auto& string_hashes{get_string_hashes()};
    auto it = string_hashes.find(hash);
    if (it != string_hashes.end())
    {
        return it->second;
    }

    return g_original_string_ids_end;
}

const char16_t* get_string(STRINGID string_id)
{
    if (string_id == g_original_string_ids_end)
    {
        return u"";
    }

    if (string_id > g_original_string_ids_end)
    {
        auto it = g_custom_strings.find(string_id);
        if (it != g_custom_strings.end())
        {
            return it->second.data();
        }

        return u"";
    }
    auto strings_table = get_strings_table();
    return strings_table[string_id];
}

STRINGID pointer_to_stringid(size_t ptr)
{
    auto strings_table = get_strings_table();
    for (STRINGID i = 0; i < g_original_string_ids_end; ++i)
    {
        if ((size_t)strings_table[i] == ptr)
            return i;
    }
    return g_original_string_ids_end;
}

void change_string(STRINGID string_id, std::u16string_view str)
{
    if (string_id == g_original_string_ids_end)
    {
        return;
    }
    else if (string_id > g_original_string_ids_end)
    {
        auto it = g_custom_strings.find(string_id);
        if (it != g_custom_strings.end())
        {
            it->second = str;
        }
    }
    else
    {
        auto strings_table = get_strings_table();

        const char16_t** old_string = strings_table + string_id;

        const auto data_size = str.size() * sizeof(char16_t);
        char16_t* new_string = (char16_t*)game_malloc(data_size + sizeof(char16_t));
        new_string[str.size()] = u'\0';
        memcpy(new_string, str.data(), data_size);

        game_free((void*)*old_string);
        *old_string = new_string;
    }
}

STRINGID add_string(std::u16string str) // future idea: add more strings variants for all languages?
{
    STRINGID new_id = g_original_string_ids_end + (STRINGID)g_custom_strings.size() + 1;
    g_custom_strings[new_id] = std::move(str);
    return new_id;
}

void add_custom_name(uint32_t uid, std::u16string name)
{
    clear_custom_name(uid);
    g_custom_shopitem_names[uid] = add_string(std::move(name));
}

void clear_custom_name(uint32_t uid)
{
    auto it = g_custom_shopitem_names.find(uid);
    if (it != g_custom_shopitem_names.end())
    {
        g_custom_strings.erase(it->second);
        g_custom_shopitem_names.erase(it);
    }
}

void clear_custom_shopitem_names()
{
    for (auto& [uid, string_id] : g_custom_shopitem_names)
    {
        g_custom_strings.erase(string_id);
    }

    g_custom_shopitem_names.clear();
}
