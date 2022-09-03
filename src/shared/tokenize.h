#pragma once

#include <cstddef>     // for size_t
#include <string_view> // for string_view

template <char Delimiter>
class Tokenize
{
  public:
    Tokenize() = delete;
    constexpr Tokenize(const Tokenize&) = default;
    constexpr Tokenize(Tokenize&&) = default;
    constexpr Tokenize& operator=(const Tokenize&) = default;
    constexpr Tokenize& operator=(Tokenize&&) = default;

    explicit constexpr Tokenize(std::nullptr_t)
        : m_Source{}
    {
    }
    explicit constexpr Tokenize(const char* source)
        : m_Source{source}
    {
        GetNext();
    }
    explicit constexpr Tokenize(std::string_view source)
        : m_Source{source}
    {
        GetNext();
    }

    constexpr bool operator==(const Tokenize& rhs) const = default;
    constexpr bool operator!=(const Tokenize& rhs) const = default;

    constexpr auto begin()
    {
        return *this;
    }
    constexpr auto end()
    {
        return Tokenize{m_Source, end_tag_t{}};
    }
    constexpr auto begin() const
    {
        return *this;
    }
    constexpr auto end() const
    {
        return Tokenize{m_Source, end_tag_t{}};
    }
    constexpr auto cbegin() const
    {
        return *this;
    }
    constexpr auto cend() const
    {
        return Tokenize{m_Source, end_tag_t{}};
    }

    constexpr auto operator*()
    {
        return m_Source.substr(m_Position, m_Next - m_Position);
    }

    constexpr decltype(auto) operator++()
    {
        if (!Advance())
            *this = end();
        return *this;
    }
    constexpr auto operator++(int)
    {
        auto copy = *this;
        ++(*this);
        return copy;
    }

  private:
    struct end_tag_t
    {
    };
    constexpr Tokenize(std::string_view source, end_tag_t)
        : m_Source{source}, m_Position{source.size()}, m_Next{source.size()}
    {
    }

    constexpr void GetNext()
    {
        ++m_Next;
        while (m_Next < m_Source.size() && m_Source[m_Next] != Delimiter)
        {
            ++m_Next;
        }
    }
    constexpr bool Advance()
    {
        m_Position = m_Next + 1;
        GetNext();
        return m_Next != m_Position;
    }

    std::string_view m_Source;
    size_t m_Position{0};
    size_t m_Next{0};
};
