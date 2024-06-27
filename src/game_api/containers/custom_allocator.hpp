#pragma once

#include <cstddef> // for size_t, ptrdiff_t
#include <new>     // for operator new

[[nodiscard]] void* custom_malloc(std::size_t size);
void custom_free(void* mem);

// This is an allocator that always uses the MemHeap implementations that the game provides
// Thus it crashes in stl-containers that the game creates/destroys but we want to modify anyways
template <typename T>
struct custom_allocator
{
    custom_allocator() = default;

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
        typedef custom_allocator<U> other;
    };
    template <class U>
    custom_allocator(const custom_allocator<U>&)
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
        return static_cast<pointer>(custom_malloc(n * sizeof(T)));
    }

    void deallocate(pointer p, [[maybe_unused]] size_type n)
    {
        custom_free(p);
    }

    void construct(pointer p, const T& val)
    {
        new (static_cast<void*>(p)) T(val);
    }

    template <class U, class... Args>
    void construct(U* const p, Args&&... args)
    {
        new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* const p)
    {
        p->~U();
    }
};
