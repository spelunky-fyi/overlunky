#include "strings.hpp"

#include <Windows.h>     // for GetCurrentThread, LONG, NO_...
#include <codecvt>       // for codecvt_utf16
#include <cstdio>        // for swprintf_s, NULL, size_t
#include <cstring>       // for memcpy
#include <functional>    // for equal_to
#include <limits>        // for numeric_limits
#include <list>          // for _List_iterator, _List_const...
#include <new>           // for operator new
#include <type_traits>   // for hash, move
#include <unordered_map> // for unordered_map, _Umap_traits...
#include <utility>       // for max, min, pair

#include "bucket.hpp"                    // for Bucket
#include "constants.hpp"                 //
#include "containers/game_allocator.hpp" // for game_free, game_malloc
#include "detours.h"                     // for DetourAttach, DetourTransac...
#include "entity.hpp"                    // for get_type, Entity, EntityDB
#include "logger.h"                      // for DEBUG
#include "memory.hpp"                    // for Memory
#include "script/events.hpp"             // for pre_speach_bubble, pre_toast
#include "search.hpp"                    // for get_address
#include "virtual_table.hpp"             // for get_virtual_function_address

static STRINGID g_original_string_ids_end{std::numeric_limits<STRINGID>::max()};

using OnShopItemNameFormatFun = void(Entity*, char16_t*);
OnShopItemNameFormatFun* g_on_shopnameformat_trampoline{nullptr};
void on_shopitemnameformat(Entity* item, char16_t* buffer)
{
    STRINGID items_stringid = item->type->description;
    auto bucket = Bucket::get();
    auto it = bucket->custom_shopitem_names.find(item->uid);
    if (it != bucket->custom_shopitem_names.end())
        items_stringid = it->second;

    if (items_stringid >= g_original_string_ids_end)
    {
        const STRINGID buy_stringid = hash_to_stringid(0x21683743); // get id of the "Buy %s" text
        constexpr auto buffer_size = 0x800;                         // TODO: maybe add check if the buffer size is to small?

        swprintf_s((wchar_t*)buffer, buffer_size, (wchar_t*)get_string(buy_stringid), get_string(items_stringid));
        return;
    }

    g_on_shopnameformat_trampoline(item, buffer);
}

using OnNPCDialogueFun = void(size_t, Entity*, char16_t*, int, bool);
OnNPCDialogueFun* g_speach_bubble_trampoline{nullptr};
void OnNPCDialogue(size_t hud, Entity* NPC, char16_t* buffer, int shoppie_sound_type, bool top)
{
    std::u16string str = pre_speach_bubble(NPC, buffer);
    char16_t* new_string = nullptr;
    if (str != no_return_str)
    {
        if (str.empty())
            return;

        const auto data_size = str.size() * sizeof(char16_t);
        new_string = (char16_t*)game_malloc(data_size + sizeof(char16_t));
        new_string[str.size()] = u'\0';
        memcpy(new_string, str.data(), data_size);

        buffer = new_string;
    }
    g_speach_bubble_trampoline(hud, NPC, buffer, shoppie_sound_type, top);
    game_free((void*)new_string);
}

using OnToastFun = void(char16_t*);
OnToastFun* g_toast_trampoline{nullptr};
void OnToast(char16_t* buffer)
{
    std::u16string str = pre_toast(buffer);
    char16_t* new_string = nullptr;
    if (str != no_return_str)
    {
        if (str.empty())
            return;

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
    auto bucket = Bucket::get();
    if (bucket->next_stringid == 0)
        bucket->next_stringid = g_original_string_ids_end + 1;

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
    for (auto& it : string_hashes)
    {
        if (it == hash)
            return (STRINGID)(&it - &string_hashes[0]);
    }
    return g_original_string_ids_end;
}

const char16_t* get_string(STRINGID string_id)
{
    if (string_id == g_original_string_ids_end)
        return u"";

    if (string_id > g_original_string_ids_end)
    {
        auto bucket = Bucket::get();
        auto it = bucket->custom_strings.find(string_id);
        if (it != bucket->custom_strings.end())
            return it->second.data();

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
        auto bucket = Bucket::get();
        auto it = bucket->custom_strings.find(string_id);
        if (it != bucket->custom_strings.end())
            it->second = str;
    }
    else
    {
        const char16_t** old_string = get_strings_table() + string_id;
        if (std::char_traits<char16_t>::length(*old_string) < str.length())
        {
            const auto data_size = str.size() * sizeof(char16_t);
            char16_t* new_string = (char16_t*)game_malloc(data_size + sizeof(char16_t));
            new_string[str.size()] = NULL;
            std::memcpy(new_string, str.data(), data_size);

            game_free((void*)*old_string);
            *old_string = new_string;
        }
        else
        {
            char16_t* nc_old_string = const_cast<char16_t*>(*old_string);
            std::memcpy(nc_old_string, str.data(), str.length() * sizeof(char16_t));
            *(nc_old_string + str.length()) = NULL;
        }
    }
}

STRINGID add_string(std::u16string str) // future idea: add more strings variants for all languages?
{
    auto bucket = Bucket::get();
    STRINGID new_id = bucket->next_stringid++;
    if (bucket->next_stringid == 0) // lazy overflow protection
        bucket->next_stringid = g_original_string_ids_end + 1;

    bucket->custom_strings[new_id] = std::move(str);
    return new_id;
}

void add_custom_name(uint32_t uid, std::u16string name)
{
    auto bucket = Bucket::get();
    clear_custom_name(uid);
    bucket->custom_shopitem_names[uid] = add_string(std::move(name));
}

void clear_custom_name(uint32_t uid)
{
    auto bucket = Bucket::get();
    auto it = bucket->custom_shopitem_names.find(uid);
    if (it != bucket->custom_shopitem_names.end())
    {
        bucket->custom_strings.erase(it->second);
        bucket->custom_shopitem_names.erase(it);
    }
}

void clear_custom_shopitem_names()
{
    auto bucket = Bucket::get();
    for (auto& [uid, string_id] : bucket->custom_shopitem_names)
        bucket->custom_strings.erase(string_id);

    bucket->custom_shopitem_names.clear();
}

std::u16string get_entity_name(ENT_TYPE id, bool fallback_strategy)
{
    using get_entity_name_func = void(ENT_TYPE, char16_t*, size_t, bool);
    static get_entity_name_func* get_entity_name_impl = (get_entity_name_func*)get_address("get_entity_name");

    std::array<char16_t, 256> out_buffer{};
    get_entity_name_impl(id, out_buffer.data(), out_buffer.size(), true);
    std::u16string return_string{out_buffer.data()};
    if (fallback_strategy && return_string.empty())
    {
        std::string enum_name{to_name(id).substr(sizeof("ENT_TYPE_") - 1)};
        std::replace(enum_name.begin(), enum_name.end(), '_', ' ');
        for (size_t i = 1; i < enum_name.size(); i++)
        {
            if (enum_name[i - 1] != ' ' && enum_name[i] != ' ')
                enum_name[i] = (char)std::tolower((int)enum_name[i]);
        }

        using cvt_type = std::codecvt_utf8_utf16<char16_t>;
        std::wstring_convert<cvt_type, char16_t> cvt;
        return_string = cvt.from_bytes(enum_name);
    }
    return return_string;
}
