//
// (c) 2020 Eduardo Doria.
//

#include "LuaScript.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "sol.hpp"
#include "LuaBinding.h"

using namespace Supernova;

void LuaScript::setObject(const char* global, Object* object){
    lua_State *L = LuaBinding::getLuaState();

    sol::state_view lua(L);

    lua[global] = object;
}

Object* LuaScript::getObject(const char* global){
    lua_State *L = LuaBinding::getLuaState();

    sol::state_view lua(L);

    return lua[global];
}

