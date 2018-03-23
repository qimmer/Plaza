#ifndef POOL_H
#define POOL_H

#include <Core/Types.h>
#include <Core/Vector.h>

#define MaxPages 4096
#define PoolPageElements 0xFF
#define MemoryGuard(G) char G [sizeof(MemoryGuardData)]
#define AssertGuard(G) Assert(memcmp(G, MemoryGuardData, sizeof(MemoryGuardData)) == 0)
#define InitGuard(G) memcpy(G, MemoryGuardData, sizeof(MemoryGuardData))
static const unsigned char MemoryGuardData[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF};

template<typename T>
class Pool
{
private:
    struct Entry
    {
        Entry() : is_occupied(false) {
			memset(data, 0, sizeof(T));
            InitGuard(preGuard);
            InitGuard(postGuard);
        }

        bool is_occupied;
        MemoryGuard(preGuard);
        char data[sizeof(T)];
        MemoryGuard(postGuard);
    };

    FixedVector<Entry*, 128> entryPages;
    Vector<u32> free_indices;
public:
    ~Pool() {
        for(Entry *page : entryPages) delete[] page;
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
    index = index & 0x000000ff;
    auto& data = this->entryPages[page][index];
    return *(T*)data.data;
}

template<typename T>
const T& Pool<T>::operator[](u32 index) const
{
    Assert(IsValid(index));

    auto page = index & 0xffffff00;
    index = index & 0x000000ff;
    const auto& data = this->entryPages[page][index];
    return *(const T*)data.data;
}

template<typename T>
bool Pool<T>::IsValid(u32 index) const
{
    auto page = index & 0xffffff00;
    index = index & 0x000000ff;

    if(this->entryPages.size() <= page) return false;

    const auto& data = this->entryPages[page][index];
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
    index = index & 0x000000ff;

    Assert(page < MaxPages);
    if(page >= this->entryPages.size())
    {
        this->entryPages.push_back(new Entry[PoolPageElements]);
    }

    if(this->entryPages[page][index].is_occupied)
    {
        return false;
    }

    this->entryPages[page][index].is_occupied = true;
    memset(this->entryPages[page][index].data, 0, sizeof(T));
    new (this->entryPages[page][index].data) T();

    for(u32 i = 0; i < this->free_indices.size(); ++i) {
        if(this->free_indices[i] == index) {
            this->free_indices.erase(this->free_indices.begin() + i);
            break;
        }
    }

    AssertGuard(this->entryPages[page][index].preGuard);
    AssertGuard(this->entryPages[page][index].postGuard);

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
    index = index & 0x000000ff;

    this->entryPages[page][index].is_occupied = false;
    ((T*)this->entryPages[page][index].data)->~T();
    memset(this->entryPages[page][index].data, 0, sizeof(T));
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