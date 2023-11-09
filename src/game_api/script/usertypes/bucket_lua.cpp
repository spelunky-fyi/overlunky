#include "bucket_lua.hpp"

#include "bucket.hpp"

namespace NBucket
{
void register_usertypes(sol::state& lua)
{
    /// Used to get and set Overlunky settings in Bucket
    auto ol_type = lua.new_usertype<Overlunky>("Overlunky", sol::no_constructor);
    ol_type["options"] = &Overlunky::options;
    ol_type["set_options"] = &Overlunky::set_options;
    ol_type["keys"] = &Overlunky::keys;
    ol_type["ignore_keys"] = &Overlunky::ignore_keys;
    ol_type["ignore_keycodes"] = &Overlunky::ignore_keycodes;
    ol_type["ignore_features"] = &Overlunky::ignore_features;
    ol_type["selected_uid"] = &Overlunky::selected_uid;
    ol_type["set_selected_uid"] = &Overlunky::set_selected_uid;
    ol_type["selected_uids"] = &Overlunky::selected_uids;
    ol_type["hovered_uid"] = &Overlunky::hovered_uid;
    ol_type["set_selected_uid"] = &Overlunky::set_selected_uid;
    ol_type["set_selected_uids"] = &Overlunky::set_selected_uids;

    /// Shared memory structure used for Playlunky-Overlunky interoperability
    auto bucket_type = lua.new_usertype<Bucket>("Bucket", sol::no_constructor);
    bucket_type["data"] = &Bucket::data;
    bucket_type["overlunky"] = sol::readonly(&Bucket::overlunky);

    /// Returns the Bucket of data stored in shared memory between Overlunky and Playlunky
    // lua["get_bucket"] = []() -> Bucket*
    lua["get_bucket"] = Bucket::get;

    /// Keycodes to be used in various input functions. Most are standard, but the OL_ prefixed keys are specific to Overlunky::ignore_keycodes.
    lua.create_named_table("KEY"
                           //, "A", 65
                           //, "", ...check__[lua_enums.txt]\[game_data/lua_enums.txt\]...
    );

    lua["KEY"]["OL_MOD_CTRL"] = 0x100;
    lua["KEY"]["OL_MOD_SHIFT"] = 0x200;
    lua["KEY"]["OL_MOD_ALT"] = 0x800;
    lua["KEY"]["OL_MOUSE_1"] = 0x401;
    lua["KEY"]["OL_MOUSE_2"] = 0x402;
    lua["KEY"]["OL_MOUSE_3"] = 0x403;
    lua["KEY"]["OL_MOUSE_4"] = 0x404;
    lua["KEY"]["OL_MOUSE_5"] = 0x405;
    lua["KEY"]["OL_MOUSE_WHEEL_DOWN"] = 0x411;
    lua["KEY"]["OL_MOUSE_WHEEL_UP"] = 0x412;

    lua["KEY"]["BACKSPACE"] = 0x08;
    lua["KEY"]["TAB"] = 0x09;
    lua["KEY"]["CLEAR"] = 0x0C;
    lua["KEY"]["RETURN"] = 0x0D;
    lua["KEY"]["SHIFT"] = 0x10;
    lua["KEY"]["CTRL"] = 0x11;
    lua["KEY"]["ALT"] = 0x12;
    lua["KEY"]["PAUSE"] = 0x13;
    lua["KEY"]["CAPS"] = 0x14;
    lua["KEY"]["ESCAPE"] = 0x1B;
    lua["KEY"]["SPACE"] = 0x20;
    lua["KEY"]["PGUP"] = 0x21;
    lua["KEY"]["PGDN"] = 0x22;
    lua["KEY"]["END"] = 0x23;
    lua["KEY"]["HOME"] = 0x24;
    lua["KEY"]["LEFT"] = 0x25;
    lua["KEY"]["UP"] = 0x26;
    lua["KEY"]["RIGHT"] = 0x27;
    lua["KEY"]["DOWN"] = 0x28;
    lua["KEY"]["SELECT"] = 0x29;
    lua["KEY"]["PRINT"] = 0x2A;
    lua["KEY"]["EXECUTE"] = 0x2B;
    lua["KEY"]["SNAPSHOT"] = 0x2C;
    lua["KEY"]["INSERT"] = 0x2D;
    lua["KEY"]["DELETE"] = 0x2E;
    lua["KEY"]["NUMPAD0"] = 0x60;
    lua["KEY"]["NUMPAD1"] = 0x61;
    lua["KEY"]["NUMPAD2"] = 0x62;
    lua["KEY"]["NUMPAD3"] = 0x63;
    lua["KEY"]["NUMPAD4"] = 0x64;
    lua["KEY"]["NUMPAD5"] = 0x65;
    lua["KEY"]["NUMPAD6"] = 0x66;
    lua["KEY"]["NUMPAD7"] = 0x67;
    lua["KEY"]["NUMPAD8"] = 0x68;
    lua["KEY"]["NUMPAD9"] = 0x69;
    lua["KEY"]["MULTIPLY"] = 0x6A;
    lua["KEY"]["ADD"] = 0x6B;
    lua["KEY"]["SEPARATOR"] = 0x6C;
    lua["KEY"]["SUBTRACT"] = 0x6D;
    lua["KEY"]["DECIMAL"] = 0x6E;
    lua["KEY"]["DIVIDE"] = 0x6F;
    lua["KEY"]["F1"] = 0x70;
    lua["KEY"]["F2"] = 0x71;
    lua["KEY"]["F3"] = 0x72;
    lua["KEY"]["F4"] = 0x73;
    lua["KEY"]["F5"] = 0x74;
    lua["KEY"]["F6"] = 0x75;
    lua["KEY"]["F7"] = 0x76;
    lua["KEY"]["F8"] = 0x77;
    lua["KEY"]["F9"] = 0x78;
    lua["KEY"]["F10"] = 0x79;
    lua["KEY"]["F11"] = 0x7A;
    lua["KEY"]["F12"] = 0x7B;
    lua["KEY"]["F13"] = 0x7C;
    lua["KEY"]["F14"] = 0x7D;
    lua["KEY"]["F15"] = 0x7E;
    lua["KEY"]["F16"] = 0x7F;
    lua["KEY"]["F17"] = 0x80;
    lua["KEY"]["F18"] = 0x81;
    lua["KEY"]["F19"] = 0x82;
    lua["KEY"]["F20"] = 0x83;
    lua["KEY"]["F21"] = 0x84;
    lua["KEY"]["F22"] = 0x85;
    lua["KEY"]["F23"] = 0x86;
    lua["KEY"]["F24"] = 0x87;
    lua["KEY"]["LSHIFT"] = 0xA0;
    lua["KEY"]["RSHIFT"] = 0xA1;
    lua["KEY"]["LCONTROL"] = 0xA2;
    lua["KEY"]["RCONTROL"] = 0xA3;
    lua["KEY"]["LALT"] = 0xA4;
    lua["KEY"]["RALT"] = 0xA5;
    lua["KEY"]["PLUS"] = 0xBB;
    lua["KEY"]["COMMA"] = 0xBC;
    lua["KEY"]["MINUS"] = 0xBD;
    lua["KEY"]["PERIOD"] = 0xBE;
    lua["KEY"]["OEM_1"] = 0xBA;
    lua["KEY"]["OEM_2"] = 0xBF;
    lua["KEY"]["OEM_3"] = 0xC0;
    lua["KEY"]["OEM_4"] = 0xDB;
    lua["KEY"]["OEM_5"] = 0xDC;
    lua["KEY"]["OEM_6"] = 0xDD;
    lua["KEY"]["OEM_7"] = 0xDE;
    lua["KEY"]["OEM_8"] = 0xDF;
    lua["KEY"]["OEM_102"] = 0xE2;
    for (char c = '0'; c < '9'; c++)
        lua["KEY"][std::string{c}] = (int)c;
    for (char c = 'A'; c < 'Z'; c++)
        lua["KEY"][std::string{c}] = (int)c;
}
}; // namespace NBucket
