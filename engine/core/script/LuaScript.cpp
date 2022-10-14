//
// (c) 2020 Eduardo Doria.
//

#include "LuaScript.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge.h"

#include "LuaBinding.h"

using namespace Supernova;

void LuaScript::setObject(std::string global, Object* object){
    lua_State *L = LuaBinding::getLuaState();

    if (!luabridge::push<Object*>(L, object))
        throw luabridge::makeErrorCode(luabridge::ErrorCode::LuaStackOverflow);

    lua_setglobal(L, global.c_str());
}

Object* LuaScript::getObject(std::string global){
    lua_State *L = LuaBinding::getLuaState();

    lua_getglobal(L, global.c_str());

    auto result = luabridge::get<Object*>(L, -1);
    if (! result)
        throw result.error();

    return result.value();
}

