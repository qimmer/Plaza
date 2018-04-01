//
// Created by Kim Johannsen on 24-03-2018.
//

#ifndef PLAZA_ALIGNEDALLOCATOR_H
#define PLAZA_ALIGNEDALLOCATOR_H

#include <stdlib.h>
#include <malloc.h>

template <typename T, std::size_t N = 16>
class AlignedAllocator {
public:
    typedef T value_type;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    typedef T * pointer;
    typedef const T * const_pointer;

    typedef T & reference;
    typedef const T & const_reference;

public:
    inline AlignedAllocator () throw () { }

    template <typename T2>
    inline AlignedAllocator (const AlignedAllocator<T2, N> &) throw () { }

    inline ~AlignedAllocator () throw () { }

    inline pointer address (reference r) {
        return &r;
    }

    inline const_pointer address (const_reference r) const {
        return &r;
    }

    inline pointer allocate (size_type n) {
        return (pointer)_aligned_malloc(std::max(n*sizeof(value_type), N), N);
    }

    inline void deallocate (pointer p, size_type) {
        _aligned_free (p);
    }

    inline void construct (pointer p, const value_type & wert) {
        new (p) value_type (wert);
    }

    inline void destroy (pointer p) {
        p->~value_type ();
    }

    inline size_type max_size () const throw () {
        return size_type (-1) / sizeof (value_type);
    }

    template <typename T2>
    struct rebind {
        typedef AlignedAllocator<T2, N> other;
    };

    bool operator!=(const AlignedAllocator<T,N>& other) const  {
        return !(*this == other);
    }

    // Returns true if and only if storage allocated from *this
    // can be deallocated from other, and vice versa.
    // Always returns true for stateless allocators.
    bool operator==(const AlignedAllocator<T,N>& other) const {
        return true;
    }
};

#endif //PLAZA_ALIGNEDALLOCATOR_H
