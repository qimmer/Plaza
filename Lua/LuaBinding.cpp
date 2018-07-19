#include <Lua/LuaBinding.h>
#include <Lua/LuaScript.h>
#include <Lua/LuaInterop.h>

#include <Core/Module.h>
#include <Core/Function.h>

#include <lua.hpp>

#define MODULES_GLOBAL "__modules"

static lua_State *L = NULL;

static int l_get (lua_State *L);
static int l_set (lua_State *L);
static int l_call (lua_State *L);

static void GetCurrentSource(lua_State *L, char *buffer, u32 size) {
	lua_getglobal(L, "debug");
	Assert(lua_istable(L, -1));
	lua_pushstring(L, "getinfo");
	lua_gettable(L, -2);
	Assert(lua_isfunction(L, -1));

	lua_pushinteger(L, 2);
	lua_pushstring(L, "Sl");

	Assert(lua_pcall(L, 2, 1, 0) == 0);

	Assert(lua_istable(L, -1));

	lua_pushstring(L, "source");
	lua_gettable(L, -2);

	snprintf(buffer, size, "%s : %d", lua_tostring(L, -2), lua_tointeger(L, -1));

	lua_pushstring(L, "line");
	lua_gettable(L, -3);

	lua_pop(L, 4);
}


int Traceback(lua_State *L) {
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	lua_getfield(L, -1, "traceback");
	lua_pushvalue(L, 1);
	lua_pushinteger(L, 2);
	lua_call(L, 2, 1);
	Log(LogChannel_Core, LogSeverity_Error, "%s", lua_tostring(L, -1));
	return 1;
}

static bool AddLuaComponent(Entity entity, Type type) {
	auto result = true;

	lua_getglobal(L, MODULES_GLOBAL);

	auto module = GetTypeModule(type);
	lua_pushstring(L, GetModuleName(module));
	lua_gettable(L, -2);

	lua_pushfstring(L, "%sData", GetTypeName(type));
	lua_gettable(L, -2);

	lua_pushhandle(L, entity);
	lua_gettable(L, -2);

	if(!lua_isnil(L, -1)) {
		result = false;
	}

	if(result) {
		lua_pushhandle(L, entity);
		lua_newtable(L);
		lua_settable(L, -3);
	}

	lua_pop(L, 4);

	return result;
}

static bool RemoveLuaComponent(Entity entity, Type type) {
    auto result = true;

    lua_getglobal(L, MODULES_GLOBAL);

    auto module = GetTypeModule(type);
    lua_pushstring(L, GetModuleName(module));
    lua_gettable(L, -2);

    lua_pushfstring(L, "%sData", GetTypeName(type));
    lua_gettable(L, -2);

    lua_pushhandle(L, entity);
    lua_gettable(L, -2);

    if(lua_isnil(L, -1)) {
        result = false;
    }

    if(result) {
        lua_pushhandle(L, entity);
        lua_pushnil(L);
        lua_settable(L, -3);
    }

    lua_pop(L, 4);

    return result;
}

static bool HasLuaComponent(Entity entity, Type type) {
    auto result = true;

    lua_getglobal(L, MODULES_GLOBAL);

    auto module = GetTypeModule(type);
    lua_pushstring(L, GetModuleName(module));
    lua_gettable(L, -2);

    lua_pushfstring(L, "%sData", GetTypeName(type));
    lua_gettable(L, -2);

    lua_pushhandle(L, entity);
    lua_gettable(L, -2);

    if(lua_isnil(L, -1)) {
        result = false;
    }

    lua_pop(L, 4);

    return result;
}

static void BindModule(lua_State *L, Module module) {
	char buffer[128];

	lua_getglobal(L, MODULES_GLOBAL); // Push module index table "__modules"
	lua_newtable(L);
	lua_pushstring(L, GetModuleName(module));
	lua_pushvalue(L, -2); // Use the previously created table as value

	// Set the table value of moduleName on the modules table to the new table and pop key and value
	// off the stack, leaving us with the new module table
	lua_settable(L, -4);

	for(auto type = GetNextType(0); IsTypeValid(type); type = GetNextType(type)) {
		if(GetTypeModule(type) != module) continue;

		lua_pushstring(L, GetTypeName(type));
		lua_pushhandle(L, type);
		lua_settable(L, -3);
	}

	for(auto property = GetNextProperty(0); IsPropertyValid(property); property = GetNextProperty(property)) {
		if(GetTypeModule(GetPropertyType(property)) != module) continue;

		// Add Property Handle
		lua_pushstring(L, GetPropertyName(property));
		lua_pushhandle(L, property);
		lua_settable(L, -3);

		// Add Property Getter
		snprintf(buffer, 128, "Get%s", GetPropertyName(property));
		lua_pushstring(L, buffer);
		lua_pushhandle(L, property);
		lua_pushcclosure(L, l_get, 1);
		lua_settable(L, -3);

		// Add Property Setter
		snprintf(buffer, 128, "Set%s", GetPropertyName(property));
		lua_pushstring(L, buffer);
		lua_pushhandle(L, property);
		lua_pushcclosure(L, l_set, 1);
		lua_settable(L, -3);
	}

	for(auto func = GetNextFunction(0); IsPropertyValid(func); func = GetNextFunction(func)) {
		if(GetFunctionModule(func) != module) continue;

		lua_pushstring(L, buffer);
		lua_pushhandle(L, func);
		lua_pushcclosure(L, l_call, 1);
		lua_settable(L, -3);
	}

	// Set handle field on module table
	lua_pushstring(L, "ModuleHandle");
	lua_pushhandle(L, module);
	lua_settable(L, -3);

	lua_pop(L, 2); // Pop module table and modules index table
}

static int l_module(lua_State *L) {
	char currentSrc[PathMax];
	GetCurrentSource(L, currentSrc, PathMax);

	auto moduleName = lua_tostring(L, -1);

	Module nativeModule;
	for(nativeModule = GetNextModule(0); IsModuleValid(nativeModule); nativeModule = GetNextModule(nativeModule)) {
		if(strcmp(GetModuleName(nativeModule), moduleName) == 0) break;
	}

	if(!IsModuleValid(nativeModule)) {
		nativeModule = CreateModule();
		SetModuleName(nativeModule, moduleName);
		SetModuleSourcePath(nativeModule, currentSrc);
	}

	BindModule(L, nativeModule);

	// Return module table
	lua_getglobal(L, MODULES_GLOBAL);
	lua_pushstring(L, GetModuleName(nativeModule));
	lua_gettable(L, -2);
	lua_remove(L, -2);

	return 1;
}

static int l_set (lua_State *L) {
	auto property = (Property)lua_tohandle(L, lua_upvalueindex(1));
	auto propertyType = GetPropertyType(property);
	auto entity = (Property)lua_tohandle(L, 1);

	char value[128];
	Assert(GetTypeSize(propertyType) <= 128);

	lua_togeneric(L, 2, propertyType, value);

	GetPropertyGenericSetter(property)(entity, value);

	return 0;
}

static int l_get (lua_State *L) {
	auto property = (Property)lua_tohandle(L, lua_upvalueindex(1));
	auto propertyType = GetPropertyType(property);
	auto entity = (Property)lua_tohandle(L, 1);

	char value[128];
	Assert(GetTypeSize(propertyType) <= 128);

	GetPropertyGenericGetter(property)(entity, value);

	lua_pushgeneric(L, propertyType, value);

	return 1;
}

static int l_call(lua_State *L) {
	auto func = (Function)lua_tohandle(L, lua_upvalueindex(1));

	char returnData[64];
	char argumentData[1024];
	char *argumentPtr = argumentData;
	char *argumentPtrMax = argumentData + 1024;

	for(auto i = 0; i < GetFunctionArguments(func); ++i) {
		auto argumentType = GetFunctionArgumentType(func, i);

		lua_togeneric(L, i + 1, argumentType, argumentPtr);
		argumentPtr += GetTypeSize(argumentType);
	}

	Assert(argumentPtr <= argumentPtrMax);

	CallFunction(func, argumentData, returnData);

	lua_pushgeneric(L, GetFunctionReturnType(func), returnData);

	return 1;
}

static int l_require(lua_State *L) {
	auto moduleName = lua_tostring(L, 1);

	lua_getglobal(L, MODULES_GLOBAL); // Push module index table "__modules"
	lua_pushstring(L, moduleName);
	lua_gettable(L, -2); // Push module table with all module information

	// If it doesn't already exist in lua modules table but exists as a native module, create the module table!
	if(lua_isnil(L, -1)) {
		Log(LogChannel_Core, LogSeverity_Info, "Module not found in module index: %s", moduleName);
		lua_pop(L, 1); // Pop nil from stack
		return 0;
	}

	return 1;
}

static void *l_alloc (void *ud, void *ptr, size_t osize, size_t nsize) {
	(void)ud;  (void)osize;  /* not used */
	if (nsize == 0) {
		free(ptr);
		return NULL;
	}
	else
		return realloc(ptr, nsize);
}

static int l_component(lua_State *L) {
	auto module = lua_tohandle(L, -2);
	auto componentName = lua_tostring(L, -1);

	auto type = CreateType();
	SetTypeModule(type, module);
	SetTypeName(type, componentName);

	RegisterComponent(type, AddLuaComponent, RemoveLuaComponent, HasLuaComponent);

	lua_getglobal(L, MODULES_GLOBAL);

	lua_pushstring(L, GetModuleName(module));
	lua_gettable(L, -2);

	lua_pushstring(L, GetTypeName(type));
	lua_pushhandle(L, type);
	lua_settable(L, -3);

	lua_pushfstring(L, "%sData", GetTypeName(type));
	lua_newtable(L);
	lua_settable(L, -3);

	lua_pop(L, 2);

	return 0;
}

static int l_property(lua_State *L) {
    auto componentType = lua_tohandle(L, -3);
    auto propertyName = lua_tostring(L, -2);
    auto propertyType = lua_tohandle(L, -1);

    auto property = CreateProperty();
    SetPropertyComponent(property, componentType);
    SetPropertyName(property, propertyName);
    SetPropertyType(property, propertyType);

    lua_getglobal(L, MODULES_GLOBAL);

    lua_pushstring(L, GetModuleName(GetTypeModule(componentType)));
    lua_gettable(L, -2);

    lua_pushstring(L, propertyName);
    lua_pushhandle(L, property);
    lua_settable(L, -3);

    lua_pushfstring(L, propertyName);

    lua_pop(L, 2);
}

static int l_extend(lua_State *L) {
	if(!lua_istable(L, -2)) {
		Traceback(L);
		Log(LogChannel_Core, LogSeverity_Error, "Invalid module table.");
		return 0;
	}

	if(!lua_istable(L, -1)) {
		Traceback(L);
		Log(LogChannel_Core, LogSeverity_Error, "Invalid module descriptor table.");
		return 0;
	}

	lua_pushstring(L, "ModuleHandle");
	lua_gettable(L, -3);

	auto module = (Module)lua_tohandle(L, -1);
	lua_pop(L, 1);

	if(!IsModuleValid(module)) {
		Log(LogChannel_Core, LogSeverity_Error, "Invalid Module");
		return 0;
	}

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		auto elementType = lua_tostring(L, -2);

		if(strcmp(elementType, "Components") == 0) {
			lua_pushnil(L);
			while (lua_next(L, -2) != 0) {


				lua_pop(L, 1); // Pop inner value
			}
		}

		lua_pop(L, 1); // Pop outer value
	}


	return 0;
}

API_EXPORT bool CallLuaFunction(void *funcPtr, u8 numArguments, Type *argumentTypes, const void *argumentData, Type returnType, void *returnData) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, (s32)(size_t)funcPtr); // Push stored function ref to stack

	const char *argumentPtr = (const char*)argumentData;

	// Push arguments onto stack
	for(u8 i = 0; i < numArguments; ++i) {
		lua_pushgeneric(L, argumentTypes[i], argumentPtr);
		argumentPtr += GetTypeSize(argumentTypes[i]);
	}
	
	if(lua_pcall(L, numArguments, 1, 0)) {
		Log(LogChannel_Core, LogSeverity_Error, "%s", lua_tostring(L, -1));
		return false;
	}
	
	// Convert result and write to returnData
	bool result = lua_togeneric(L, -1, returnType, returnData);
	lua_pop(L, 1);

	return result;
}

LocalFunction(OnStarted, void, Service service) {
	Assert(sizeof(lua_Number) >= 8); // Make sure a number is a double or larger to contain 64-bit handles
	L = luaL_newstate();
    luaL_openlibs(L);

    lua_newtable(L);
    lua_setglobal(L, MODULES_GLOBAL);

    lua_pushcclosure(L, l_require, 0);
    lua_setglobal(L, "require");

	lua_pushcclosure(L, l_module, 0);
	lua_setglobal(L, "module");

	lua_pushcclosure(L, l_extend, 0);
	lua_setglobal(L, "extend");

	lua_pushcclosure(L, l_component, 0);
	lua_setglobal(L, "component");

    lua_pushcclosure(L, l_property, 0);
    lua_setglobal(L, "property");

    for(auto module = GetNextModule(0); IsModuleValid(module); module = GetNextModule(module)) {
    	BindModule(L, module);
    }
}

LocalFunction(OnStopped, void, Service service) {
	lua_close(L);
}

lua_State* GetLuaState() {
	return L;
}

DefineService(LuaBinding)
	RegisterSubscription(LuaBindingStarted, OnStarted, 0)
	RegisterSubscription(LuaBindingStopped, OnStopped, 0)
EndService()
