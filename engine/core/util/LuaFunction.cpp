#include "LuaFunction.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaIntf.h"

#include "LuaBind.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

LuaFunction::LuaFunction(){
    this->function = 0;
}

LuaFunction::LuaFunction(const LuaFunction& t){
    this->function = t.function;
}

LuaFunction& LuaFunction::operator = (const LuaFunction& t){
    this->function = t.function;

    return *this;
}

int LuaFunction::set(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        function = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting not a Lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void LuaFunction::reset(){
    function = 0;
}

void LuaFunction::call() {
    if (function != 0) {
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void LuaFunction::call(int p1){
    if (function != 0) {
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        lua_pushnumber(LuaBind::getLuaState(), p1);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void LuaFunction::call(int p1, int p2){
    if (function != 0) {
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        lua_pushnumber(LuaBind::getLuaState(), p1);
        lua_pushnumber(LuaBind::getLuaState(), p2);
        LuaBind::luaCallback(2, 0, 0);
    }
}

void LuaFunction::call(float p1){
    if (function != 0) {
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        lua_pushnumber(LuaBind::getLuaState(), p1);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void LuaFunction::call(float p1, float p2){
    if (function != 0) {
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        lua_pushnumber(LuaBind::getLuaState(), p1);
        lua_pushnumber(LuaBind::getLuaState(), p2);
        LuaBind::luaCallback(2, 0, 0);
    }
}

void LuaFunction::call(int p1, float p2, float p3){
    if (function != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        lua_pushnumber(LuaBind::getLuaState(), p1);
        lua_pushnumber(LuaBind::getLuaState(), p2);
        lua_pushnumber(LuaBind::getLuaState(), p3);
        LuaBind::luaCallback(3, 0, 0);
    }
}

void LuaFunction::call(Object* p1){
    if (function != 0) {
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        LuaIntf::Lua::push(LuaBind::getLuaState(), p1);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void LuaFunction::call(std::string p1){
    if (function != 0) {
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, function);
        LuaIntf::Lua::push(LuaBind::getLuaState(), p1);
        LuaBind::luaCallback(1, 0, 0);
    }
}