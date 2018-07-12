//
// Created by Kim on 10/07/2018.
//

#include "JavaScriptContext.h"
#include <duktape.h>
#include <string.h>
#include <ctype.h>
#include <Core/Debug.h>


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

static void BindProperty(duk_context *ctx, Entity property) {

}

static void BindComponent(duk_context *ctx, Entity component) {
    // Make name camel case
    char nameCamelCase[32];
    strncpy(nameCamelCase, GetName(component), 32);
    ToCamelCase(nameCamelCase);

    duk_push_entity(ctx, component);
    duk_put_prop_string(ctx, 0, nameCamelCase);
}

static void BindFunction(duk_context *ctx, Entity function) {
    Assert(function, HasComponent(function, ComponentOf_Function()));

    auto functionIndex = GetComponentIndex(function, ComponentOf_Function());
    Assert(function, functionIndex < UINT16_MAX);

    // Make name camel case
    char nameCamelCase[32];
    strncpy(nameCamelCase, GetName(function), 32);
    ToCamelCase(nameCamelCase);

    duk_push_c_function(ctx, CallFunc, DUK_VARARGS);
    duk_set_magic(ctx, 0, *(s16*)&functionIndex);
    duk_put_prop_string(ctx, 0, nameCamelCase);
}

static void BindEvent(duk_context *ctx, Entity event) {

}

static void BindModule(duk_context *ctx, Entity module) {
    duk_push_bare_object(ctx);

    for_children(child, module) {
        if(HasComponent(child, ComponentOf_Component())) {
            BindComponent(ctx, child);
        }
    }

    // Make name camel case
    char nameCamelCase[32];
    strncpy(nameCamelCase, GetName(module), 32);
    ToCamelCase(nameCamelCase);

    // Push module object onto the global table
    duk_put_global_string(ctx, nameCamelCase);
}

LocalFunction(OnJavaScriptContextAdded, void, Entity context) {
    auto data = GetJavaScriptContextData(context);

    data->ctx = duk_create_heap_default();
}

LocalFunction(OnJavaScriptContextRemoved, void, Entity context) {
    auto data = GetJavaScriptContextData(context);

    duk_destroy_heap(data->ctx);
}

BeginUnit(JavaScriptContext)
    BeginComponent(JavaScriptContext)
    EndComponent()

    RegisterSubscription(JavaScriptContextAdded, OnJavaScriptContextAdded, 0)
    RegisterSubscription(JavaScriptContextRemoved, OnJavaScriptContextRemoved, 0)

    auto context = NodeOf_JavaScriptMainContext();
    AddComponent(context, ComponentOf_JavaScriptContext());
EndUnit()