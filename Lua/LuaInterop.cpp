//
// Created by Kim on 02/06/2018.
//

#include "LuaInterop.h"
#include <lua.hpp>
#include <Core/Type.h>
#include <Core/Entity.h>

static const Type
        u8_type = TypeOf_u8(),
        u16_type = TypeOf_u16(),
        u32_type = TypeOf_u32(),
        u64_type = TypeOf_u64(),
        s8_type = TypeOf_s8(),
        s16_type = TypeOf_s16(),
        s32_type = TypeOf_s32(),
        s64_type = TypeOf_s64(),
        float_type = TypeOf_float(),
        double_type = TypeOf_double(),
        bool_type = TypeOf_bool(),
        StringRef_type = TypeOf_StringRef(),
        Type_type = TypeOf_Type(),
        Property_type = TypeOf_Property(),
        Entity_type = TypeOf_Entity(),
        void_type = TypeOf_void();

#define lua_tovalue_handlecommon(T, HANDLER) if(type == T ## _type) { *(T*)valuePtr = HANDLER(L, idx); return true; } do {}while(false)
#define lua_pushvalue_handlecommon(T, HANDLER) if(type == T ## _type) { HANDLER(L, *(T*)valuePtr); return true; } do {}while(false)

void lua_pushhandle(struct lua_State *L, Handle h) {
    lua_pushnumber(L, *(double*)&h);
}

Handle lua_tohandle(struct lua_State *L, int idx) {
    lua_Number n = lua_tonumber(L, idx);
    return *(Handle*)&n;
}

bool lua_togeneric(lua_State *L, int idx, Type type, void *valuePtr) {
    lua_tovalue_handlecommon(u8, lua_tonumber);
    lua_tovalue_handlecommon(u16, lua_tonumber);
    lua_tovalue_handlecommon(u32, lua_tonumber);
    lua_tovalue_handlecommon(u64, lua_tonumber);
    lua_tovalue_handlecommon(s8, lua_tonumber);
    lua_tovalue_handlecommon(s16, lua_tonumber);
    lua_tovalue_handlecommon(s32, lua_tonumber);
    lua_tovalue_handlecommon(s64, lua_tonumber);
    lua_tovalue_handlecommon(float, lua_tonumber);
    lua_tovalue_handlecommon(double, lua_tonumber);
    lua_tovalue_handlecommon(StringRef, lua_tostring);
    lua_tovalue_handlecommon(bool, lua_toboolean);
    lua_tovalue_handlecommon(Entity, lua_tohandle);
    lua_tovalue_handlecommon(Type, lua_tohandle);
    lua_tovalue_handlecommon(Property, lua_tohandle);

    return false;
}

bool lua_pushgeneric(lua_State *L, Type type, const void *valuePtr) {
    lua_pushvalue_handlecommon(u8, lua_pushnumber);
    lua_pushvalue_handlecommon(u16, lua_pushnumber);
    lua_pushvalue_handlecommon(u32, lua_pushnumber);
    lua_pushvalue_handlecommon(u64, lua_pushnumber);
    lua_pushvalue_handlecommon(s8, lua_pushnumber);
    lua_pushvalue_handlecommon(s16, lua_pushnumber);
    lua_pushvalue_handlecommon(s32, lua_pushnumber);
    lua_pushvalue_handlecommon(s64, lua_pushnumber);
    lua_pushvalue_handlecommon(float, lua_pushnumber);
    lua_pushvalue_handlecommon(double, lua_pushnumber);
    lua_pushvalue_handlecommon(StringRef, lua_pushstring);
    lua_pushvalue_handlecommon(bool, lua_pushboolean);
    lua_pushvalue_handlecommon(Entity, lua_pushhandle);
    lua_pushvalue_handlecommon(Type, lua_pushhandle);
    lua_pushvalue_handlecommon(Property, lua_pushhandle);

    if(type == void_type) {
        lua_pushnil(L);
        return true;
    }

    return false;
}
