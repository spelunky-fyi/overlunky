#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <span>

struct ByteStr
{
    std::string_view str;
};
template <>
struct fmt::formatter<ByteStr>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const ByteStr& byte_str, FormatContext& ctx) const
    {
        auto out = ctx.out();

        const std::size_t num_bytes = byte_str.str.size();
        if (num_bytes > 0)
        {
            const uint8_t first_byte = byte_str.str[0];
            if (first_byte == '*')
            {
                out = format_to(out, "??");
            }
            else
            {
                out = format_to(out, "{:02x}", first_byte);
            }

            std::span<uint8_t> remainder_byte_span{(uint8_t*)byte_str.str.data() + 1, num_bytes - 1};
            for (uint8_t c : remainder_byte_span)
            {
                if (c == '*')
                {
                    out = format_to(out, " ??");
                }
                else
                {
                    out = format_to(out, " {:02x}", c);
                }
            }
        }

        return out;
    }
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define COMMON_FORMATTER(name, format, ...)                 \
    try                                                     \
    {                                                       \
        fflush(stdout);                                     \
        fmt::print("[" name "] " format "\n", __VA_ARGS__); \
        fflush(stdout);                                     \
    }                                                       \
    catch (fmt::format_error & e)                           \
    {                                                       \
        puts("Formatting exception:" format);               \
        puts(__FILE__ " at " TOSTRING(__LINE__));           \
        puts(e.what());                                     \
    }                                                       \
    catch (...)                                             \
    {                                                       \
    }

#define PANIC(format, ...)                              \
    do                                                  \
    {                                                   \
        COMMON_FORMATTER("panic", format, __VA_ARGS__); \
        std::exit(-1);                                  \
    } while (false)

#define ERR(format, ...)                                \
    do                                                  \
    {                                                   \
        COMMON_FORMATTER("error", format, __VA_ARGS__); \
    } while (false)

#define DEBUG(format, ...)                              \
    do                                                  \
    {                                                   \
        COMMON_FORMATTER("debug", format, __VA_ARGS__); \
    } while (false)

#define INFO(format, ...)                              \
    do                                                 \
    {                                                  \
        COMMON_FORMATTER("info", format, __VA_ARGS__); \
    } while (false)
