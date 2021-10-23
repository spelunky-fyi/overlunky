#include "strings.hpp"
#include "entity.hpp"
#include "fix_entity_descriptions.hpp"
#include "script/events.hpp"

#include "string_hashes.cpp"

#include "detours.h"
#include <cassert>

static STRINGID wrong_stringid = 0;
std::unordered_map<STRINGID, std::u16string> custom_strings;

using OnShopItemNameFormatFun = void(Entity*, char16_t*);
OnShopItemNameFormatFun* g_on_shopnameformat_trampoline{nullptr};
void on_shopitemnameformat(Entity* item, char16_t* buffer)
{
    const STRINGID items_stringid = item->type->description;
    if (items_stringid >= wrong_stringid)
    {
        const STRINGID buy_stringid = hash_to_stringid(0x21683743); // get id of the "Buy %s" text
        constexpr auto buffer_size = 0x800;                         // maybe TODO: add check if the buffer size is to small?

        swprintf_s((wchar_t*)buffer, buffer_size, (wchar_t*)get_string(buy_stringid), get_string(items_stringid));
        return;
    }

    g_on_shopnameformat_trampoline(item, buffer);
}

using OnNPCDialogueFun = void(size_t, Entity*, char16_t*, int);
OnNPCDialogueFun* g_speach_bubble_trampoline{nullptr};
void OnNPCDialogue(size_t func, Entity* NPC, char16_t* buffer, int shoppie_sound_type)
{
    std::u16string str = pre_speach_bubble(NPC, buffer);
    if (str != u"~[:NO_RETURN:]#")
    {
        if (str.empty())
            return;

        const auto data_size = str.size() * sizeof(char16_t);
        char16_t* new_string = (char16_t*)game_malloc(data_size + sizeof(char16_t));
        new_string[str.size()] = u'\0';
        memcpy(new_string, str.data(), data_size);

        buffer = new_string;
    }
    g_speach_bubble_trampoline(func, NPC, buffer, shoppie_sound_type);
}

void strings_init()
{
    if (wrong_stringid == 0)
    {
        //get wrong stringid from bordertile
        wrong_stringid = get_type(1)->description;
    }
    fix_entity_descriptions();

    auto addr_insta = get_address("format_shopitem_name");
    auto addr_npcdialogue = get_address("npc_dialogue_fun");
    g_on_shopnameformat_trampoline = (OnShopItemNameFormatFun*)addr_insta;
    g_speach_bubble_trampoline = (OnNPCDialogueFun*)addr_npcdialogue;
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&g_on_shopnameformat_trampoline, &on_shopitemnameformat);
    DetourAttach((void**)&g_speach_bubble_trampoline, &OnNPCDialogue);

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
    auto it = string_hashes.find(hash);
    if (it != string_hashes.end())
        return it->second;

    return wrong_stringid;
}

const char16_t* get_string(STRINGID string_id)
{
    if (string_id == wrong_stringid)
        return u"";

    if (string_id > wrong_stringid)
    {
        auto it = custom_strings.find(string_id);
        if (it != custom_strings.end())
        {
            return it->second.data();
        }

        return u"";
    }
    auto strings_table = get_strings_table();
    return strings_table[string_id];
}

void change_string(STRINGID string_id, std::u16string str)
{
    if (string_id == wrong_stringid)
        return;
    else if (string_id > wrong_stringid)
    {
        auto it = custom_strings.find(string_id);
        if (it != custom_strings.end())
        {
            it->second = std::move(str);
            return;
        }
    }
    else
    {
        //stolen from character name change
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
    STRINGID new_id = wrong_stringid + (STRINGID)custom_strings.size() + 1;
    custom_strings[new_id] = std::move(str);
    return new_id;
}
