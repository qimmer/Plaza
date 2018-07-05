//
// Created by Kim on 02/06/2018.
//

#ifndef PLAZA_LUAINTEROP_H
#define PLAZA_LUAINTEROP_H

#include <Core/Handle.h>

void lua_pushhandle(struct lua_State *L, Handle h);
Handle lua_tohandle(struct lua_State *L, int idx);
bool lua_togeneric(struct lua_State *L, int idx, Type type, void *valuePtr);
bool lua_pushgeneric(struct lua_State *L, Type type, const void *valuePtr);

#endif //PLAZA_LUAINTEROP_H
