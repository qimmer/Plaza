//
// Created by Kim on 17-10-2018.
//

#include "FunctionCommonSignatures.h"

void RegisterCommonSignatures() {
    RegisterFunctionSignature(NativeFunctionInvoker_void, void);
    RegisterFunctionSignature(NativeFunctionInvoker_Entity, Entity);
    RegisterFunctionSignature(NativeFunctionInvoker_void_Entity, void, Entity);
    RegisterFunctionSignature(NativeFunctionInvoker_void_Entity_Entity, void, Entity, Entity);
    RegisterFunctionSignature(NativeFunctionInvoker_bool_Entity, bool, Entity);
    RegisterFunctionSignature(NativeFunctionInvoker_bool_u32, bool, u32);
    RegisterFunctionSignature(NativeFunctionInvoker_Variant_Variant, Variant, Variant);
    RegisterFunctionSignature(NativeFunctionInvoker_Variant_Variant_Variant, Variant, Variant, Variant);

    RegisterTypeInvoker(u8)
    RegisterTypeInvoker(u16)
    RegisterTypeInvoker(u32)
    RegisterTypeInvoker(u64)
    RegisterTypeInvoker(s8)
    RegisterTypeInvoker(s16)
    RegisterTypeInvoker(s32)
    RegisterTypeInvoker(s64)
    RegisterTypeInvoker(float)
    RegisterTypeInvoker(double)
    RegisterTypeInvoker(bool)
    RegisterTypeInvoker(Entity)
    RegisterTypeInvoker(NativePtr)
    RegisterTypeInvoker(StringRef)
    RegisterTypeInvoker(Type)
    RegisterTypeInvoker(v2f)
    RegisterTypeInvoker(v2i)
    RegisterTypeInvoker(v3f)
    RegisterTypeInvoker(v3i)
    RegisterTypeInvoker(v4f)
    RegisterTypeInvoker(v4i)
    RegisterTypeInvoker(rgb8)
    RegisterTypeInvoker(rgba8)
    RegisterTypeInvoker(rgb32)
    RegisterTypeInvoker(rgba32)
    RegisterTypeInvoker(m3x3f)
    RegisterTypeInvoker(m4x4f)
    RegisterTypeInvoker(Date)
    RegisterTypeInvoker(Variant)
}

