#ifndef INCLUDE_ALLOC_H
#define INCLUDE_ALLOC_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdint>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <set>
#include <list>
#include <queue>
#include <map>
#include <vector>


uint64_t memory_usage = 0;

// use this when calling STL object if you want
// to keep track of memory usage
template <class T> class MemoryCountingAllocator {
public:
    // type definitions
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;


    // rebind allocator to type U
    template <class U> struct rebind {
        typedef MemoryCountingAllocator<U> other;
    };

    pointer address(reference value) const {
        return &value;
    }
    const_pointer address(const_reference value) const {
        return &value;
    }

    MemoryCountingAllocator() : base() {}
    MemoryCountingAllocator(const MemoryCountingAllocator &) : base() {}
    template <typename U>
    MemoryCountingAllocator(const MemoryCountingAllocator<U> &) : base() {}
    ~MemoryCountingAllocator() {}

    // return maximum number of elements that can be allocated
    size_type max_size() const throw() {
        return base.max_size();
    }

    pointer allocate(size_type num, const void * p = 0) {
        memory_usage += num * sizeof(T);
        return base.allocate(num,p);
    }

    void construct(pointer p, const T &value) {
        return base.construct(p,value);
    }

    // destroy elements of initialized storage p
    void destroy(pointer p) {
        base.destroy(p);
    }

    // deallocate storage p of deleted elements
    void deallocate(pointer p, size_type num ) {
        memory_usage -= num * sizeof(T);
        base.deallocate(p,num);
    }
    std::allocator<T> base;
};

// for our purposes, we don't want to distinguish between allocators.
template <class T1, class T2>
bool operator==(const MemoryCountingAllocator<T1> &, const T2 &) throw() {
    return true;
}

template <class T1, class T2>
bool operator!=(const MemoryCountingAllocator<T1> &, const T2 &) throw() {
    return false;
}

#endif
