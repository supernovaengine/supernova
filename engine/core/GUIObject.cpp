
#include "GUIObject.h"

#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "Log.h"

#include <cmath>

using namespace Supernova;

GUIObject::GUIObject(): Mesh2D(){
    state = 0;
}

GUIObject::~GUIObject(){

}

int GUIObject::getState(){
    return state;
}

void GUIObject::onDown(void (*onDownFunc)()){
    this->onDownFunc = onDownFunc;
}

int GUIObject::onDown(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onDownLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting GUI onDown is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void GUIObject::call_onDown(){
    if (onDownFunc != NULL){
        onDownFunc();
    }
    if (onDownLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onDownLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void GUIObject::onUp(void (*onUpFunc)()){
    this->onUpFunc = onUpFunc;
}

int GUIObject::onUp(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Error setting GUI onUp is not lua function");
        return luaL_error(L, "This is not a valid function");
    }
    return 0;
}

void GUIObject::call_onUp(){
    if (onUpFunc != NULL){
        onUpFunc();
    }
    if (onUpLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onUpLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

bool GUIObject::isCoordInside(float x, float y){
    Vector3 point = worldRotation.getRotationMatrix() * Vector3(x, y, 0);

    if (point.x >= (getWorldPosition().x - getCenter().x) and
        point.x <= (getWorldPosition().x - getCenter().x + abs(getWidth() * getWorldScale().x)) and
        point.y >= (getWorldPosition().y - getCenter().y) and
        point.y <= (getWorldPosition().y - getCenter().y + abs(getHeight() * getWorldScale().y))) {
        return true;
    }
    return false;
}

void GUIObject::engine_onDown(int pointer, float x, float y){
}

void GUIObject::engine_onUp(int pointer, float x, float y){
}

void GUIObject::engine_onTextInput(std::string text){
}
