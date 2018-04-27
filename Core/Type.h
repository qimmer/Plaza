#ifndef PLAZA_TYPE_H
#define PLAZA_TYPE_H

#include <Core/Handle.h>
#include <Core/Types.h>

#define DefineType(TYPENAME) \
    void _InitType_ ## TYPENAME(Type *handle);\
    Type TypeOf_ ## TYPENAME () { \
        static Type type = 0; \
        if(!type) _InitType_ ## TYPENAME(&type); \
        return type;\
    }\
    void _InitType_ ## TYPENAME(Type *handle) {\
        auto type = CreateType(); \
        *handle = type;\
        SetTypeName(*handle, #TYPENAME);\
        SetTypeSize(*handle, sizeof(TYPENAME));\

#define EndType() \
    }

#define DeclareType(TYPENAME) Type TypeOf_ ## TYPENAME ();

#define TypeOf(TYPENAME) TypeOf_ ## TYPENAME ()

DeclareHandle(Type)

void SetTypeName(Type type, const char* name);
const char* GetTypeName(Type type);
u32 GetTypeSize(Type type);
void SetTypeSize(Type type, u32 size);
Type FindTypeByName(StringRef name);

DeclareType(void)
DeclareType(u8)
DeclareType(u16)
DeclareType(u32)
DeclareType(u64)
DeclareType(s8)
DeclareType(s16)
DeclareType(s32)
DeclareType(s64)
DeclareType(float)
DeclareType(double)
DeclareType(StringRef)
DeclareType(bool)

DeclareType(v2i)
DeclareType(v3i)
DeclareType(v4i)
DeclareType(v2f)
DeclareType(v3f)
DeclareType(v4f)
DeclareType(rgba8)
DeclareType(rgba32)
DeclareType(rgb8)
DeclareType(rgb32)

DeclareType(m3x3f)
DeclareType(m4x4f)

DeclareType(Type)


#endif