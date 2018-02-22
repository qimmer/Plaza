//
// Created by Kim Johannsen on 02/01/2018.
//

#include <Core/Type.h>
#include <Core/String.h>
#include <Core/Vector.h>
#include <Core/Pool.h>


    struct TypeData {
        String name;
        u32 Size;
        Vector<Type> dependencies;
        Vector<Type> drivers;
    };

    DefineHandle(Type, TypeData)

    const char *GetTypeName(Type type) {
        return TypeAt(type).name.c_str();
    }

    void SetTypeName(Type type, const char *name) {
        TypeAt(type).name = name;
    }

    u32 GetTypeSize(Type type) {
        return TypeAt(type).Size;
    }

    void SetTypeSize(Type type, u32 size) {
        TypeAt(type).Size = size;
    }

    DefineType(u8)
    EndType()

    DefineType(u16)
    EndType()

    DefineType(u32)
    EndType()

    DefineType(u64)
    EndType()

    DefineType(s8)
    EndType()

    DefineType(s16)
    EndType()

    DefineType(s32)
    EndType()

    DefineType(s64)
    EndType()

    DefineType(float)
    EndType()

    DefineType(double)
    EndType()

    DefineType(StringRef)
    EndType()

    DefineType(bool)
    EndType()

    DefineType(v2i)
    EndType()

    DefineType(v3i)
    EndType()

    DefineType(v4i)
    EndType()

    DefineType(v2f)
    EndType()

    DefineType(v3f)
    EndType()

    DefineType(v4f)
    EndType()

    DefineType(rgba8)
    EndType()

    DefineType(rgba32)
    EndType()

    DefineType(rgb32)
    EndType()

    DefineType(rgb8)
    EndType()

    DefineType(m3x3f)
    EndType()

    DefineType(m4x4f)
    EndType()

    DefineType(Type)
    EndType()

    Type GetTypeFromName(StringRef name) {
        for(auto type = GetNextType(0); IsTypeValid(type); type = GetNextType(type)) {
            if(strcmp(GetTypeName(type), name) == 0) {
                return type;
            }
        }

        return 0;
    }
