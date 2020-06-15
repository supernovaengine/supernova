//
// (c) 2020 Eduardo Doria.
//

#include "LuaScript.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaIntf/LuaIntf.h"
#include "LuaBinding.h"

using namespace Supernova;

void LuaScript::setObject(const char* global, Object* object){
    lua_State *L = LuaBinding::getLuaState();
    LuaIntf::Lua::setGlobal(L, global, object);
}

Object* LuaScript::getObject(const char* global){
    lua_State *L = LuaBinding::getLuaState();

    Object* object = NULL;

    lua_getglobal(L, global);
    if (lua_isuserdata(L, -1))
        object = LuaIntf::CppObject::get<Object>(L, -1, false);

    return object;
}

