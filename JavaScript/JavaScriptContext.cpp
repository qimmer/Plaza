//
// Created by Kim on 10/07/2018.
//

#include "JavaScriptContext.h"
#include <duktape.h>
#include <string.h>
#include <ctype.h>
#include <Core/Debug.h>
#include <Foundation/Stream.h>
#include <Foundation/PersistancePoint.h>

#include <EASTL/unordered_set.h>
#include <EASTL/string.h>

#define duk_push_entity(CONTEXT, ENTITY) duk_push_number(CONTEXT, *(double*)&ENTITY)

inline Entity duk_to_entity(duk_context *ctx, u8 index) {
    auto number = duk_to_number(ctx, index);
    return *(Entity*)&number;
}

struct JavaScriptContext {
    duk_context *ctx;
};

#define HandleArgument(TYPE, DUKTYPE, DUKTYPELOWERCASE) \
    case TypeOf_ ## TYPE :\
        if(duk_get_type(ctx, i) != DUK_TYPE_ ## DUKTYPE) {\
            Log(0, LogSeverity_Error, "Error in function call arguments. Argument %d in function %s is %s, but %s was expected.", i, GetName(GetComponentEntity(ComponentOf_Function(), functionIndex)), #DUKTYPELOWERCASE, #TYPE);\
            return -1;\
        }\
        *(TYPE*)argumentPtr = duk_to_ ## DUKTYPELOWERCASE (ctx, i);\
        argumentPtrs[i] = argumentPtr;\
        argumentPtr += sizeof(TYPE);\
        break;

#define HandleReturnType(TYPE, DUKTYPE, DUKTYPELOWERCASE) \
    case TypeOf_ ## TYPE :\
        duk_push_ ## DUKTYPELOWERCASE (ctx, *(TYPE*)returnBuffer);\
        return 1;

static duk_ret_t CallFunc(duk_context *ctx) {
    s16 magic = duk_get_current_magic(ctx);
    auto functionIndex = (u32)(*(u16*)&magic);

    char returnBuffer[64];
    char argumentBuffer[256];
    char *argumentPtr = argumentBuffer;

    const void *argumentPtrs[32];
    Type argumentTypes[32];

    auto numJsArguments = duk_get_top(ctx);
    auto numArguments = GetFunctionArguments(functionIndex, 32, argumentTypes);

    if(numJsArguments != numArguments) {
        Log(0, LogSeverity_Error, "Incorrect number of arguments passed to native function %s.", GetName(GetComponentEntity(ComponentOf_Function(), functionIndex)));
        return -1;
    }

    for(auto i = 0; i < numArguments; ++i) {
        switch(argumentTypes[i]) {
            HandleArgument(bool, BOOLEAN, boolean)
            HandleArgument(u8, NUMBER, number)
            HandleArgument(u16, NUMBER, number)
            HandleArgument(u32, NUMBER, number)
            HandleArgument(u64, NUMBER, number)
            HandleArgument(s8, NUMBER, number)
            HandleArgument(s16, NUMBER, number)
            HandleArgument(s32, NUMBER, number)
            HandleArgument(s64, NUMBER, number)
            HandleArgument(float, NUMBER, number)
            HandleArgument(double, NUMBER, number)
            HandleArgument(StringRef, STRING, string)
            HandleArgument(Entity, NUMBER, entity)
            case TypeOf_void:
                duk_push_undefined(ctx);
                return 1;
            default:
                Log(0, LogSeverity_Error, "Unsupported function argument type: %d", GetTypeName(argumentTypes[i]));
                return -1;
        }
    }

    if(!CallFunction(GetComponentEntity(ComponentOf_Function(), functionIndex), returnBuffer, numArguments, argumentTypes, argumentPtrs)) {
        return -1;
    }

    auto returnType = GetFunctionReturnTypeByIndex(functionIndex);
    switch(returnType) {
        HandleReturnType(bool, BOOLEAN, boolean)
        HandleReturnType(u8, NUMBER, number)
        HandleReturnType(u16, NUMBER, number)
        HandleReturnType(u32, NUMBER, number)
        HandleReturnType(u64, NUMBER, number)
        HandleReturnType(s8, NUMBER, number)
        HandleReturnType(s16, NUMBER, number)
        HandleReturnType(s32, NUMBER, number)
        HandleReturnType(s64, NUMBER, number)
        HandleReturnType(float, NUMBER, number)
        HandleReturnType(double, NUMBER, number)
        HandleReturnType(StringRef, STRING, string)
        HandleReturnType(Entity, NUMBER, entity)
        default:
            Log(0, LogSeverity_Error, "Unsupported function return type: %d", GetTypeName(GetFunctionReturnTypeByIndex(functionIndex)));
            return -1;
    }
}

static bool CallJavaScriptFunc(
        u64 functionImplementation,
        Type returnArgumentTypeIndex,
        void *returnData,
        u32 numArguments,
        const Type *argumentTypes,
        const void **argumentDataPtrs
) {

}

static void ToCamelCase(char *line)  {
    bool active = true;

    for(int i = 0; line[i] != '\0'; i++) {
        if(isalpha(line[i])) {
            if(active) {
                line[i] = toupper(line[i]);
                active = false;
            } else {
                line[i] = tolower(line[i]);
            }
        } else if(line[i] == ' ') {
            active = true;
        }
    }
}

static void BindModule(duk_context *ctx, Entity module) {
    // Module object
    duk_push_bare_object(ctx);

    for_children(child, module) {
        if(HasComponent(child, ComponentOf_Function())) {
            auto functionIndex = GetComponentIndex(ComponentOf_Function(), child);
            Assert(child, functionIndex < UINT16_MAX);

            duk_push_c_function(ctx, CallFunc, DUK_VARARGS);
            duk_set_magic(ctx, -1, *(s16*)&functionIndex);
        } else {
            duk_push_entity(ctx, child);
        }

        auto name = GetName(child);
        duk_put_prop_string(ctx, -2, name);
    }

    // Push module object onto the global table
    duk_put_global_string(ctx, GetName(module));
}

LocalFunction(OnJavaScriptContextAdded, void, Entity component, Entity context) {
    auto data = GetJavaScriptContextData(context);

    data->ctx = duk_create_heap_default();

    for_entity(module, moduleData, Module, {
        BindModule(data->ctx, module);
    }
}

LocalFunction(OnJavaScriptContextRemoved, void, Entity component, Entity context) {
    auto data = GetJavaScriptContextData(context);

    duk_destroy_heap(data->ctx);
}

static void RebindModuleChild(Entity module, Entity child, StringRef oldName) {
    for_entity(context, contextData, JavaScriptContext, {
        BindModule(contextData->ctx, module);
    }
}

API_EXPORT bool EvaluateJavaScript(Entity context, StringRef name, StringRef code) {
    auto data = GetJavaScriptContextData(context);

    if (duk_peval_string(data->ctx, code) != 0) {
        Log(context, LogSeverity_Error, "Error: %s (%s)\n", duk_safe_to_string(data->ctx, -1), name);
        duk_pop(data->ctx);  // pop error
        return false;
    } else {
        duk_pop(data->ctx);  // pop result

        duk_push_global_object(data->ctx);

        for_entity(function, functionData, Function, {
            auto caller = GetFunctionCaller(function);

            // If function has no caller or has a JavaScript caller,
            // we should check if this function has been implemented/overridden
            // in the new script we just evaluated.

            if(!caller || caller == CallJavaScriptFunc) {
                auto functionName = GetName(function);
                if(duk_get_prop_string(data->ctx, -1, functionName) == 1) {
                    Verbose("%s implements %s", name, functionName);

                    SetFunctionCaller(function, CallJavaScriptFunc);
                    SetFunctionImplementation(function, (u64)duk_get_heapptr(data->ctx, -1));
                }
                duk_pop(data->ctx);
            }
        }

        duk_pop(data->ctx); // pop global object

        return true;
    }
}


BeginUnit(JavaScriptContext)
    BeginComponent(JavaScriptContext)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnJavaScriptContextAdded, ComponentOf_JavaScriptContext())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnJavaScriptContextRemoved, ComponentOf_JavaScriptContext())

    auto context = NodeOf_JavaScriptMainContext();
    SetName(context, "MainJavaScriptContext");
    AddComponent(context, ComponentOf_JavaScriptContext());
EndUnit()
