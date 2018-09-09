//
// Created by Kim Johannsen on 01-03-2018.
//

#include <string.h>

#include <Core/Function.h>
#include <Core/Vector.h>
#include <Core/Pool.h>
#include <Core/Types.h>
#include <cstdarg>
#include "Entity.h"
#include "Component.h"
#include "Property.h"
#include "Identification.h"

#include <ffi.h>

static ffi_type
        type_v2i, type_v3i, type_v4i,
        type_v2f, type_v3f, type_v4f,
        type_m3x3f, type_m4x4f;

static ffi_type* type_v2i_elements[] = {
        &ffi_type_sint32,
        &ffi_type_sint32,
        NULL
};

static ffi_type* type_v3i_elements[] = {
        &ffi_type_sint32,
        &ffi_type_sint32,
        &ffi_type_sint32,
        NULL
};

static ffi_type* type_v4i_elements[] = {
        &ffi_type_sint32,
        &ffi_type_sint32,
        &ffi_type_sint32,
        &ffi_type_sint32,
        NULL
};

static ffi_type* type_v2f_elements[] = {
        &ffi_type_float,
        &ffi_type_float,
        NULL
};

static ffi_type* type_v3f_elements[] = {
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,
        NULL
};

static ffi_type* type_v4f_elements[] = {
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,
        NULL
};

static ffi_type* type_m3x3f_elements[] = {
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,

        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,

        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,

        NULL
};

static ffi_type* type_m4x4f_elements[] = {
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,

        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,

        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,

        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,
        &ffi_type_float,

        NULL
};

static ffi_type* ffi_types[] = {
        &ffi_type_void, //TypeOf_unknown,

        &ffi_type_void, //TypeOf_void,
        &ffi_type_uint8, //TypeOf_u8,
        &ffi_type_uint16, //TypeOf_u16,
        &ffi_type_uint32, //TypeOf_u32,
        &ffi_type_uint64, //TypeOf_u64,
        &ffi_type_sint8, //TypeOf_s8,
        &ffi_type_sint16, //TypeOf_s16,
        &ffi_type_sint32, //TypeOf_s32,
        &ffi_type_sint64, //TypeOf_s64,
        &ffi_type_float, //TypeOf_float,
        &ffi_type_double, //TypeOf_double,
        &ffi_type_uint8, //TypeOf_bool,
        &ffi_type_pointer, //TypeOf_StringRef,

        &type_v2i, //TypeOf_v2i,
        &type_v3i, //TypeOf_v3i,
        &type_v4i, //TypeOf_v4i,

        &type_v2f, //TypeOf_v2f,
        &type_v3f, //TypeOf_v3f,
        &type_v4f, //TypeOf_v4f,

        &type_m3x3f, //TypeOf_m3x3f,
        &type_m4x4f, //TypeOf_m4x4f,

        &ffi_type_uint64, //TypeOf_Entity,
        &ffi_type_uint8, //TypeOf_Type,

        &ffi_type_uint32, //TypeOf_rgba8,
        &ffi_type_uint32, //TypeOf_rgb8,

        &type_v4f, //TypeOf_rgba32,
        &type_v3f, //TypeOf_rgb32,

        &ffi_type_uint64, //TypeOf_Date,
};

#define ALIGNMENT_SSE 16

struct FunctionArgument {
    Type FunctionArgumentType;
};

struct Function {
    Type FunctionReturnType;
    FunctionCallerType FunctionCaller;

    Vector(FunctionArguments, Entity, 16)
};

struct NativeFunction {
    ffi_cif cif;
    ffi_type** args;
    void(*funcPtr)();
};

API_EXPORT void SetNativeFunctionPointer(Entity function, void *ptr) {
    AddComponent(function, ComponentOf_NativeFunction());
    auto data = GetNativeFunctionData(function);

    if(data->args) {
        free(data->args);
        data->args = NULL;
    }

    data->funcPtr = (void(*)())ptr;

    SetFunctionCaller(function, NULL);

    if(ptr) {

        auto numArgs = GetNumFunctionArguments(function);
        auto args = GetFunctionArguments(function);
        Assert(function, numArgs < 32);

        auto argPtrs = data->args = (ffi_type**)malloc(sizeof(ffi_type*) * numArgs);

        auto i = 0;
        for(i = 0; i < numArgs; ++i) {
            argPtrs[i] = ffi_types[GetFunctionArgumentType(args[i])];
            Assert(function, argPtrs[i]);
        }

        ffi_prep_cif(
                &data->cif,
                FFI_DEFAULT_ABI,
                numArgs,
                ffi_types[GetFunctionReturnType(function)],
                argPtrs
        );

        SetFunctionCaller(function, CallNativeFunction);
    }
}

LocalFunction(OnNativeFunctionRemoved, void, Entity component, Entity entity) {
    SetNativeFunctionPointer(entity, NULL);
}

API_EXPORT void SetFunctionArgsByDecl(Entity f, StringRef arguments) {
    auto len = strlen(arguments);
    auto offset = 0;
    auto byteOffset = 0;

    char argumentSplits[PathMax];
    strcpy(argumentSplits, arguments);

    while(offset < len) {
        auto argument = &argumentSplits[0] + offset;
        auto nextComma = strchr(&argumentSplits[0] + offset, ',');
        if(!nextComma) nextComma = &argumentSplits[0] + len;
        auto argumentLength = nextComma - argument;
        offset += argumentLength + 1;
        *nextComma = '\0';

        auto name = strrchr(argument, ' ');
        Assert(f, name);
        *name = '\0';
        name++;

        auto typeSignature = argument;
        auto typeSignatureEnd = name - 2;
        // Trim space
        while(isspace(*typeSignature)) typeSignature++;
        while(isspace(*typeSignatureEnd) && typeSignatureEnd > typeSignature) {
            *typeSignatureEnd = '\0';
            typeSignatureEnd--;
        }

        auto argumentEntity = AddFunctionArguments(f);
        SetName(argumentEntity, name);

        auto type = 0;
        for(auto i = 0; i < TypeOf_MAX; ++i) {
            if(strcmp(typeSignature, GetTypeName(i)) == 0) {
                type = i;
                break;
            }
        }

        if(!type) {
            Log(f, LogSeverity_Error, "Unsupported function argument type: %s", typeSignature);
            return;
        }

        SetFunctionArgumentType(argumentEntity, type);
        byteOffset += GetTypeSize(type);
    }
}

API_EXPORT bool CallNativeFunction(
        Entity function,
        void *returnData,
        u32 numArguments,
        const u8 *argumentTypes,
        const void **argumentDataPtrs
) {
    auto data = GetNativeFunctionData(function);

    ffi_call(&data->cif, data->funcPtr, returnData, const_cast<void**>(argumentDataPtrs));

    return true;
}

API_EXPORT bool CallFunction(
        Entity f,
        void *returnData,
        u32 numArguments,
        const u8 *argumentTypes,
        const void **argumentDataPtrs
) {
    auto data = GetFunctionData(f);

    Verbose(VerboseLevel_Invocations, "Calling function %s ...", GetName(f));

    const void **finalArgumentPtrs = (const void **)alloca(numArguments * sizeof(void*));

    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    for(auto i = 0; i < data->FunctionArguments.Count; ++i) {
        if(i >= numArguments) {
            finalArgumentPtrs[i] = nullData;
        } else {
            finalArgumentPtrs[i] = argumentDataPtrs[i];
        }
    }

    if(numArguments < data->FunctionArguments.Count) {
        Log(f, LogSeverity_Error, "Insufficient function arguments provided when calling %s", GetDebugName(f));
        return false;
    }

    return data->FunctionCaller(
            f,
            returnData,
            numArguments,
            argumentTypes,
            argumentDataPtrs
    );
}

#define INIT_FFI_STRUCT(T) \
    type_ ## T .size = type_ ## T .alignment = 0;\
    type_ ## T .type = FFI_TYPE_STRUCT;\
    type_ ## T .elements = type_ ## T ## _elements

void __InitializeFunction() {
    auto component = ComponentOf_Function();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Function));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_FunctionReturnType(), offsetof(Function, FunctionReturnType), sizeof(Function::FunctionReturnType), TypeOf_Type,  component, 0, PropertyKind_Value);
    __Property(PropertyOf_FunctionArguments(), offsetof(Function, FunctionArguments), sizeof(Function::FunctionArguments), TypeOf_Entity,  component, ComponentOf_FunctionArgument(), PropertyKind_Array);

    component = ComponentOf_FunctionArgument();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(FunctionArgument));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_FunctionArgumentType(), offsetof(FunctionArgument, FunctionArgumentType), sizeof(FunctionArgument::FunctionArgumentType), TypeOf_Type,  component, 0, PropertyKind_Value);

    component = ComponentOf_NativeFunction();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(NativeFunction));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());

    INIT_FFI_STRUCT(v2i);
    INIT_FFI_STRUCT(v3i);
    INIT_FFI_STRUCT(v4i);
    INIT_FFI_STRUCT(v2f);
    INIT_FFI_STRUCT(v3f);
    INIT_FFI_STRUCT(v4f);
    INIT_FFI_STRUCT(m3x3f);
    INIT_FFI_STRUCT(m4x4f);
}

int __ArgStackOffset(int value) {
    int val = 0;
    auto offset = ((char*)&value - (char*)&val);
    return offset;
}

API_EXPORT u32 GetFunctionArguments(u32 functionIndex, u32 maxArguments, Type *argumentTypes) {
    auto data = (Function*) GetComponentBytes(ComponentOf_Function(), functionIndex);

    for(auto i = 0; i < data->FunctionArguments.Count; ++i) {
        auto argumentData = GetFunctionArgumentData(GetVector(data->FunctionArguments)[i]);
        argumentTypes[i] = argumentData->FunctionArgumentType;
    }

    return data->FunctionArguments.Count;
}

API_EXPORT Type GetFunctionReturnTypeByIndex(u32 functionIndex) {
    auto data = (Function*) GetComponentBytes(ComponentOf_Function(), functionIndex);
    return data->FunctionReturnType;
}

API_EXPORT FunctionCallerType GetFunctionCaller(Entity function) {
    auto data = GetFunctionData(function);
    return data->FunctionCaller;
}

API_EXPORT void SetFunctionCaller(Entity function, FunctionCallerType caller) {
    auto data = GetFunctionData(function);
    if(data) {
        data->FunctionCaller = caller;
    }
}


BeginUnit(Function)
    BeginComponent(Function)
        RegisterProperty(Type, FunctionReturnType)
        RegisterArrayProperty(FunctionArgument, FunctionArguments)
    EndComponent()

    BeginComponent(FunctionArgument)
        RegisterProperty(Type, FunctionArgumentType)
    EndComponent()

    BeginComponent(NativeFunction)
        // RegisterBase(Function)
    EndComponent()

    RegisterSubscription(EntityComponentRemoved, OnNativeFunctionRemoved, ComponentOf_NativeFunction())
EndUnit()
