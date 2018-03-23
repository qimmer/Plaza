#ifndef POOL_H
#define POOL_H

#include <Core/Types.h>
#include <Core/Vector.h>
#include <algorithm>

#define MaxPages 4096
#define PoolPageElements 0xFF
#define MemoryGuard(G) char G [16]
#define AssertGuard(G) Assert(memcmp(G, MemoryGuardData, 16) == 0)
#define InitGuard(G) memcpy(G, MemoryGuardData, 16)
static const unsigned char MemoryGuardData[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

template<typename T>
class Pool
{
private:
    struct ALIGN(alignof(T)) Entry {
        Entry() : is_occupied(0) {
            InitGuard(preGuard);
            InitGuard(postGuard);
        }

        MemoryGuard(preGuard);
        char data[sizeof(T)];
        MemoryGuard(postGuard);
        u32 is_occupied;
    };
    struct Alloc {
        void *originalAllocation;
        Entry *alignedAllocation;
    };

    Vector<Alloc> entryPages;
    Vector<u32> free_indices;
public:
    ~Pool() {
        for(auto page : entryPages) free(page.originalAllocation);
    }
    T& operator[] (u32 index);
    const T& operator[] (u32 index) const;
    bool IsValid(u32 index) const;
    u32 End() const;

    u32 Add();
    bool Insert(u32 index);
    bool Remove(u32 index);
};

template<typename T>
T& Pool<T>::operator[](u32 index)
{
    Assert(IsValid(index));

    auto page = index & 0xffffff00;
    index = index & 0xff;
    auto& data = this->entryPages[page].alignedAllocation[index];
    return *(T*)data.data;
}

template<typename T>
const T& Pool<T>::operator[](u32 index) const
{
    Assert(IsValid(index));

    auto page = index & 0xffffff00;
    index = index & 0xff;
    const auto& data = this->entryPages[page].alignedAllocation[index];
    return *(const T*)data.data;
}

template<typename T>
bool Pool<T>::IsValid(u32 index) const
{
    auto page = index & 0xffffff00;
    index = index & 0xff;

    if(this->entryPages.size() <= page) return false;

    const auto& data = this->entryPages[page].alignedAllocation[index];
    AssertGuard(data.preGuard);
    AssertGuard(data.postGuard);
    return data.is_occupied;
}

template<typename T>
u32 Pool<T>::End() const
{
    return this->entryPages.size() * PoolPageElements;
}

template<typename T>
bool Pool<T>::Insert(u32 index)
{
    auto page = index & 0xffffff00;
    index = index & 0xff;

    Assert(page < MaxPages);
    if(page >= this->entryPages.size())
    {
        auto alignment = std::max((size_t)alignof(T), (size_t)16);
        size_t allocSize = PoolPageElements*sizeof(Entry) + alignment;
        void* allocation = malloc(allocSize);
        auto alignedAllocation = (Entry*)std::align(alignment, PoolPageElements*sizeof(Entry), allocation, allocSize);
        for(auto i = 0; i < PoolPageElements; ++i) {
            new (&alignedAllocation[i]) Entry();
        }
        this->entryPages.push_back({allocation, alignedAllocation});
    }

    if(this->entryPages[page].alignedAllocation[index].is_occupied)
    {
        return false;
    }

    this->entryPages[page].alignedAllocation[index].is_occupied = true;
    memset(this->entryPages[page].alignedAllocation[index].data, 0, sizeof(T));
    new (&this->entryPages[page].alignedAllocation[index].data[0]) T();
    for(u32 i = 0; i < this->free_indices.size(); ++i) {
        if(this->free_indices[i] == index) {
            this->free_indices.erase(this->free_indices.begin() + i);
            break;
        }
    }
    AssertGuard(this->entryPages[page].alignedAllocation[index].preGuard);
    AssertGuard(this->entryPages[page].alignedAllocation[index].postGuard);

    return true;
}

template<typename T>
bool Pool<T>::Remove(u32 index)
{
    if(!IsValid(index))
    {
        return false;
    }

    auto page = index & 0xffffff00;
    index = index & 0xff;

    this->entryPages[page].alignedAllocation[index].is_occupied = false;
    ((T*)this->entryPages[page].alignedAllocation[index].data)->~T();
    memset(this->entryPages[page].alignedAllocation[index].data, 0, sizeof(T));
    this->free_indices.push_back(index);
    return true;
}

template<typename T>
u32 Pool<T>::Add() {
    u32 index = this->End();
    if(this->free_indices.size() > 0) {
        index = this->free_indices[this->free_indices.size() - 1];
        this->free_indices.pop_back();
    }

    this->Insert(index);

    return index;
}

#endif