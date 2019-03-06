//
// Created by Kim on 17-10-2018.
//

#ifndef PLAZA_FUNCTIONCOMMONSIGNATURES_H
#define PLAZA_FUNCTIONCOMMONSIGNATURES_H

#include <Core/NativeUtils.h>
#include <Core/Variant.h>

NativeFunctionInvokerVoid0()
NativeFunctionInvokerVoid1(Entity)
NativeFunctionInvokerVoid2(Entity, Entity)
NativeFunctionInvokerVoid3(Entity, Entity, Variant)
NativeFunctionInvoker1(bool, Entity)
NativeFunctionInvoker1(bool, u32)
NativeFunctionInvoker1(Variant, Variant)
NativeFunctionInvoker2(Variant, Variant, Variant)
NativeFunctionInvoker2(Variant, Entity, Entity)

#define NoArgInvoker(Type) \
    NativeFunctionInvoker0(Type)

#define PropertyChangedInvoker(Type) \
    NativeFunctionInvokerVoid3(Entity, Type, Type)

#define RegisterNoArgInvoker(Type)\
    RegisterFunctionSignature(NativeFunctionInvoker ## _ ## Type, Type);

#define RegisterPropertyChangedInvoker(Type)\
    RegisterFunctionSignature(NativeFunctionInvoker ## _void_ ## Entity ## _ ## Type ## _ ## Type, void, Entity, Type, Type);

#define TypeInvoker(...) \
    NoArgInvoker(__VA_ARGS__)\
    PropertyChangedInvoker(__VA_ARGS__)

#define RegisterTypeInvoker(...) \
    RegisterNoArgInvoker(__VA_ARGS__)\
    RegisterPropertyChangedInvoker(__VA_ARGS__)

TypeInvoker(u8)
TypeInvoker(u16)
TypeInvoker(u32)
TypeInvoker(u64)
TypeInvoker(s8)
TypeInvoker(s16)
TypeInvoker(s32)
TypeInvoker(s64)
TypeInvoker(float)
TypeInvoker(double)
TypeInvoker(bool)
TypeInvoker(Entity)
TypeInvoker(NativePtr)
TypeInvoker(StringRef)
TypeInvoker(Type)
TypeInvoker(v2f)
TypeInvoker(v2i)
TypeInvoker(v3f)
TypeInvoker(v3i)
TypeInvoker(v4f)
TypeInvoker(v4i)
TypeInvoker(rgb8)
TypeInvoker(rgba8)
TypeInvoker(rgb32)
TypeInvoker(rgba32)
TypeInvoker(Date)
TypeInvoker(Variant)

void RegisterCommonSignatures();

#endif //PLAZA_FUNCTIONCOMMONSIGNATURES_H
