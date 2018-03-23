//
// Created by Kim Johannsen on 20-03-2018.
//

#ifndef PLAZA_ENUM_H
#define PLAZA_ENUM_H

#include "Handle.h"

DeclareHandle(Enum)

void SetEnumCombinable(Enum e, bool combinable);
bool GetEnumCombinable(Enum e);

void SetEnumFlag(Enum e, StringRef name, u64 value);

u32 GetEnumFlags(Enum e);
StringRef GetEnumFlagName(Enum e, u32 flagIndex);
u32 GetEnumFlagIndexByValue(Enum e, u64 flagValue);
u32 GetEnumFlagIndexByName(Enum e, StringRef name);
u64 GetEnumFlagValue(Enum e, u32 flagIndex);

#define DeclareEnum(NAME) \
    Enum EnumOf_ ## NAME ();

#define DefineEnum(NAME, CANCOMBINE) \
    Enum EnumOf_ ## NAME () {\
        static Enum e = 0; \
        if(!e) {\
            e = CreateEnum();\
            SetEnumCombinable(e, CANCOMBINE);

#define DefineFlag(FLAGNAME) \
            SetEnumFlag(e, #FLAGNAME, FLAGNAME);

#define EndEnum() \
        }\
        return e;\
    }

#endif //PLAZA_ENUM_H
