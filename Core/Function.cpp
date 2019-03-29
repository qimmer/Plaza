//
// Created by Kim Johannsen on 01-03-2018.
//

#include <string.h>

#include <Core/NativeUtils.h>
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

static char nullData[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

API_EXPORT void SetFunctionArgsByDecl(Entity f, StringRef arguments) {
	auto functionData = GetFunction(f);

    auto len = strlen(arguments);
    auto offset = 0;
    auto byteOffset = 0;

    char argumentSplits[PathMax];
    strcpy(argumentSplits, arguments);

    while(offset < len) {
        auto argument = &argumentSplits[0] + offset;
        auto nextComma = strchr(&argumentSplits[0] + offset, ',');
        if(!nextComma) nextComma = &argumentSplits[0] + len;
        auto argumentLength = (u32)(nextComma - argument);
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

        auto argumentEntity = CreateEntity();

        Type type = 0;
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

        SetFunctionArgument(argumentEntity, {type});
        byteOffset += GetTypeSize(type);

        AddFunctionArguments(f, argumentEntity);
    }


}

API_EXPORT Variant CallFunction(
        Entity f,
        u32 numArguments,
        const Variant *arguments
) {
    thread_local u32 callStackCount = 0;
    static const u32 callStackMax = 128;

    auto data = GetFunction(f);
    if(!data.FunctionCaller) return Variant_Empty;

    if(callStackCount > callStackMax) {
        Error(f, "Infinite recursive call. Refusing to call function %s to prevent stack overflow.", GetIdentification(f).Uuid);

        Variant defaultValue;
        memset(&defaultValue.data, 0, sizeof(defaultValue.data));
        defaultValue.type = data.FunctionReturnType;

        return defaultValue;
    }

	Verbose(Verbose_Function, "Calling function %s ...", GetName(f));

	if (numArguments < GetNumFunctionArguments(f)) {
		Log(f, LogSeverity_Error, "Insufficient function arguments provided when calling %s", GetDebugName(f));
		return Variant_Empty;
	}

	/*if (data->NumFunctionArguments != numArguments) {
		Variant *finalArguments = (Variant*)alloca(numArguments * sizeof(Variant));

		u32 numArgs = data->NumFunctionArguments;
		for (auto i = 0; i < numArgs; ++i) {
			if (i >= numArguments) {
				finalArguments[i] = Variant_Empty;
			}
			else {
				auto type = data->FunctionArgumentTypes[i];
				finalArguments[i] = (type == TypeOf_Variant) ? arguments[i] : Cast(arguments[i], type);
			}
		}

		if (numArguments < numArgs) {
			Log(f, LogSeverity_Error, "Insufficient function arguments provided when calling %s", GetDebugName(f));
			return Variant_Empty;
		}

		numArguments = numArgs;
		arguments = finalArguments;
	}*/
    

#ifdef PROFILE
    ProfileStart(GetName(f), 100.0);
#endif
    
    callStackCount++;
    auto result = ((FunctionCallerType)data.FunctionCaller)(
            data.FunctionPtr,
			arguments
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
    /*ProfileEntry ent;
    clock_gettime(CLOCK_MONOTONIC, &ent.start);
    ent.tag = tag;
    ent.threshold = thresholdMsecs;

    profileEntries.push_back(ent);*/
}

API_EXPORT void ProfileEnd() {
    /*struct timespec ts_end;
    clock_gettime(CLOCK_MONOTONIC, &ts_end);

    auto entry = profileEntries[profileEntries.size()-1];
    profileEntries.pop_back();

    double diff = (double)(ts_end.tv_sec - entry.start.tv_sec) * 1000; // ms
    diff += ((double)ts_end.tv_nsec - (double)entry.start.tv_nsec) / 1000000.0;

    if(diff > entry.threshold) {
        Log(0, LogSeverity_Warning, "Function took too long and might cause frame spikes! %s took %.2f milliseconds!", entry.tag, diff);
    }*/
}

void __InitializeFunction() {
    RegisterCommonSignatures();
}

int __ArgStackOffset(int value) {
    int val = 0;
    auto offset = (s32)((char*)&value - (char*)&val);
    return offset;
}

BeginUnit(Function)
    BeginComponent(Function)
        RegisterProperty(Type, FunctionReturnType)
		RegisterProperty(NativePtr, FunctionCaller)
		RegisterProperty(NativePtr, FunctionPtr)
        RegisterArrayProperty(FunctionArgument, FunctionArguments)
    EndComponent()

    BeginComponent(FunctionArgument)
        RegisterProperty(Type, FunctionArgumentType)
    EndComponent()
EndUnit()
