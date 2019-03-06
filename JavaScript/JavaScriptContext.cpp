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
#include <Core/Identification.h>

#define duk_push_entity(CONTEXT, ENTITY) duk_push_number(CONTEXT, *(double*)&ENTITY)

#define Verbose_JS "js"

inline Entity duk_to_entity(duk_context *ctx, u8 index) {
    auto number = duk_to_number(ctx, index);
    return *(Entity*)&number;
}

struct JavaScriptContext {
    duk_context *ctx;
};

struct JavaScriptFunction {
    NativePtr JavaScriptFunctionHandle, JavaScriptFunctionContext;
};

struct JavaScript {
};

#define HandleArgument(TYPE, DUKTYPE, DUKTYPELOWERCASE) \
    case TypeOf_ ## TYPE :\
        if(duk_get_type(ctx, i) != DUK_TYPE_ ## DUKTYPE) {\
            Log(0, LogSeverity_Error, "Error in function call arguments. Argument %d in function %s is %s, but %s was expected.", i, GetUuid(GetComponentEntity(ComponentOf_Function(), functionIndex)), #DUKTYPELOWERCASE, #TYPE);\
            return -1;\
        }\
        finalArguments[i].as_ ## TYPE = duk_to_ ## DUKTYPELOWERCASE (ctx, i);\
        break;

#define HandleReturnType(TYPE, DUKTYPE, DUKTYPELOWERCASE) \
    case TypeOf_ ## TYPE :\
        duk_push_ ## DUKTYPELOWERCASE (ctx, returnValue.as_ ## TYPE);\
        break;

static duk_ret_t CallFunc(duk_context *ctx) {
    s16 magic = duk_get_current_magic(ctx);
    auto functionIndex = (u32)(*(u16*)&magic);
    auto function = GetComponentEntity(ComponentOf_Function(), functionIndex);

    auto numJsArguments = duk_get_top(ctx);

    u32 numFunctionArguments = 0;
    auto arguments = GetFunctionArguments(function, &numFunctionArguments);

    if(numJsArguments != numFunctionArguments) {
        Log(0, LogSeverity_Error, "Incorrect number of arguments passed to native function %s.", GetUuid(GetComponentEntity(ComponentOf_Function(), functionIndex)));
        return -1;
    }

    Variant *finalArguments = (Variant*)alloca(numFunctionArguments * sizeof(Variant));

    u32 numArgs = 0;
    auto functionArguments = GetFunctionArguments(function, &numArgs);
    for(auto i = 0; i < numArgs; ++i) {
        if(i >= numFunctionArguments) {
            finalArguments[i] = Variant_Empty;
        } else {
            auto type = GetFunctionArgumentType(functionArguments[i]);
            //finalArguments[i] = (type == TypeOf_Variant) ? arguments[i] : Cast(arguments[i], type);
        }
    }

    for(auto i = 0; i < numFunctionArguments; ++i) {
        auto type = GetFunctionArgumentType(arguments[i]);
        switch(type) {
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
                Log(0, LogSeverity_Error, "Unsupported function argument type: %d", GetTypeName(type));
                return -1;
        }
    }

    auto returnValue = CallFunction(function, numFunctionArguments, finalArguments);

    switch(returnValue.type) {
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
            return 0;
    }

    return 1;
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
    //duk_push_c_function(ctx,);

    duk_get_prototype(ctx, -1);

    for_children(child, Functions, module) {
        auto functionIndex = GetComponentIndex(ComponentOf_Function(), child);
        Assert(child, functionIndex < UINT16_MAX);

        duk_push_c_function(ctx, CallFunc, DUK_VARARGS);
        duk_set_magic(ctx, -1, *(s16*)&functionIndex);

        auto name = GetUuid(child);
        duk_put_prop_string(ctx, -2, name);
    }

    for_children(child2, Components, module) {
        duk_push_entity(ctx, child2);

        auto name = GetUuid(child2);
        duk_put_prop_string(ctx, -2, name);
    }

    for_children(child3, Properties, module) {
        duk_push_entity(ctx, child3);

        auto name = GetUuid(child3);
        duk_put_prop_string(ctx, -2, name);
    }

    duk_pop(ctx); // Pop prototype

    // Push module function onto the global table
    duk_put_global_string(ctx, GetUuid(module));
}

LocalFunction(OnJavaScriptContextAdded, void, Entity component, Entity context) {
    auto data = GetJavaScriptContextData(context);

    data->ctx = duk_create_heap_default();

    for_entity(module, moduleData, Module) {
        BindModule(data->ctx, module);
    }
}

LocalFunction(OnJavaScriptContextRemoved, void, Entity component, Entity context) {
    auto data = GetJavaScriptContextData(context);

    duk_destroy_heap(data->ctx);
}

static void RebindModuleChild(Entity module, Entity child, StringRef oldName) {
    for_entity(context, contextData, JavaScriptContext) {
        BindModule(contextData->ctx, module);
    }
}

static Variant CallJavaScriptFunc(
        Entity function,
        u32 numArguments,
        const Variant *arguments) {
    auto data = GetJavaScriptFunctionData(function);
    //duk_push_heapptr(data->JavaScriptFunctionContext, data->JavaScriptFunctionHandle);
}

static bool EvaluateJavaScript(Entity script) {
    auto data = GetJavaScriptContextData(JavaScriptContextOf_JavaScriptMainContext());

    if(!StreamOpen(script, StreamMode_Read)) {
        return false;
    }

    StreamSeek(script, StreamSeek_End);
    auto scriptLength = StreamTell(script);
    StreamSeek(script, 0);

    auto scriptCode = (char*)malloc(scriptLength + 1);
    StreamRead(script, scriptLength, scriptCode);
    scriptCode[scriptLength + 1] = '\0';
    StreamClose(script);

    if (duk_peval_string(data->ctx, scriptCode) != 0) {
        Log(script, LogSeverity_Error, "Script Evaluation Error: %s\n", duk_safe_to_string(data->ctx, -1));
        duk_pop(data->ctx);  // pop error
        return false;
    } else {
        duk_pop(data->ctx);  // pop result

        duk_push_global_object(data->ctx);

        for_entity(function, functionData, Function) {
            auto caller = GetFunctionCaller(function);

            // If function has no caller or has a JavaScript caller,
            // we should check if this function has been implemented/overridden
            // in the new script we just evaluated.

            if(!caller || caller == CallJavaScriptFunc) {
                auto functionName = GetUuid(function);
                if(duk_get_prop_string(data->ctx, -1, functionName) == 1) {
                    //Verbose(Verbose_JS, "%s implements %s", name, functionName);

                    SetFunctionCaller(function, (NativePtr)CallJavaScriptFunc);
                    SetJavaScriptFunctionHandle(function, duk_get_heapptr(data->ctx, -1));
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

    BeginComponent(JavaScriptFunction)
        RegisterProperty(NativePtr, JavaScriptFunctionHandle)
    EndComponent()

    BeginComponent(JavaScript)
        RegisterBase(Stream)
        RegisterArrayProperty(JavaScriptFunction, JavaScriptFunctions)
    EndComponent()

    RegisterSubscription(EventOf_EntityComponentAdded(), OnJavaScriptContextAdded, ComponentOf_JavaScriptContext())
    RegisterSubscription(EventOf_EntityComponentRemoved(), OnJavaScriptContextRemoved, ComponentOf_JavaScriptContext())

    AddComponent(JavaScriptContextOf_JavaScriptMainContext(), ComponentOf_JavaScriptContext());
EndUnit()
