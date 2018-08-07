//
// Created by Kim Johannsen on 01-03-2018.
//

#include <dyncall/dyncall.h>
#include <dyncall/dyncall_signature.h>
#include <dyncall/dyncall_struct.h>
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

static DCCallVM *CallVM = 0;
static DCstruct *Struct_v2i, *Struct_v3i, *Struct_v4i, *Struct_v2f, *Struct_v3f, *Struct_v4f, *Struct_m3x3f, *Struct_m4x4f;

#define ALIGNMENT_SSE 16

struct FunctionArgument {
    Type FunctionArgumentType;
};

struct Function {
    u64 FunctionImplementation;
    Type FunctionReturnType;
    FunctionCaller FunctionCaller;

    Vector(FunctionArguments, Entity, 16)
};

BeginUnit(Function)
    BeginComponent(Function)
        RegisterProperty(u64, FunctionImplementation)
        RegisterProperty(Type, FunctionReturnType)
        RegisterArrayProperty(FunctionArgument, FunctionArguments)
    EndComponent()

    BeginComponent(FunctionArgument)
        RegisterProperty(Type, FunctionArgumentType)
    EndComponent()
EndUnit()


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

#define _CallFunction_HandleType(TYPE, DCTYPE) \
    case TypeOf_ ## TYPE :\
        dcArg ## DCTYPE(CallVM, *((TYPE*)argumentDataPtrs[i]));\
        break;

#define _CallFunction_HandlePointerType(TYPE) \
    case TypeOf_ ## TYPE :\
        dcArgPointer(CallVM, const_cast<char*>(*((TYPE*)argumentDataPtrs[i])));\
        break;

#define _CallFunction_HandleVecType(TYPE) \
    case TypeOf_ ## TYPE :\
        dcArgStruct(CallVM, Struct_ ## TYPE, const_cast<void*>((const void*)argumentDataPtrs[i]));\
        break;


#define _CallFunction_HandleCall(TYPE, DCTYPE) \
    case TypeOf_ ## TYPE :\
        *((TYPE*)returnData) = dcCall ## DCTYPE(CallVM, funcPtr);\
        break;

#define _CallFunction_HandlePointerCall(TYPE) \
    case TypeOf_ ## TYPE :\
        *((TYPE*)returnData) = (TYPE)dcCallPointer(CallVM, funcPtr);\
        break;

#define _CallFunction_HandleVecCall(TYPE) \
    case TypeOf_ ## TYPE :\
        dcCallStruct(CallVM, funcPtr, Struct_ ## TYPE, returnData);\
        break;

static void Cleanup() {
    dcFreeStruct(Struct_v2i);
    dcFreeStruct(Struct_v3i);
    dcFreeStruct(Struct_v4i);
    dcFreeStruct(Struct_v2f);
    dcFreeStruct(Struct_v3f);
    dcFreeStruct(Struct_v4f);
    dcFreeStruct(Struct_m3x3f);
    dcFreeStruct(Struct_m4x4f);
    dcFree(CallVM);
    CallVM = 0;
}

static void Initialize() {
    CallVM = dcNewCallVM(4096);
    dcMode(CallVM, DC_CALL_C_DEFAULT);
    dcReset(CallVM);

    Struct_v2i = dcNewStruct(1, 16);
    dcStructField(Struct_v2i, DC_SIGCHAR_INT, DEFAULT_ALIGNMENT, 2);
    dcCloseStruct(Struct_v2i);

    Struct_v3i = dcNewStruct(1, 16);
    dcStructField(Struct_v3i, DC_SIGCHAR_INT, DEFAULT_ALIGNMENT, 3);
    dcCloseStruct(Struct_v3i);

    Struct_v4i = dcNewStruct(1, 16);
    dcStructField(Struct_v4i, DC_SIGCHAR_INT, 16, 4);
    dcCloseStruct(Struct_v4i);

    Struct_v2f = dcNewStruct(1, 16);
    dcStructField(Struct_v2f, DC_SIGCHAR_FLOAT, DEFAULT_ALIGNMENT, 2);
    dcCloseStruct(Struct_v2f);

    Struct_v3f = dcNewStruct(1, 16);
    dcStructField(Struct_v3f, DC_SIGCHAR_FLOAT, DEFAULT_ALIGNMENT, 3);
    dcCloseStruct(Struct_v3f);

    Struct_v4f = dcNewStruct(1, 16);
    dcStructField(Struct_v4f, DC_SIGCHAR_FLOAT, 16, 4);
    dcCloseStruct(Struct_v4f);

    Struct_m3x3f = dcNewStruct(1, 16);
    dcStructField(Struct_m3x3f, DC_SIGCHAR_FLOAT, DEFAULT_ALIGNMENT, 3*3);
    dcCloseStruct(Struct_m3x3f);

    Struct_m4x4f = dcNewStruct(1, 16);
    dcStructField(Struct_m4x4f, DC_SIGCHAR_FLOAT, 16, 4*4);
    dcCloseStruct(Struct_m4x4f);
}

API_EXPORT bool CallNativeFunction(
        u64 funcImplementation,
        u8 returnArgumentTypeIndex,
        void *returnData,
        u32 numArguments,
        const u8 *argumentTypes,
        const void **argumentDataPtrs
) {
    static bool initialized = false;

    if(!initialized) {
        atexit(Cleanup);
        Initialize();
        initialized = true;
    }

    dcReset(CallVM);

    auto funcPtr = (void*)funcImplementation;

    for(auto i = 0; i < numArguments; ++i) {
        switch(argumentTypes[i]) {
            _CallFunction_HandleType(Type, Char)
            _CallFunction_HandleType(bool, Bool)
            _CallFunction_HandleType(u8, Char)
            _CallFunction_HandleType(u16, Short)
            _CallFunction_HandleType(u32, Int)
            _CallFunction_HandleType(u64, LongLong)
            _CallFunction_HandleType(s8, Char)
            _CallFunction_HandleType(s16, Short)
            _CallFunction_HandleType(s32, Int)
            _CallFunction_HandleType(s64, LongLong)
            _CallFunction_HandleType(float, Float)
            _CallFunction_HandleType(double, Double)
            _CallFunction_HandlePointerType(StringRef)
            _CallFunction_HandleType(Entity, LongLong)

            _CallFunction_HandleVecType(v2i)
            _CallFunction_HandleVecType(v3i)
            _CallFunction_HandleVecType(v4i)
            _CallFunction_HandleVecType(v2f)
            _CallFunction_HandleVecType(v3f)
            _CallFunction_HandleVecType(v4f)

            default:
                Log(0, LogSeverity_Error, "Error calling native function. Argument type '%s' is unsupported.", GetName(argumentTypes[i]));
                return false;
        }
    }

    switch(returnArgumentTypeIndex) {
        _CallFunction_HandleCall(Type, Char)
        _CallFunction_HandleCall(bool, Bool)
        _CallFunction_HandleCall(u8, Char)
        _CallFunction_HandleCall(u16, Short)
        _CallFunction_HandleCall(u32, Int)
        _CallFunction_HandleCall(u64, LongLong)
        _CallFunction_HandleCall(s8, Char)
        _CallFunction_HandleCall(s16, Short)
        _CallFunction_HandleCall(s32, Int)
        _CallFunction_HandleCall(s64, LongLong)
        _CallFunction_HandleCall(float, Float)
        _CallFunction_HandleCall(double, Double)
        _CallFunction_HandlePointerCall(StringRef)
        _CallFunction_HandleCall(Entity, LongLong)

        _CallFunction_HandleVecCall(v2i)
        _CallFunction_HandleVecCall(v3i)
        _CallFunction_HandleVecCall(v4i)
        _CallFunction_HandleVecCall(v2f)
        _CallFunction_HandleVecCall(v3f)
        _CallFunction_HandleVecCall(v4f)

        case TypeOf_void:
            dcCallVoid(CallVM, funcPtr);
            break;

        default:
            Log(0, LogSeverity_Error, "Error calling native function. Return type '%s' is unsupported.", GetTypeName(returnArgumentTypeIndex));
            return false;
    }

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

    return data->FunctionCaller(
            data->FunctionImplementation,
            data->FunctionReturnType,
            returnData,
            numArguments,
            argumentTypes,
            argumentDataPtrs
    );
}

void __InitializeFunction() {
    auto component = ComponentOf_Function();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Function));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_FunctionImplementation(), offsetof(Function, FunctionImplementation), sizeof(Function::FunctionImplementation), TypeOf_u64,  component, 0, PropertyKind_Value);
    __Property(PropertyOf_FunctionReturnType(), offsetof(Function, FunctionReturnType), sizeof(Function::FunctionReturnType), TypeOf_Type,  component, 0, PropertyKind_Value);
    __Property(PropertyOf_FunctionArguments(), offsetof(Function, FunctionArguments), sizeof(Function::FunctionArguments), TypeOf_Entity,  component, ComponentOf_FunctionArgument(), PropertyKind_Array);

    component = ComponentOf_FunctionArgument();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(FunctionArgument));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_FunctionArgumentType(), offsetof(FunctionArgument, FunctionArgumentType), sizeof(FunctionArgument::FunctionArgumentType), TypeOf_Type,  component, 0, PropertyKind_Value);
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

API_EXPORT FunctionCaller GetFunctionCaller(Entity function) {
    auto data = GetFunctionData(function);
    return data->FunctionCaller;
}

API_EXPORT void SetFunctionCaller(Entity function, FunctionCaller caller) {
    auto data = GetFunctionData(function);
    data->FunctionCaller = caller;
}

