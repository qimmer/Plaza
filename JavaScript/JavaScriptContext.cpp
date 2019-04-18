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

#define HandleArgument(TYPE, DUKTYPE, DUKTYPELOWERCASE) \
    case TypeOf_ ## TYPE :\
        if(duk_get_type(ctx, i) != DUK_TYPE_ ## DUKTYPE) {\
            Log(0, LogSeverity_Error, "Error in function call arguments. Argument %d in function %s is %s, but %s was expected.", i, GetIdentification(GetComponentEntity(ComponentOf_Function(), functionIndex)).Uuid, #DUKTYPELOWERCASE, #TYPE);\
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

    auto functionData = GetFunction(function);

    if(numJsArguments != functionData.FunctionArguments.GetSize()) {
        Log(0, LogSeverity_Error, "Incorrect number of arguments passed to native function %s.", GetIdentification(GetComponentEntity(ComponentOf_Function(), functionIndex)).Uuid);
        return -1;
    }

    Variant *finalArguments = (Variant*)alloca(functionData.FunctionArguments.GetSize() * sizeof(Variant));

    for(auto i = 0; i < functionData.FunctionArguments.GetSize(); ++i) {
        if(i >= numJsArguments) {
            finalArguments[i] = Variant_Empty;
        } else {
            auto type = GetFunctionArgument(functionData.FunctionArguments[i]).FunctionArgumentType;
            //finalArguments[i] = (type == TypeOf_Variant) ? arguments[i] : Cast(arguments[i], type);
        }
    }

    for(auto i = 0; i < functionData.FunctionArguments.GetSize(); ++i) {
        auto type = GetFunctionArgument(functionData.FunctionArguments[i]).FunctionArgumentType;
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

    auto returnValue = CallFunction(function, functionData.FunctionArguments.GetSize(), finalArguments);

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

    auto moduleData = GetModule(module);

    for(auto child : moduleData.Functions) {
        auto functionIndex = GetComponentIndex(ComponentOf_Function(), child);
        Assert(child, functionIndex < UINT16_MAX);

        duk_push_c_function(ctx, CallFunc, DUK_VARARGS);
        duk_set_magic(ctx, -1, *(s16*)&functionIndex);

        auto name = GetIdentification(child).Uuid;
        duk_put_prop_string(ctx, -2, name);
    }

    for(auto child2 : moduleData.Components) {
        auto componentData = GetComponent(child2);

        duk_push_entity(ctx, child2);

        auto name = GetIdentification(child2).Uuid;
        duk_put_prop_string(ctx, -2, name);

        for(auto child3 : componentData.Properties) {
            duk_push_entity(ctx, child3);

            auto name = GetIdentification(child3).Uuid;
            duk_put_prop_string(ctx, -2, name);
        }
    }

    duk_pop(ctx); // Pop prototype

    // Push module function onto the global table
    duk_put_global_string(ctx, GetIdentification(module).Uuid);
}

static void OnContextChanged(Entity entity, const JavaScriptContext& oldData, const JavaScriptContext& newData) {
    if(!oldData.JavaScriptContextHandle && !newData.JavaScriptContextHandle) {
        // Added
        auto data = newData;

        data.JavaScriptContextHandle = duk_create_heap_default();
        SetJavaScriptContext(entity, data);

        Module moduleData;
        for_entity_data(module, ComponentOf_Module(), &moduleData) {
            BindModule((duk_context*)data.JavaScriptContextHandle, module);
        }
    }

    if(oldData.JavaScriptContextHandle && !newData.JavaScriptContextHandle) {
        // Removed or reset
        duk_destroy_heap((duk_context*)oldData.JavaScriptContextHandle);
    }
}

static void RebindModuleChild(Entity module, Entity child, StringRef oldName) {
    JavaScriptContext contextData;
    for_entity_data(context, ComponentOf_JavaScriptContext(), &contextData) {
        BindModule((duk_context*)contextData.JavaScriptContextHandle, module);
    }
}

static Variant CallJavaScriptFunc(
        Entity function,
        u32 numArguments,
        const Variant *arguments) {
    auto data = GetJavaScriptFunction(function);
    //duk_push_heapptr(data.JavaScriptFunctionContext, data.JavaScriptFunctionHandle);
}

static bool EvaluateJavaScript(Entity script) {
    auto data = GetJavaScriptContext(JavaScriptContextOf_JavaScriptMainContext());

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

    if (duk_peval_string((duk_context*)data.JavaScriptContextHandle, scriptCode) != 0) {
        Log(script, LogSeverity_Error, "Script Evaluation Error: %s\n", duk_safe_to_string((duk_context*)data.JavaScriptContextHandle, -1));
        duk_pop((duk_context*)data.JavaScriptContextHandle);  // pop error
        return false;
    } else {
        duk_pop((duk_context*)data.JavaScriptContextHandle);  // pop result

        duk_push_global_object((duk_context*)data.JavaScriptContextHandle);

        Function functionData;
        for_entity_data(function, ComponentOf_Function(), &functionData) {
            // If function has no caller or has a JavaScript caller,
            // we should check if this function has been implemented/overridden
            // in the new script we just evaluated.

            if(!functionData.FunctionCaller || functionData.FunctionCaller == CallJavaScriptFunc) {
                auto functionName = GetIdentification(function).Uuid;
                if(duk_get_prop_string((duk_context*)data.JavaScriptContextHandle, -1, functionName) == 1) {
                    //Verbose(Verbose_JS, "%s implements %s", name, functionName);

                    functionData.FunctionCaller = (NativePtr)CallJavaScriptFunc;
                    SetFunction(function, functionData);
                    SetJavaScriptFunction(function, {duk_get_heapptr((duk_context*)data.JavaScriptContextHandle, -1), (duk_context*)data.JavaScriptContextHandle});
                }
                duk_pop((duk_context*)data.JavaScriptContextHandle);
            }
        }

        duk_pop((duk_context*)data.JavaScriptContextHandle); // pop global object

        return true;
    }
}

BeginUnit(JavaScriptContext)
    BeginComponent(JavaScriptContext)
        RegisterProperty(NativePtr, JavaScriptContextHandle)
    EndComponent()

    BeginComponent(JavaScriptFunction)
        RegisterProperty(NativePtr, JavaScriptFunctionHandle)
    EndComponent()

    BeginComponent(JavaScript)
        RegisterBase(Stream)
        RegisterArrayProperty(JavaScriptFunction, JavaScriptFunctions)
    EndComponent()

    RegisterSystem(OnContextChanged, ComponentOf_JavaScriptContext())

    AddComponent(JavaScriptContextOf_JavaScriptMainContext(), ComponentOf_JavaScriptContext());
EndUnit()
