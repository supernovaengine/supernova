
#include "GUIObject.h"

#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

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
        //TODO: return error in Lua
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
        //TODO: return error in Lua
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
    if (x >= getPosition().x and
        x <= (getPosition().x + getWidth()) and
        y >= getPosition().y and
        y <= (getPosition().y + getHeight())) {
        return true;
    }
    return false;
}

void GUIObject::engine_onDown(float x, float y){
}

void GUIObject::engine_onUp(float x, float y){
}

void GUIObject::engine_onTextInput(std::string text){
}
