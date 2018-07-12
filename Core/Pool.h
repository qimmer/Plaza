#ifndef POOL_H
#define POOL_H

#include <Core/Types.h>
#include <Core/Vector.h>
#include <Core/Debug.h>
#include <Core/Math.h>
#include <memory>

#define MaxPages 4096
#define PoolPageElements 256

inline unsigned long upper_power_of_two(unsigned long v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

class Pool
{
private:
    u32 elementSize, blockSize;
    Vector<char*> entryPages;
    Vector<u32> freeIndices;
public:
    Pool() {
        blockSize = 0;
        elementSize = 0;
        SetElementSize(1);
    }

    inline char* operator[] (u32 index);
    inline const char* operator[] (u32 index) const;
    inline bool IsValid(u32 index) const;
    inline u32 End() const;

    inline u32 Add();
    inline bool Insert(u32 index);
    inline bool Remove(u32 index);

    inline void SetElementSize(u32 size);
    inline u32 GetElementSize();
};

inline char* Pool::operator[](u32 index)
{
    auto page = (index & 0xffffff00) >> 8;
    index &= 0xff;
    return &this->entryPages[page][index * elementSize];
}

inline void Pool::SetElementSize(u32 size) {
    auto oldBlockSize = blockSize;
    elementSize = size;
    blockSize = upper_power_of_two(std::max(size + 1, (u32)16));

    // Make sure to expand every single existing element with the new size by allocating new pages
    for(auto i = 0; i < entryPages.size(); ++i) {
        auto newPage = (char*)malloc(blockSize * PoolPageElements);
        auto oldPage = entryPages[i];
        for(auto j = 0; j < PoolPageElements; ++j) {
            memcpy(newPage + (j * blockSize), oldPage + (j * oldBlockSize), Min(elementSize, oldBlockSize));
            newPage[j * blockSize + blockSize - 1] = oldPage[j * oldBlockSize + oldBlockSize - 1]; // Make sure to copy occupied flag
        }
        free(oldPage);
        entryPages[i] = newPage;
    }
}

inline bool Pool::IsValid(u32 index) const
{
    auto page = (index & 0xffffff00) >> 8;
    index &= 0xff;

    if(this->entryPages.size() <= page) return false;

    return this->entryPages[page][index * blockSize + blockSize - 1];
}

inline u32 Pool::End() const
{
    return this->entryPages.size() * PoolPageElements;
}

inline bool Pool::Insert(u32 index)
{
    auto page = (index & 0xffffff00) >> 8;
    index &= 0xff;

    if(page >= this->entryPages.size())
    {
        for(auto i = this->entryPages.size(); i <= page; ++i) {
            this->entryPages.emplace_back((char*)calloc(PoolPageElements, blockSize));
        }
    }

    if(this->entryPages[page][index * blockSize + blockSize - 1])
    {
        return false;
    }

    memset(&this->entryPages[page][index * blockSize], 0, blockSize);
    this->entryPages[page][index * blockSize + blockSize - 1] = true;

    for(u32 i = 0; i < this->freeIndices.size(); ++i) {
        if(this->freeIndices[i] == index) {
            this->freeIndices.erase(this->freeIndices.begin() + i);
            break;
        }
    }

    return true;
}

inline bool Pool::Remove(u32 index)
{
    auto page = (index & 0xffffff00) >> 8;
    index = index & 0xff;

    if(this->entryPages.size() <= page) return false;

    auto& data = this->entryPages[page][index];

    if(!this->entryPages[page][index * blockSize + blockSize - 1]) return false;

    this->entryPages[page][index * blockSize + blockSize - 1] = false;

    this->freeIndices.push_back(index);
    return true;
}

inline u32 Pool::Add() {
    u32 index = this->End();
    if(this->freeIndices.size() > 0) {
        index = this->freeIndices[this->freeIndices.size() - 1];
        this->freeIndices.pop_back();
    }

    this->Insert(index);

    return index;
}

inline u32 Pool::GetElementSize() {
    return elementSize;
}

#endif