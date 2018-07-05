#ifndef LUABINDING_H
#define LUABINDING_H

#include <Core/Service.h>

DeclareService(LuaBinding)

struct lua_State* GetLuaState();
int Traceback(struct lua_State *L);

#endif
