#include <Lua/LuaScript.h>
#include <Lua/LuaBinding.h>
#include <Foundation/Stream.h>
#include <Core/String.h>

#include <lua.hpp>

struct LuaScript {
    String LuaScriptErrors;
	Vector<Function> LuaScriptFunctions;
	Vector<Type> LuaScriptComponents;
};

DefineComponent(LuaScript)
	Dependency(Stream)
    DefineProperty(StringRef, LuaScriptErrors)
EndComponent()

DefineComponentPropertyReactive(LuaScript, StringRef, LuaScriptErrors)

static void LoadScript(Entity entity) {
    if(!HasLuaScript(entity)) return;

    if(!StreamOpen(entity, StreamMode_Read)) {
        Log(LogChannel_Core, LogSeverity_Error, "Could not open Lua Script '%s' for reading.", GetEntityPath(entity));
        return;
    }

    StreamSeek(entity, StreamSeek_End);
    auto size = StreamTell(entity);
    StreamSeek(entity, 0);

    Vector<char> buffer(size + 1);
    if(!StreamRead(entity, size, buffer.data()) == size) {
        Log(LogChannel_Core, LogSeverity_Error, "Error reading Lua Script '%s'.", GetEntityPath(entity));
    }
    buffer[size] = '\0'; // Null termination
    StreamClose(entity);

    auto L = GetLuaState();

    lua_pushcfunction(L, Traceback);

    char scriptPath[PATH_MAX];
    snprintf(scriptPath, PATH_MAX, "@%s", GetStreamPath(entity));

    // Load, compile and push global function scope onto the stack
    if(luaL_loadbuffer(L, buffer.data(), buffer.size() - 1, scriptPath)) {
        SetLuaScriptErrors(entity, lua_tostring(L, -1));
        Log(LogChannel_Core, LogSeverity_Error, "%s", lua_tostring(L, -1));
        return;
    }

    // Call the script as if it was a function with no args
    if(lua_pcall(L, 0, 0, lua_gettop(L) - 1)) {
        SetLuaScriptErrors(entity, "Runtime error when running script.");
        return;
    }

    SetLuaScriptErrors(entity, "");
}

DefineService(LuaScript)
	Subscribe(StreamChanged, LoadScript)
    Subscribe(StreamContentChanged, LoadScript)
    Subscribe(LuaScriptAdded, LoadScript)
EndService()
