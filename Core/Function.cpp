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
#include "Date.h"
#include "NativeFunction.h"

#include "FunctionCommonSignatures.h"

#include <time.h>

#define Verbose_Function "function"

#define ALIGNMENT_SSE 16

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

API_EXPORT Variant CallFunction(
        Entity f,
        u32 numArguments,
        const Variant *arguments
) {
    thread_local u32 callStackCount = 0;
    static const u32 callStackMax = 128;

    auto data = GetFunctionData(f);
    if(!data) return Variant_Empty;

    if(callStackCount > callStackMax) {
        Error(f, "Infinite recursive call. Refusing to call function %s to prevent stack overflow.", GetUuid(f));

        Variant defaultValue;
        memset(&defaultValue.data, 0, sizeof(defaultValue.data));
        defaultValue.type = data->FunctionReturnType;

        return defaultValue;
    }

    Verbose(Verbose_Function, "Calling function %s ...", GetName(f));

    Variant *finalArguments = (Variant*)alloca(numArguments * sizeof(Variant));

    static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    u32 numArgs = 0;
    auto functionArguments = GetFunctionArguments(f, &numArgs);
    for(auto i = 0; i < numArgs; ++i) {
        if(i >= numArguments) {
            finalArguments[i] = Variant_Empty;
        } else {
            auto type = GetFunctionArgumentType(functionArguments[i]);
            finalArguments[i] = (type == TypeOf_Variant) ? arguments[i] : Cast(arguments[i], type);
        }
    }

    if(numArguments < numArgs) {
        Log(f, LogSeverity_Error, "Insufficient function arguments provided when calling %s", GetDebugName(f));
        return Variant_Empty;
    }

#ifdef PROFILE
    ProfileStart(GetName(f), 100.0);
#endif
    auto caller = (FunctionCallerType)data->FunctionCaller;

    callStackCount++;
    auto result = caller(
            f,
            numArgs,
            finalArguments
    );
    callStackCount--;

#ifdef PROFILE
    ProfileEnd(); // Warning if function call exceeds 100.0ms
#endif

    return result;
}

struct ProfileEntry {
    StringRef tag;
    double threshold;
    struct timespec start;
};

std::vector<ProfileEntry> profileEntries;

API_EXPORT void ProfileStart(StringRef tag, double thresholdMsecs) {
    ProfileEntry ent;
    clock_gettime(CLOCK_MONOTONIC, &ent.start);
    ent.tag = tag;
    ent.threshold = thresholdMsecs;

    profileEntries.push_back(ent);
}

API_EXPORT void ProfileEnd() {
    struct timespec ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_end);

    auto entry = profileEntries[profileEntries.size()-1];
    profileEntries.pop_back();

    double diff = (double)(ts_end.tv_sec - entry.start.tv_sec) * 1000; // ms
    diff += ((double)ts_end.tv_nsec - (double)entry.start.tv_nsec) / 1000000.0;

    if(diff > entry.threshold) {
        Log(0, LogSeverity_Warning, "Function took too long and might cause frame spikes! %s took %.2f milliseconds!", entry.tag, diff);
    }
}

void __InitializeFunction() {
    auto component = ComponentOf_Function();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(Function));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_FunctionReturnType(), offsetof(Function, FunctionReturnType), sizeof(Function::FunctionReturnType), TypeOf_Type,  component, 0, PropertyKind_Value);
    __Property(PropertyOf_FunctionArguments(), InvalidIndex, 0, TypeOf_Entity,  component, ComponentOf_FunctionArgument(), PropertyKind_Array);

    component = ComponentOf_FunctionArgument();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(FunctionArgument));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());
    __Property(PropertyOf_FunctionArgumentType(), offsetof(FunctionArgument, FunctionArgumentType), sizeof(FunctionArgument::FunctionArgumentType), TypeOf_Type,  component, 0, PropertyKind_Value);

    component = ComponentOf_NativeFunction();
    AddComponent(component, ComponentOf_Component());
    SetComponentSize(component, sizeof(NativeFunction));
    SetOwner(component, ModuleOf_Core(), PropertyOf_Components());

    RegisterCommonSignatures();
}

int __ArgStackOffset(int value) {
    int val = 0;
    auto offset = ((char*)&value - (char*)&val);
    return offset;
}

API_EXPORT Type GetFunctionReturnTypeByIndex(u32 functionIndex) {
    auto data = (Function*) GetComponentBytes(ComponentOf_Function(), functionIndex);
    return data->FunctionReturnType;
}

__PropertyCoreImpl(NativePtr, FunctionCaller, Function)

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
EndUnit()
