// Classes with only the functions that were currently used, more can be added if needed

template <class T, class Allocator = std::allocator<T>>
class VectorEditTracked : std::vector<T, Allocator>
{
  public:
    using vector = std::vector<T, Allocator>;
    using vector::begin;
    using vector::end;
    using vector::size;
    uint32_t edit_num = 0;

    VectorEditTracked() = default;
    VectorEditTracked(const VectorEditTracked& other) = default;
    constexpr vector& operator=(const VectorEditTracked& other)
    {
        if (edit_num != other.edit_num)
        {
            vector::operator=(other);
            edit_num = other.edit_num;
        }
        return *this;
    }
    constexpr void push_back(const T& value)
    {
        edit_num++;
        vector::push_back(value);
    }
    constexpr void push_back(T&& value)
    {
        edit_num++;
        vector::push_back(std::move(value));
    }
    constexpr void clear() noexcept
    {
        edit_num++;
        vector::clear();
    }
};

template <
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, T>>>
class UMapEditTracked : std::unordered_map<Key, T, Hash, KeyEqual, Allocator>
{
  public:
    using unordered_map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;
    using unordered_map::begin;
    using unordered_map::end;
    using unordered_map::size;
    uint32_t edit_num = 0;

    UMapEditTracked() = default;
    UMapEditTracked(const UMapEditTracked& other) = default;
    UMapEditTracked& operator=(const UMapEditTracked& other)
    {
        if (edit_num != other.edit_num)
        {
            unordered_map::operator=(other);
            edit_num = other.edit_num;
        }
        return *this;
    }
    T& operator[](const Key& key)
    {
        edit_num++;
        return unordered_map::operator[](key);
    }
    T& operator[](Key&& key)
    {
        edit_num++;
        return unordered_map::operator[](std::move(key));
    }
    void clear() noexcept
    {
        edit_num++;
        return unordered_map::clear();
    }
    using iterator = unordered_map::iterator;
    using const_iterator = unordered_map::const_iterator;
    iterator erase(iterator pos)
    {
        edit_num++;
        return unordered_map::erase(pos);
    }
    iterator erase(const_iterator pos)
    {
        edit_num++;
        return unordered_map::erase(pos);
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        edit_num++;
        return unordered_map::erase(first, last);
    }
    size_t erase(const Key& key)
    {
        edit_num++;
        return unordered_map::erase(key);
    }
};
