#pragma once

#include <WinSock2.h>
#include <array>
#include <cstdint>

struct OnlinePlayer
{
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint8_t unknown5;
    uint8_t ready_state;
    uint8_t character;
    char player_name[37];
};

struct OnlinePlayerShort
{
    uint32_t unknown1;
    uint32_t unknown2;
    uint32_t unknown3;
    uint32_t unknown4;
    uint8_t unknown5;
    uint8_t ready_state;
    uint8_t character;
    char player_name[33]; // this smells fishy... why fewer chars available for local name, but leave at 37 for the player structs?
};

struct OnlineLobby
{
    uint32_t code;
    uint32_t unknown1;
    uint32_t keys_pressed;
    uint32_t keys_pressed_sync;
    uint8_t unknown2;
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

// These are sockaddr_in, but that struct has the extra 8 bytes at
// the end, which is not avaiable in the Online struct (so just cast it)
struct SockAddrIn
{
    uint16_t family;
    uint16_t port;
    uint32_t address;
};

struct Server
{
    SockAddrIn sock_addr;
    int64_t unknown_query;
    int64_t unknown_response;
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
    uint32_t socket;
    uint32_t unknown16;
    uint32_t unknown17;
    uint32_t unknown18;
    uint32_t unknown19;
    uint32_t unknown20;
    uint32_t unknown21;
    uint32_t unknown22;
    SockAddrIn sock_addr_unknown1;
    SockAddrIn sock_addr_unknown2; // to edge server
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
    uint32_t unknown39;
    std::array<OnlinePlayer, 4> online_players;
    OnlinePlayerShort local_player;
    OnlineLobby lobby;
    OnlineLobby lobby_dupe;
    std::array<Server, 32> servers;
    uint32_t unknown40;
    uint32_t unknown41;
    size_t unknown42;
    uint32_t unknown43;
    uint32_t unknown44;
    uint32_t unknown45;
    uint32_t unknown46;
    SockAddrIn sock_addr_game;
    size_t unknown47;
    size_t unknown48;
    size_t unknown49;
    size_t unknown50;
    size_t unknown51;
    size_t unknown52;
    size_t unknown53;
    size_t unknown54;
    size_t unknown55;

    virtual ~Online() = 0;
};

Online* get_online();
