
#include "GUIObject.h"

#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

using namespace Supernova;

GUIObject::GUIObject(): Mesh2D(){
}

GUIObject::~GUIObject(){

}

void GUIObject::onPress(void (*onPressFunc)()){
    this->onPressFunc = onPressFunc;
}

int GUIObject::onPress(lua_State *L){

    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onPressLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void GUIObject::call_onPress(){
    if (onPressFunc != NULL){
        onPressFunc();
    }
    if (onPressLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onPressLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}
