//
// Created by Kim Johannsen on 01-03-2018.
//

#include <dyncall/dyncall.h>
#include <dyncall/dyncall_signature.h>
#include <dyncall/dyncall_struct.h>

#include <Core/Function.h>
#include <Core/String.h>
#include <Core/Vector.h>
#include <Core/Pool.h>
#include <Core/Type.h>

DefineService(FunctionCallService)
EndService()

static DCCallVM *CallVM = 0;
static DCstruct *Struct_v2i, *Struct_v3i, *Struct_v4i, *Struct_v2f, *Struct_v3f, *Struct_v4f, *Struct_m3x3f, *Struct_m4x4f;

#define ALIGNMENT_SSE 16

struct Argument {
    String Name;
    Type ArgumentType;
};

struct FunctionData {
    String Name;
    Type ReturnType, ContextType;
    void *Ptr;
    Vector<Argument> Arguments;
};

DefineHandle(Function, FunctionData)

Type GetFunctionContextType(Function f) {
    return FunctionAt(f)->ContextType;
}

Type GetFunctionReturnType(Function f) {
    return FunctionAt(f)->ReturnType;
}

const char *GetFunctionName(Function f) {
    return FunctionAt(f)->Name.c_str();
}

void *GetFunctionPtr(Function f) {
    return FunctionAt(f)->Ptr;
}

u32 GetFunctionArguments(Function f) {
    return FunctionAt(f)->Arguments.size();
}

StringRef GetFunctionArgumentName(Function f, u32 index) {
    return FunctionAt(f)->Arguments[index].Name.c_str();
}

Type GetFunctionArgumentType(Function f, u32 index) {
    return FunctionAt(f)->Arguments[index].ArgumentType;
}

Function FindFunctionByName(StringRef name) {
    return 0;
}

void SetFunctionContextType(Function f, Type type) {
    FunctionAt(f)->ContextType = type;
}

void SetFunctionReturnType(Function f, Type type) {
    FunctionAt(f)->ReturnType = type;
}

void SetFunctionName(Function f, const char *name) {
    FunctionAt(f)->Name = name;
}

void SetFunctionPtr(Function f, void *ptr) {
    FunctionAt(f)->Ptr = ptr;
}

void SetFunctionArguments(Function f, StringRef arguments) {
    auto data = FunctionAt(f);
    auto len = strlen(arguments);
    auto offset = 0;

    char argumentSplits[PATH_MAX];
    strcpy(argumentSplits, arguments);

    data->Arguments.clear();
    while(offset < len) {
        auto argument = &argumentSplits[0] + offset;
        auto nextComma = strchr(&argumentSplits[0] + offset, ',');
        if(!nextComma) nextComma = &argumentSplits[0] + len;
        auto argumentLength = nextComma - argument;
        offset += argumentLength + 1;
        *nextComma = '\0';

        auto name = strrchr(argument, ' ');
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

        data->Arguments.push_back({name, FindTypeByName(typeSignature)});
    }
}

#define _CallFunction_HandleType(TYPE, DCTYPE) \
    if(arg.ArgumentType == TypeOf_ ## TYPE()) {\
        dcArg ## DCTYPE(CallVM, *((TYPE*)argumentData));\
    } else

#define _CallFunction_HandlePointerType(TYPE) \
    if(arg.ArgumentType == TypeOf_ ## TYPE()) {\
        auto dta = (void**)argumentData;\
        dcArgPointer(CallVM, *dta);\
    } else

#define _CallFunction_HandleVecType(TYPE) \
    if(arg.ArgumentType == TypeOf_ ## TYPE()) {\
        dcArgStruct(CallVM, Struct_ ## TYPE, const_cast<void*>(argumentData));\
    } else


#define _CallFunction_HandleCall(TYPE, DCTYPE) \
    if(GetFunctionReturnType(f) == TypeOf_ ## TYPE()) {\
        *((TYPE*)returnData) = dcCall ## DCTYPE(CallVM, GetFunctionPtr(f));\
    } else

#define _CallFunction_HandlePointerCall(TYPE) \
    if(GetFunctionReturnType(f) == TypeOf_ ## TYPE()) {\
        *((TYPE*)returnData) = (TYPE)dcCallPointer(CallVM, GetFunctionPtr(f));\
    } else

#define _CallFunction_HandleVecCall(TYPE) \
    if(GetFunctionReturnType(f) == TypeOf_ ## TYPE()) {\
        dcCallStruct(CallVM, GetFunctionPtr(f), Struct_ ## TYPE, returnData);\
    } else

bool CallFunction(Function f, const void *argumentData, void *returnData) {
    dcReset(CallVM);

    auto data = FunctionAt(f);

    for(auto& arg : data->Arguments) {
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

        _CallFunction_HandleVecType(v2i)
        _CallFunction_HandleVecType(v3i)
        _CallFunction_HandleVecType(v4i)
        _CallFunction_HandleVecType(v2f)
        _CallFunction_HandleVecType(v3f)
        _CallFunction_HandleVecType(v4f)
        _CallFunction_HandleVecType(m3x3f)
        _CallFunction_HandleVecType(m4x4f)

        if(GetTypeSize(arg.ArgumentType) == sizeof(Handle)) {
            dcArgLongLong(CallVM, *((Handle *) argumentData));
        } else {
            Log(LogChannel_Core, LogSeverity_Error, "Error calling function '%s'. Argument type '%s' is unsupported.", GetFunctionName(f), GetTypeName(arg.ArgumentType));
            return false;
        }
    }

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

    _CallFunction_HandleVecCall(v2i)
    _CallFunction_HandleVecCall(v3i)
    _CallFunction_HandleVecCall(v4i)
    _CallFunction_HandleVecCall(v2f)
    _CallFunction_HandleVecCall(v3f)
    _CallFunction_HandleVecCall(v4f)
    _CallFunction_HandleVecCall(m3x3f)
    _CallFunction_HandleVecCall(m4x4f)

    if(GetTypeSize(GetFunctionReturnType(f)) == sizeof(Handle)) {
        *((Handle*)returnData) = dcCallLongLong(CallVM, GetFunctionPtr(f));
    }
    else if(GetFunctionReturnType(f) ==TypeOf_void()) {
        dcCallVoid(CallVM, GetFunctionPtr(f));
    } else {
        Log(LogChannel_Core, LogSeverity_Error, "Error calling function '%s'. Return type '%s' is unsupported.", GetFunctionName(f), GetTypeName(GetFunctionReturnType(f)));
        return false;
    }

    return true;
}

static bool ServiceStart() {
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

    return true;
}

static bool ServiceStop() {
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
    return true;
}