#include "PhysicsWorld.h"

#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaIntf.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

PhysicsWorld::PhysicsWorld(){
    onBeginContactFunc = NULL;
    onBeginContactLuaFunc = 0;

    onEndContactFunc = NULL;
    onEndContactLuaFunc = 0;
}

void PhysicsWorld::call_onBeginContact(CollisionShape* shapeA, CollisionShape* shapeB){
    if (onBeginContactFunc != NULL){
        onBeginContactFunc(shapeA, shapeB);
    }
    if (onBeginContactLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onBeginContactLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), shapeA);
        LuaIntf::Lua::push(LuaBind::getLuaState(), shapeB);
        LuaBind::luaCallback(2, 0, 0);
    }
}

void PhysicsWorld::call_onEndContact(CollisionShape* shapeA, CollisionShape* shapeB){
    if (onEndContactFunc != NULL){
        onEndContactFunc(shapeA, shapeB);
    }
    if (onEndContactLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onEndContactLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), shapeA);
        LuaIntf::Lua::push(LuaBind::getLuaState(), shapeB);
        LuaBind::luaCallback(2, 0, 0);
    }
}

void PhysicsWorld::onBeginContact(void (*onBeginContactFunc)(CollisionShape*, CollisionShape*)){
    this->onBeginContactFunc = onBeginContactFunc;
}

int PhysicsWorld::onBeginContact(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        onBeginContactLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void PhysicsWorld::onEndContact(void (*onEndContactFunc)(CollisionShape*, CollisionShape*)){
    this->onEndContactFunc = onEndContactFunc;
}

int PhysicsWorld::onEndContact(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        onEndContactLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}