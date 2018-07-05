#ifndef LUASCRIPT_H
#define LUASCRIPT_H

#include <Core/Entity.h>

DeclareComponent(LuaScript)
DeclareComponentPropertyReactive(LuaScript, StringRef, LuaScriptErrors)

DeclareService(LuaScript)

#endif