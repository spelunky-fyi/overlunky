#include "strings.hpp"
#include "entity.hpp"
#include "fix_entity_descriptions.hpp"

#include "detours.h"
#include <cassert>

static STRINGID wrong_stringid = 0;
std::map<STRINGID, std::u16string> custom_strings;
std::map<uint32_t, STRINGID> string_hashes = {
    {0xd41f49aa, 0},
    {0xcd92c15d, 1}
    // TODO: make script to dump all automatically
};

using OnShopItemNameFormatFun = void(Entity*, char16_t*);
OnShopItemNameFormatFun* g_on_shopnameformat_trampoline{nullptr};
void on_shopitemnameformat(Entity* item, char16_t* buffer)
{
    const STRINGID items_stringid = item->type->description;
    if (items_stringid >= wrong_stringid)
    {
        const STRINGID buy_stringid = 1340; // id of the "Buy %s" text //TODO: replace with hash_to_stringid(0x21683743); when dumping script is done
        constexpr auto buffer_size = 100;   //guess the buffer size, add check if the buffer size is to small?

        swprintf_s((wchar_t*)buffer, buffer_size, (wchar_t*)get_string(buy_stringid), get_string(items_stringid));
        return;
    }

    g_on_shopnameformat_trampoline(item, buffer);
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
    g_on_shopnameformat_trampoline = (OnShopItemNameFormatFun*)addr_insta;
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((void**)&g_on_shopnameformat_trampoline, &on_shopitemnameformat);
    const LONG error = DetourTransactionCommit();
    if (error != NO_ERROR)
    {
        DEBUG("Failed hooking get_shopitem_name: {}\n", error);
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

void change_string(STRINGID string_id, const std::u16string str)
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

STRINGID add_string(const std::u16string str) // future idea: add more strings variants for all languages?
{
    STRINGID new_id = wrong_stringid + (STRINGID)custom_strings.size() + 1;
    custom_strings.insert(std::pair<STRINGID, std::u16string>(new_id, str));
    custom_strings[new_id] = std::move(str);
    return new_id;
}
