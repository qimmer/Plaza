#ifndef PLAZA_HANDLE_H
#define PLAZA_HANDLE_H

#include <Core/Delegate.h>
#include <Core/Debug.h>
#include <Core/Pool.h>
#include <climits>

typedef u32 Generation;
typedef u32 Index;
typedef u64 Handle;

typedef struct {
    u32 index, generation;
} HandleData;

#define GetHandleIndex(handle) (((Index *) &handle)[0])
#define GetHandleGeneration(handle) (((Generation *) &handle)[1])

#define GetHandle(index, generation) ((((Handle)generation) << 32) | index)

#define DeclareHandle(HandleName) \
    extern Vector<Generation> HandleName ## _generations;\
    extern Vector<Index> HandleName ## _freeSlots;\
    typedef Handle HandleName; \
    HandleName Get ## HandleName ## FromIndex (Index index); \
    HandleName GetNext ## HandleName (HandleName handle); \
    HandleName Create ## HandleName(); \
    void Destroy ## HandleName (HandleName handle); \
    typedef void(* HandleName ## Handler)(HandleName handle); \
    DeclareEvent(HandleName ## Created, HandleName ## Handler) \
    DeclareEvent(HandleName ## Destroyed, HandleName ## Handler)\
    inline bool Is ## HandleName ## Occupied(Index index) {\
        return HandleName ## _generations[index] % 2 != 0;\
    }\
    inline bool Is ## HandleName ## Valid(HandleName handle) {\
        return handle && HandleName ## _generations[GetHandleIndex(handle)] == GetHandleGeneration(handle);\
    }

#define DefineHandle(HandleName, DataTypeName) \
    Vector<Generation> HandleName ## _generations;\
    Vector<Index> HandleName ## _freeSlots;\
    Pool<DataTypeName> HandleName ## _pool;\
    DefineEvent(HandleName ## Created, HandleName ## Handler) \
    DefineEvent(HandleName ## Destroyed, HandleName ## Handler) \
    \
    \
    HandleName Get ## HandleName ## FromIndex (Index index) {\
        if(index >= HandleName ## _generations.size()) return 0; \
        if(HandleName ## _generations[index] % 2 == 0) return 0; \
        return GetHandle(index, HandleName ## _generations[index]); \
    }\
    HandleName GetNext ## HandleName (HandleName handle) { \
        auto index = GetHandleIndex(handle); \
        if(index >= HandleName ## _generations.size()) return 0; \
        if(!handle) return GetHandle(0, HandleName ## _generations[0]); \
        index++;\
        while(!Is ## HandleName ## Occupied (index)) { \
            if(index >= HandleName ## _generations.size()) return 0; \
            index++; \
        } \
        auto gen = HandleName ## _generations[index]; \
        return GetHandle(index, gen); \
    } \
    \
    HandleName Create ## HandleName () {\
        Index index;\
        if (HandleName ## _freeSlots.size()) {\
            index = HandleName ## _freeSlots[HandleName ## _freeSlots.size() - 1];\
            HandleName ## _freeSlots.pop_back();\
        } else {\
            HandleName ## _generations.push_back(0);\
            index = HandleName ## _generations.size() - 1;\
        }\
        \
        HandleName ## _generations[index]++;\
        \
        Assert(HandleName ## _generations[index] % 2 != 0);\
        \
        auto handle = GetHandle(index, HandleName ## _generations[index]);\
        HandleName ## _pool.Insert(index);\
        \
        FireEvent(HandleName ## Created, handle);\
        return handle;\
    }\
    \
    void Destroy ## HandleName(HandleName handle) {\
        if (!Is ## HandleName ## Valid(handle)) {\
            return;\
        }\
        \
        FireEvent(HandleName ## Destroyed, handle);\
        \
        auto index = GetHandleIndex(handle);\
        HandleName ## _pool.Remove(index);\
        HandleName ## _freeSlots.push_back(index);\
        ++ HandleName ## _generations[index];\
        \
        Assert(HandleName ## _generations[index] % 2 == 0);\
    }\
    DataTypeName* HandleName ## At(HandleName handle) { \
        return & HandleName ## _pool[GetHandleIndex(handle)];\
    }

#endif