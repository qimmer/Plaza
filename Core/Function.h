#ifndef PLAZA_FUNCTION_H
#define PLAZA_FUNCTION_H

#include <Core/Handle.h>
#include <Core/Type.h>
#include <Core/Service.h>

DeclareHandle(Function)
DeclareService(FunctionCallService)

Type GetFunctionContextType(Function f);
Type GetFunctionReturnType(Function f);
const char * GetFunctionName(Function f);
void * GetFunctionPtr(Function f);
u32 GetFunctionArguments(Function f);
StringRef GetFunctionArgumentName(Function f, u32 index);
Type GetFunctionArgumentType(Function f, u32 index);

Function FindFunctionByName(StringRef name);

void SetFunctionContextType(Function f, Type type);
void SetFunctionReturnType(Function f, Type type);
void SetFunctionName(Function f, const char * name);
void SetFunctionPtr(Function f, void* ptr);
void SetFunctionArguments(Function f, StringRef arguments);

bool CallFunction(Function f, const void *argumentData, void *returnData);

#define DefineFunction(RETURN_TYPE, FUNCTION_NAME, ...) \
    Function FunctionOf_ ## FUNCTION_NAME () { \
        typedef RETURN_TYPE (*FUNCTION_NAME ## _signature)(__VA_ARGS__);\
        static Function f = 0; \
        if(!f) { \
            f = CreateFunction(); \
            SetFunctionReturnType(f, TypeOf_ ## RETURN_TYPE ()); \
            SetFunctionName(f, #FUNCTION_NAME ); \
            SetFunctionPtr(f, (void*)&FUNCTION_NAME); \
            SetFunctionArguments(f, #__VA_ARGS__); \
        } \
        return f;\
    }

#define DeclareFunction(FUNCTION_NAME) Function FunctionOf_ ## FUNCTION_NAME ();

#define DefineMethod(RETURN_TYPE, FUNCTION_NAME, ...) \
        Function func_ ## FUNCTION_NAME = CreateFunction(); \
        SetFunctionContextType(func_ ## FUNCTION_NAME, type); \
        SetFunctionReturnType(func_ ## FUNCTION_NAME, TypeOf_ ## RETURN_TYPE ()); \
        SetFunctionName(func_ ## FUNCTION_NAME, #FUNCTION_NAME ); \
        SetFunctionPtr(func_ ## FUNCTION_NAME, (void*)&FUNCTION_NAME); \
        SetFunctionArguments(func_ ## FUNCTION_NAME, #__VA_ARGS__);

#define DefineExtensionMethod(COMPONENT_TYPE, RETURN_TYPE, FUNCTION_NAME, ...) \
        Function func_ ## FUNCTION_NAME = CreateFunction(); \
        SetFunctionContextType(func_ ## FUNCTION_NAME, TypeOf_ ## COMPONENT_TYPE ()); \
        SetFunctionReturnType(func_ ## FUNCTION_NAME, TypeOf_ ## RETURN_TYPE ()); \
        SetFunctionName(func_ ## FUNCTION_NAME, #FUNCTION_NAME ); \
        SetFunctionPtr(func_ ## FUNCTION_NAME, (void*)&FUNCTION_NAME); \
        SetFunctionArguments(func_ ## FUNCTION_NAME, #__VA_ARGS__);

#endif