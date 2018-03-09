//
// Created by Kim Johannsen on 08-03-2018.
//

#ifndef PLAZA_NULLEDALLOCATOR_H
#define PLAZA_NULLEDALLOCATOR_H


#include <limits>

template <class T>
class NulledAllocator {
public:
    // type definitions
    typedef T        value_type;
    typedef T*       pointer;
    typedef const T* const_pointer;
    typedef T&       reference;
    typedef const T& const_reference;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    static const unsigned long long MagicNumber = 0xFFAAFFAAFFAAFFAA;

    // rebind allocator to type U
    template <class U>
    struct rebind {
        typedef NulledAllocator<U> other;
    };

    // return address of values
    pointer address (reference value) const {
        return &value;
    }
    const_pointer address (const_reference value) const {
        return &value;
    }

    /* constructors and destructor
     * - nothing to do because the allocator has no state
     */
    NulledAllocator() throw() {
    }
    NulledAllocator(const NulledAllocator&) throw() {
    }
    template <class U>
    NulledAllocator (const NulledAllocator<U>&) throw() {
    }
    ~NulledAllocator() throw() {
    }

    // return maximum number of elements that can be allocated
    size_type max_size () const throw() {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    // allocate but don't initialize num elements of type T
    pointer allocate (size_type num, const void* = 0) {
        char *mem = (char*)malloc(num*sizeof(T) + sizeof(unsigned long long) * 3);
        void *block = &mem[sizeof(unsigned long long)*2];
        ((unsigned long long*)mem)[0] = num *sizeof(T);
        ((unsigned long long*)mem)[1] = MagicNumber;
        mem += sizeof(unsigned long long)*2 + num*sizeof(T);
        *((unsigned long long*)mem) = MagicNumber;

        pointer ret = (pointer)(block);
        memset(ret, 0, num*sizeof(T));
        return ret;
    }

    // initialize elements of allocated storage p with value value
    void construct (pointer p, const T& value) {
        // initialize memory with placement new
        new((void*)p)T(value);
    }

    // destroy elements of initialized storage p
    void destroy (pointer p) {
        // destroy objects by calling their destructor
        p->~T();
    }

    // deallocate storage p of deleted elements
    void deallocate (pointer p, size_type num) {
        char *mem = (char*)p - sizeof(unsigned long long) * 2;
        char *block = mem;
        assert(((unsigned long long*)mem)[1] == MagicNumber);
        unsigned long long size = ((unsigned long long*)mem)[0];
        mem += sizeof(unsigned long long) * 2 + size;
        assert(*((unsigned long long*)mem) == MagicNumber);
        free(block);
    }
};

// return that all specializations of this allocator are interchangeable
template <class T1, class T2>
bool operator== (const NulledAllocator<T1>&,
                 const NulledAllocator<T2>&) throw() {
    return true;
}
template <class T1, class T2>
bool operator!= (const NulledAllocator<T1>&,
                 const NulledAllocator<T2>&) throw() {
    return false;
}


#endif //PLAZA_NULLEDALLOCATOR_H
