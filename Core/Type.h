#ifndef PLAZA_TYPE_H
#define PLAZA_TYPE_H

#include <Core/Handle.h>
#include <Core/Types.h>
#include <Core/Delegate.h>

#define DefineType(TYPENAME) \
    Type TypeOf_ ## TYPENAME () { \
        static Type type = 0; \
        if(!type) { \
            type = CreateType(); \
            SetTypeName(type, #TYPENAME);\
            SetTypeSize(type, sizeof(TYPENAME));

#define EndType() \
        } \
        return type; \
    }

#define DeclareType(TYPENAME) Type TypeOf_ ## TYPENAME ();

#define TypeOf(TYPENAME) TypeOf_ ## TYPENAME ()

DeclareHandle(Type)

void SetTypeName(Type type, const char* name);
const char* GetTypeName(Type type);
u32 GetTypeSize(Type type);
void SetTypeSize(Type type, u32 size);
Type GetTypeFromName(StringRef name);

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