#pragma once

#include <cstddef>     // for size_t
#include <stdexcept>   // for length_error, runtime_error
#include <string_view> // for string_view

#include "tokenize.h" // for Tokenize

template <std::size_t N>
struct GhidraByteString
{
    inline static constexpr std::size_t M = (N + 1) / 3;
    char cpp_byte_string[M]{};

    constexpr GhidraByteString(char const (&str)[N])
    {
        std::size_t i{};
        for (const auto substr : Tokenize<' '>{str})
        {
            if (substr.size() != 2)
            {
                throw std::length_error{"GhidraByteString must be constructed from a sequence of 2-char long strings."};
            }
            else if (substr == "..")
            {
                cpp_byte_string[i] = '*';
            }
            else
            {
                cpp_byte_string[i] = from_string(substr);
            }

            i++;
        }
    };
    constexpr std::size_t size() const
    {
        return M;
    }

  private:
    static constexpr bool is_digit(char c)
    {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }
    static constexpr char tolower(const char c)
    {
        return (c >= 'A' && c <= 'F') ? c + ('a' - 'A') : c;
    }
    static constexpr char to_byte(char c)
    {
        return (c >= '0' && c <= '9')
                   ? c - '0'
                   : c - 'a' + 10;
    }

    static constexpr char from_string(std::string_view str)
    {
        const char first = tolower(str[0]);
        const char second = tolower(str[1]);
        if (!is_digit(first) || !is_digit(second))
        {
            throw std::runtime_error{"Not a digit"};
        }

        const char value = (to_byte(first) << 4) | to_byte(second);
        return value;
    }
};

template <GhidraByteString Str>
constexpr auto operator"" _gh()
{
    return std::string_view{Str.cpp_byte_string, Str.size()};
}

#ifndef _MSC_VER
static_assert("0F 0f af 00 12 22 .. .. 12 .."_gh == "\x0F\x0f\xaf\x00\x12\x22**\x12*"sv);
#endif
