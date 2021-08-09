#pragma once

#include <cstddef>
#include <cstdint>

void* game_malloc(std::size_t size);
void game_free(void* mem);

// This is an allocator that always uses the malloc/free implementations that the game provides
// Thus it avoids CRT-mismatch while debugging and should be used in stl-containers
// that the game creates/destroyes but we want to modify anyways
template <typename T>
struct game_allocator
{
    game_allocator() = default;

    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    template <class U>
    struct rebind
    {
        typedef game_allocator<U> other;
    };
    template <class U>
    game_allocator(const game_allocator<U>&)
    {
    }

    pointer address(reference x) const
    {
        return &x;
    }
    const_pointer address(const_reference x) const
    {
        return &x;
    }
    size_type max_size() const throw()
    {
        return size_type(-1) / sizeof(value_type);
    }

    pointer allocate(size_type n, [[maybe_unused]] void* hint = nullptr)
    {
        return static_cast<pointer>(game_malloc(n * sizeof(T)));
    }

    void deallocate(pointer p, [[maybe_unused]] size_type n)
    {
        game_free(p);
    }

    void construct(pointer p, const T& val)
    {
        new (static_cast<void*>(p)) T(val);
    }

    void construct(pointer p)
    {
        new (static_cast<void*>(p)) T();
    }

    void destroy(pointer p)
    {
        p->~T();
    }
};
