//
// Created by Kim Johannsen on 03/01/2018.
//

#ifndef PLAZA_VECTOR_H
#define PLAZA_VECTOR_H

#include "Algorithms.h"
#include <memory.h>
#include <memory>

/*
#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/unordered_set.h>
#include <EASTL/set.h>

#include <Core/StackContainer.h>
#include <Core/AlignedAllocator.h>

template<typename T, size_t fixedCapacity = 32> using Vector = eastl::fixed_vector<T, fixedCapacity>;
template<typename T> using Set = eastl::unordered_set<T, std::hash<T>, std::equal_to<T>>;
template<typename T, typename Comparator = std::less<T>> using OrderedSet = eastl::set<T, Comparator>;

*/
static const int ChildArrayStaticCap = 8;

Entity* TempAlloc(u32 amount);
void TempFree();

struct ChildArray {
private:
    u32 Count;
    u32 DynCapacity;
    Entity* DynBuf;
    Entity StaBuf [ChildArrayStaticCap];
    bool Detached;
    inline Entity *GetArray() {
        this->Detach();

        return this->DynBuf ? this->DynBuf : &this->StaBuf[0];
    }
    inline const Entity *GetArrayConst() const {
        return this->DynBuf ? this->DynBuf : &this->StaBuf[0];
    }
public:
    inline void Detach() {
        if(this->DynBuf && !this->Detached) {
            auto attachedBuffer = this->DynBuf;
            auto tempBuf = TempAlloc(this->Count);
            memcpy(tempBuf, this->DynBuf, sizeof(Entity) * this->Count);
            this->DynBuf = tempBuf;
            free(attachedBuffer);
        }

        this->Detached = true;
    }

    inline bool IsDetached() {
        return this->Detached;
    }

    inline void Attach() {
        if(this->DynBuf) {
            auto memory = (Entity*)malloc(sizeof(Entity) * this->DynCapacity);
            memcpy(memory, this->DynBuf, sizeof(Entity) * this->Count);
            this->DynBuf = memory;
            this->Detached = false;
        }
    }

    inline u32 Add(Entity value) {
        auto index = GetIndex(value);
        if(index != InvalidIndex) {
            return index;
        }
        this->SetSize(this->Count + 1);
        this->GetArray()[this->Count - 1] = value;

        return this->Count - 1;
    }
    inline void Remove(u32 index) {
        if(this->Count > index) {
            this->GetArray()[index] = this->GetArrayConst()[this->Count - 1];
            this->SetSize(this->Count - 1);
        }
    }
    inline Entity& operator[](const u32 index) {
        return this->GetArray()[index];
    }
    inline const Entity& operator[](const u32 index) const {
        return this->GetArrayConst()[index];
    }
    inline u32 GetSize() const {
        return this->Count;
    }
    inline void SetSize(u32 newSize) {
        if(this->Count == newSize) return;

        if(newSize > ChildArrayStaticCap) {
            if(!*this->DynBuf) {
                auto newCap = UpperPowerOf2(newSize);
                this->DynBuf = TempAlloc(newCap);
                this->DynCapacity = newCap;
                memcpy(this->DynBuf, this->StaBuf, this->Count * sizeof(Entity));
            } else {
                if(this->DynCapacity < newSize) {
                    auto newCap = UpperPowerOf2(newSize);
                    auto newBuf = TempAlloc(newCap);
                    memcpy(newBuf, this->DynBuf, this->Count * sizeof(Entity));

                    this->DynBuf = newBuf;
                    this->DynCapacity = newCap;
                }
            }
        } else {
            if(this->DynBuf) {
                memcpy(this->StaBuf, this->DynBuf, Min(ChildArrayStaticCap, this->DynCapacity) * sizeof(Entity));
            }
            this->DynCapacity = 0;
            this->DynBuf = 0;
        }

        auto elements = this->DynBuf ? this->DynBuf : &this->StaBuf[0];
        for(auto i = this->Count; i < newSize; ++i) {
            elements[i] = 0;
        }

        this->Count = newSize;
        this->Detached = true;
    }
    inline const Entity *begin() const {
        return GetArrayConst();
    }
    inline const Entity *end() const {
        return begin() + this->Count;
    }
    inline u32 GetIndex(Entity value) {
        auto entities = this->begin();
        for(auto i = 0; i < GetSize(); ++i) {
            if(entities[i] == value) {
                return i;
            }
        }
        return InvalidIndex;
    }
};

#endif //PLAZA_VECTOR_H
