#ifndef POOL_H
#define POOL_H

#include <Core/Types.h>
#include <Core/Vector.h>
#include <Core/Debug.h>
#include <Core/Math.h>
#include <memory>

#define MaxPages 4096
#define PoolPageElements 256

#define MemoryGuard 1

#define AvailableBlock 0
#define OccupiedBlock 32

static const u64 MemoryMagic = 0xdeadbeefdeadbeef;
static const size_t MemoryGuardSize = sizeof(u64);

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
    u32 elementSize, blockSize, endIndex;
    Vector<char*> entryPages;
    Vector<u32> freeIndices;
public:
    Pool() {
        blockSize = 0;
        elementSize = 0;
        endIndex = 0;
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
    auto block = &this->entryPages[page][index * blockSize];

#if MemoryGuard
    auto firstGuard = (u64*)block;
    auto element = block + MemoryGuardSize;
    auto lastGuard = (u64*)(element + elementSize);

    Assert(0, *firstGuard == MemoryMagic);
    Assert(0, *lastGuard == MemoryMagic);

    return element;
#else
    return block;
#endif
}

inline void Pool::SetElementSize(u32 size) {
    Assert(0, size > 0);

    auto oldBlockSize = blockSize;
    auto oldElementSize = elementSize;
    elementSize = size;

#if MemoryGuard
    blockSize = upper_power_of_two(size + 1 + MemoryGuardSize * 2);
#else
    blockSize = upper_power_of_two(size + 1);
#endif
    // Make sure to expand every single existing element with the new size by allocating new pages
    for(auto i = 0; i < entryPages.size(); ++i) {
        auto newPage = (char*)calloc(PoolPageElements, blockSize);

#if MemoryGuard
        // Set magic numbers before and after element data
        for(auto j = 0; j < PoolPageElements; ++j) {
            auto firstMagic = (u64*)&newPage[j * blockSize];
            auto lastMagic = (u64*)&newPage[j * blockSize + elementSize + MemoryGuardSize];
            *firstMagic = *lastMagic = MemoryMagic;
        }
#endif

        auto oldPage = entryPages[i];

        if(oldBlockSize > 0) {
            for (auto j = 0; j < PoolPageElements; ++j) {
                auto newBlock = newPage + (j * blockSize);
                auto oldBlock = oldPage + (j * oldBlockSize);
#if MemoryGuard
                memcpy(newBlock + MemoryGuardSize, oldBlock + MemoryGuardSize, Min(elementSize, oldElementSize));
#else
                memcpy(newBlock, oldBlock, Min(elementSize, oldElementSize));
#endif
                newBlock[blockSize - 1] = oldBlock[oldBlockSize - 1];
            }

            free(oldPage);
        }

        entryPages[i] = newPage;
    }
}

inline bool Pool::IsValid(u32 index) const
{
    auto page = (index & 0xffffff00) >> 8;
    index &= 0xff;

    if(this->entryPages.size() <= page) return false;

    auto block = &this->entryPages[page][index * blockSize];
    auto blockState = block[blockSize - 1];

#if MemoryGuard
    auto firstGuard = (u64*)block;
    auto element = block + MemoryGuardSize;
    auto lastGuard = (u64*)(element + elementSize);

    Assert(0, *firstGuard == MemoryMagic);
    Assert(0, *lastGuard == MemoryMagic);
    Assert(0, blockState == AvailableBlock || blockState == OccupiedBlock);
#endif

    return blockState == OccupiedBlock;
}

inline u32 Pool::End() const
{
    return this->endIndex;
}

inline bool Pool::Insert(u32 index)
{
    auto absoluteIndex = index;

    Assert(0, this->elementSize > 0);
    auto page = (index & 0xffffff00) >> 8;
    index &= 0xff;

    if(page >= this->entryPages.size())
    {
        for(auto i = this->entryPages.size(); i <= page; ++i) {
            auto newPage = (char*)calloc(PoolPageElements, blockSize);
            this->entryPages.push_back(newPage);

#if MemoryGuard
            // Set magic numbers before and after element data
            for(auto j = 0; j < PoolPageElements; ++j) {
                auto firstMagic = (u64*)&newPage[j * blockSize];
                auto lastMagic = (u64*)&newPage[j * blockSize + elementSize + MemoryGuardSize];
                *firstMagic = *lastMagic = MemoryMagic;
            }
#endif
        }
    }

    auto block = &this->entryPages[page][index * blockSize];
    auto blockState = block[blockSize - 1];

#if MemoryGuard
    Assert(0, blockState == AvailableBlock || blockState == OccupiedBlock);
#endif

    if(blockState == OccupiedBlock)
    {
        return false;
    }

#if MemoryGuard
    memset(block + MemoryGuardSize, 0, elementSize);
#else
    memset(block, 0, elementSize);
#endif
    block[blockSize - 1] = OccupiedBlock;

    for(u32 i = 0; i < this->freeIndices.size(); ++i) {
        if(this->freeIndices[i] == absoluteIndex) {
            this->freeIndices.erase(this->freeIndices.begin() + i);
            break;
        }
    }

    this->endIndex = Max(this->endIndex, absoluteIndex + 1);

    return true;
}

inline bool Pool::Remove(u32 index)
{
    auto absoluteIndex = index;

    auto page = (index & 0xffffff00) >> 8;
    index = index & 0xff;

    if(this->entryPages.size() <= page) return false;

    auto block = &this->entryPages[page][index * blockSize];

    if(block[blockSize - 1] == AvailableBlock) return false;

    block[blockSize - 1] = AvailableBlock;

    this->freeIndices.push_back(absoluteIndex);
    return true;
}

inline u32 Pool::Add() {
    Assert(0, this->elementSize > 0);

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