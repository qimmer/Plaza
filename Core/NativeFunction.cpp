//
// Created by Kim on 17-10-2018.
//

#include "NativeFunction.h"
#include "Debug.h"

#include <EASTL/unordered_map.h>
#include <EASTL/string.h>

API_EXPORT Variant CallNativeFunction(
        Entity function,
        u32 numArguments,
        const Variant *arguments
) {
    auto data = GetNativeFunctionData(function);

    if(!data->NativeFunctionInvoker) return Variant_Empty;

    auto invoker = (NativeFunctionInvokerType)data->NativeFunctionInvoker;
    return invoker(data->NativeFunctionPointer, arguments);
}

static eastl::unordered_map<eastl::string, NativePtr> functionSignatureInvokers;

// Removes argument names from argument lists to only conserve argument types in the comma separated argument list
static void SanitizeSignature(StringRef signature, char *buffer) {
    while(*signature) {
        auto wordStart = signature;
        while(!isalnum(*wordStart) && *wordStart) wordStart++;

        while(isalnum(*wordStart)) {
            *buffer = *wordStart;
            wordStart++;
            buffer++;
        }

        wordStart = strchr(wordStart, ',');
        if(wordStart) {
            signature = wordStart + 1;
            while(*signature && !isalnum(*signature)) {
                signature++;
            }

            if(!*signature) break;

            *buffer = ',';
            ++buffer;
        } else {
            break;
        }
    }

    *buffer = '\0';
}

API_EXPORT void RegisterFunctionSignatureImpl(NativePtr invokerFuncPtr, StringRef signature) {
    char buffer[1024];
    SanitizeSignature(signature, buffer);

    functionSignatureInvokers[buffer] = invokerFuncPtr;
}

API_EXPORT void SetNativeFunctionInvokerBySignature(Entity function, StringRef signature) {
    char buffer[1024];
    SanitizeSignature(signature, buffer);

    auto it = functionSignatureInvokers.find(buffer);
    if(it == functionSignatureInvokers.end()) {
        Log(function, LogSeverity_Error, "Function signature not registered: %s", buffer);
        return;
    }

    SetNativeFunctionInvoker(function, it->second);
}

__PropertyCoreImpl(NativePtr, NativeFunctionPointer, NativeFunction)
__PropertyCoreImpl(NativePtr, NativeFunctionInvoker, NativeFunction)

BeginUnit(NativeFunction)
    BeginComponent(NativeFunction)
        RegisterBase(Function)
        RegisterProperty(NativePtr, NativeFunctionPointer)
        RegisterProperty(NativePtr, NativeFunctionInvoker)
    EndComponent()
EndUnit()
