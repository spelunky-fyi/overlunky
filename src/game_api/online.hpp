#pragma once

#include <array>   // for array
#include <cstdint> // for uint32_t, uint8_t, int8_t, int32_t
#include <string>  // for string

enum class GAME_MODE : uint32_t
{
    COOP = 1,
    ARENA = 2
};

enum class READY_STATE : uint8_t
{
    NOT_READY = 0,
    READY = 1,
    SEARCHING = 2
};

enum class PLATFORM : uint8_t
{
    NONE = 16,
    DISCORD = 17,
    STEAM = 18,
    XBOX = 19,
    SWITCH = 32,
    PLAYSTATION = 48
};

struct OnlinePlayer
{
    uint8_t player_name_length;
    uint8_t padding[3];
    uint32_t unknown1;
    uint64_t id;
    GAME_MODE game_mode;
    uint8_t unknown2;
    READY_STATE ready_state : 2;
    PLATFORM platform : 6;
    uint8_t character;
    char player_name[33]; // could be 32 actually?
};

struct OnlineLobby
{
    uint32_t code;
    uint32_t unknown1;
    uint32_t keys_pressed;
    uint32_t keys_pressed_sync;
    uint8_t local_player_slot;
    uint8_t unknown3;
    int8_t unknown4;
    int8_t unknown5;
    uint32_t unknown6;
    int8_t unknown7;
    int8_t unknown8;
    uint8_t unknown9;
    uint8_t unknown10;
    int32_t unknown11;
    int32_t unknown12;
    int8_t unknown13;
    int8_t unknown14;
    uint8_t unknown15;
    uint8_t unknown16;
    uint32_t unknown17;
    uint8_t unknown18;
    uint8_t unknown19;
    int8_t unknown20;
    int8_t unknown21;
    int8_t unknown22;
    uint8_t unknown23;
    uint8_t unknown24;
    uint8_t unknown25;
    uint32_t unknown26;

    /// Gets the string equivalent of the code
    std::string get_code();
};

class Online
{
  public:
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint32_t unknown5;
    uint32_t unknown6;
    uint32_t unknown7;
    uint32_t unknown8;
    uint32_t unknown9;
    uint32_t unknown10;
    uint32_t unknown11;
    uint32_t unknown12;
    uint32_t unknown13;
    uint32_t unknown14;
    uint32_t unknown15;
    uint32_t unknown16;
    uint32_t unknown17;
    uint32_t unknown18;
    uint32_t unknown19;
    uint32_t unknown20;
    uint32_t unknown21;
    uint32_t unknown22;
    uint32_t unknown23;
    uint32_t unknown24;
    uint32_t unknown25;
    uint32_t unknown26;
    uint32_t unknown27;
    uint32_t unknown28;
    uint32_t unknown29;
    uint32_t unknown30;
    uint32_t unknown31;
    uint32_t unknown32;
    uint32_t unknown33;
    uint32_t unknown34;
    uint32_t unknown35;
    uint32_t unknown36;
    uint32_t unknown37;
    uint32_t unknown38;
    std::array<OnlinePlayer, 4> online_players;
    OnlinePlayer local_player;
    OnlineLobby lobby;
    OnlineLobby lobby_dupe;
    // some more stuff

    virtual ~Online() = 0;
};

Online* get_online();
