// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef BASE_STACK_CONTAINER_H_
#define BASE_STACK_CONTAINER_H_
#include <string>
#include <vector>
#include <cassert>
#include <memory.h>

// This allocator can be used with STL containers to provide a stack buffer
// from which to allocate memory and overflows onto the heap. This stack buffer
// would be allocated on the stack and allows us to avoid heap operations in
// some situations.
//
// STL likes to make copies of allocators, so the allocator itself can't hold
// the data. Instead, we make the creator responsible for creating a
// StackAllocator::Source which contains the data. Copying the allocator
// merely copies the pointer to this shared source, so all allocators created
// based on our allocator will share the same stack buffer.
//
// This stack buffer implementation is very simple. The first allocation that
// fits in the stack buffer will use the stack buffer. Any subsequent
// allocations will not use the stack buffer, even if there is unused room.
// This makes it appropriate for array-like containers, but the caller should
// be sure to reserve() in the container up to the stack buffer size. Otherwise
// the container will allocate a small array which will "use up" the stack
// buffer.
template<typename T, size_t stack_capacity>
class StackAllocator : public std::allocator<T> {
public:
    typedef typename std::allocator<T>::pointer pointer;
    typedef typename std::allocator<T>::size_type size_type;
    static const unsigned long long magic = 0xFFAAFFAAFFAAFFAA;

    // For the straight up copy c-tor, we can share storage.
    StackAllocator(const StackAllocator<T, stack_capacity>& rhs)
            : std::allocator<T>(), isAllocated(false), magicNumberStart(magic), magicNumberEnd(magic) {

    }
    // ISO C++ requires the following constructor to be defined,
    // and std::vector in VC++2008SP1 Release fails with an error
    // in the class _Container_base_aux_alloc_real (from <xutility>)
    // if the constructor does not exist.
    // For this constructor, we cannot share storage; there's
    // no guarantee that the Source buffer of Ts is large enough
    // for Us.
    // TODO: If we were fancy pants, perhaps we could share storage
    // iff sizeof(T) == sizeof(U).
    template<typename U, size_t other_capacity>
    StackAllocator(const StackAllocator<U, other_capacity>& other) {
        magicNumberStart = magicNumberEnd = magic;
    }
    explicit StackAllocator() :
		isAllocated(false), magicNumberStart(magic), magicNumberEnd(magic){
    }
    // Actually do the allocation. Use the stack buffer if nobody has used it yet
    // and the size requested fits. Otherwise, fall through to the standard
    // allocator.
    pointer allocate(size_type n, void* hint = 0) {
		assert(magicNumberStart == magicNumberEnd == magic);
        assert(n <= stack_capacity);
        assert(magicNumberStart == magic && magicNumberEnd == magic && !isAllocated);
        memset(stack_buffer_, 0xFE, n);
        isAllocated = true;
        return (pointer)stack_buffer_;
    }
    // Free: when trying to free the stack buffer, just mark it as free. For
    // non-stack-buffer pointers, just fall though to the standard allocator.
    void deallocate(pointer p, size_type n) {
        assert(magicNumberStart == magic && magicNumberEnd == magic && isAllocated);
        isAllocated = false;
		assert(magicNumberStart == magicNumberEnd == magic);
    }
private:
    unsigned long long magicNumberStart;
    T stack_buffer_[stack_capacity];
    unsigned long long magicNumberEnd;

    bool isAllocated;
};

#endif  // BASE_STACK_CONTAINER_H_