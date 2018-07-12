#ifndef LUABINDING_H
#define LUABINDING_H



struct lua_State* GetLuaState();
int Traceback(struct lua_State *L);

#endif
