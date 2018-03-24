#ifndef POOL_H
#define POOL_H

#include <Core/Types.h>
#include <Core/Vector.h>
#include <algorithm>
#include <memory>

#define MaxPages 4096
#define PoolPageElements 256

#ifdef POOL_MEM_GUARD
#define MemoryGuard(G) char G [16]
#define AssertGuard(G) Assert(memcmp(G, MemoryGuardData, 16) == 0)
#define InitGuard(G) memcpy(G, MemoryGuardData, 16)
static const char *MemoryGuardData = "DeadBeefDeadBif";
#endif

template<typename T>
class ALIGN(alignof(T)) Pool
{
private:
    T default_value;

    struct ALIGN(alignof(T)) Entry {
        Entry() : is_occupied(0) {
#ifdef POOL_MEM_GUARD
            InitGuard(preGuard);
            InitGuard(postGuard);
#endif
        }
#ifdef POOL_MEM_GUARD
        MemoryGuard(preGuard);
#endif
        T data;
#ifdef POOL_MEM_GUARD
        MemoryGuard(postGuard);
#endif
        u32 is_occupied;
    };

    Vector<Vector<Entry>> entryPages;
    Vector<u32> free_indices;
public:
    Pool() {
        this->default_value.~T();
        memset(&this->default_value, 0, sizeof(T));
        new (&this->default_value) T();
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
    auto page = (index & 0xffffff00) >> 8;
    index = index & 0xff;

    auto& entry = this->entryPages[page][index];
    Assert(entry.is_occupied);
#ifdef POOL_MEM_GUARD
    AssertGuard(entry.preGuard);
    AssertGuard(entry.postGuard);
#endif
    return entry.data;
}

template<typename T>
const T& Pool<T>::operator[](u32 index) const
{
    auto page = (index & 0xffffff00) >> 8;
    index = index & 0xff;

    const auto& entry = this->entryPages[page][index];
    Assert(entry.is_occupied);
#ifdef POOL_MEM_GUARD
    AssertGuard(entry.preGuard);
    AssertGuard(entry.postGuard);
#endif
    return entry.data;
}

template<typename T>
bool Pool<T>::IsValid(u32 index) const
{
    auto page = (index & 0xffffff00) >> 8;
    index = index & 0xff;

    if(this->entryPages.size() <= page) return false;

    const auto& data = this->entryPages[page][index];
#ifdef POOL_MEM_GUARD
    AssertGuard(data.preGuard);
    AssertGuard(data.postGuard);
#endif
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
    auto page = (index & 0xffffff00) >> 8;
    index = index & 0xff;

    if(page >= this->entryPages.size())
    {
        for(auto i = this->entryPages.size(); i <= page; ++i) {
            this->entryPages.push_back(Vector<Entry>(PoolPageElements));
        }
    }

    auto& entry = this->entryPages[page][index];
    if(entry.is_occupied)
    {
        return false;
    }

    entry.is_occupied = true;
    entry.data = this->default_value;

    for(u32 i = 0; i < this->free_indices.size(); ++i) {
        if(this->free_indices[i] == index) {
            this->free_indices.erase(this->free_indices.begin() + i);
            break;
        }
    }

#ifdef POOL_MEM_GUARD
    AssertGuard(entry.preGuard);
    AssertGuard(entry.postGuard);
#endif

    return true;
}

template<typename T>
bool Pool<T>::Remove(u32 index)
{
    auto page = (index & 0xffffff00) >> 8;
    index = index & 0xff;

    if(this->entryPages.size() <= page) return false;

    auto& data = this->entryPages[page][index];

    if(!data.is_occupied) return false;

    data.is_occupied = false;
    data.data = this->default_value;

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