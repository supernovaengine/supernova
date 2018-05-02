#include "Events.h"
#include "platform/Log.h"
#include "Engine.h"
#include "LuaBind.h"
#include "Input.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

using namespace Supernova;


void (*Events::onDrawFunc)();
int Events::onDrawLuaFunc;

void (*Events::onUpdateFunc)();
int Events::onUpdateLuaFunc;

void (*Events::onTouchStartFunc)(float, float);
int Events::onTouchStartLuaFunc;

void (*Events::onTouchEndFunc)(float, float);
int Events::onTouchEndLuaFunc;

void (*Events::onTouchDragFunc)(float, float);
int Events::onTouchDragLuaFunc;

void (*Events::onMouseDownFunc)(int, float, float);
int Events::onMouseDownLuaFunc;

void (*Events::onMouseUpFunc)(int, float, float);
int Events::onMouseUpLuaFunc;

void (*Events::onMouseDragFunc)(int, float, float);
int Events::onMouseDragLuaFunc;

void (*Events::onMouseMoveFunc)(float, float);
int Events::onMouseMoveLuaFunc;

void (*Events::onKeyDownFunc)(int);
int Events::onKeyDownLuaFunc;

void (*Events::onKeyUpFunc)(int);
int Events::onKeyUpLuaFunc;


Events::Events() {
}

Events::~Events() {
}

void Events::onDraw(void (*onDrawFunc)()){
    Events::onDrawFunc = onDrawFunc;
}

int Events::onDraw(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onDrawLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onDraw(){
    if (onDrawFunc != NULL){
        onDrawFunc();
    }
    if (onDrawLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onDrawLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void Events::onUpdate(void (*onUpdateFunc)()){
    Events::onUpdateFunc = onUpdateFunc;
}

int Events::onUpdate(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onUpdateLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onUpdate(){
    if (onUpdateFunc != NULL){
        onUpdateFunc();
    }
    if (onUpdateLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onUpdateLuaFunc);
        LuaBind::luaCallback(0, 0, 0);
    }
}

void Events::onTouchStart(void (*onTouchStartFunc)(float, float)){
    Events::onTouchStartFunc = onTouchStartFunc;
}

int Events::onTouchStart(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onTouchStartLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onTouchStart(float x, float y){
    if (onTouchStartFunc != NULL){
        onTouchStartFunc(x, y);
    }
    if (onTouchStartLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onTouchStartLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(2, 0, 0);
    }
    Input::addTouchStarted();
    Input::setTouchPosition(x, y);
}

void Events::onTouchEnd(void (*onTouchEndFunc)(float, float)){
    Events::onTouchEndFunc = onTouchEndFunc;
}

int Events::onTouchEnd(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onTouchEndLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onTouchEnd(float x, float y){
    if (onTouchEndFunc != NULL){
        onTouchEndFunc(x, y);
    }
    if (onTouchEndLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onTouchEndLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(2, 0, 0);
    }
    Input::releaseTouchStarted();
    Input::setTouchPosition(x, y);
}

void Events::onTouchDrag(void (*onTouchDragFunc)(float, float)){
    Events::onTouchDragFunc = onTouchDragFunc;
}

int Events::onTouchDrag(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onTouchDragLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onTouchDrag(float x, float y){
    if (onTouchDragFunc != NULL){
        onTouchDragFunc(x, y);
    }
    if (onTouchDragLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onTouchDragLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(2, 0, 0);
    }
    Input::setTouchPosition(x, y);
}

void Events::onMouseDown(void (*onMouseDownFunc)(int, float, float)){
    Events::onMouseDownFunc = onMouseDownFunc;
}

int Events::onMouseDown(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onMouseDownLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onMouseDown(int button, float x, float y){
    if (onMouseDownFunc != NULL){
        onMouseDownFunc(button, x, y);
    }
    if (onMouseDownLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onMouseDownLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), button);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::addMousePressed(button);
    Input::setMousePosition(x, y);
}

void Events::onMouseUp(void (*onMouseUpFunc)(int, float, float)){
    Events::onMouseUpFunc = onMouseUpFunc;
}

int Events::onMouseUp(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onMouseUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onMouseUp(int button, float x, float y){
    if (onMouseUpFunc != NULL){
        onMouseUpFunc(button, x, y);
    }
    if (onMouseUpLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onMouseUpLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), button);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::releaseMousePressed(button);
    Input::setMousePosition(x, y);
}

void Events::onMouseDrag(void (*onMouseDragFunc)(int, float, float)){
    Events::onMouseDragFunc = onMouseDragFunc;
}

int Events::onMouseDrag(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onMouseDragLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onMouseDrag(int button, float x, float y){
    if (onMouseDragFunc != NULL){
        onMouseDragFunc(button, x, y);
    }
    if (onMouseDragLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onMouseDragLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), button);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(3, 0, 0);
    }
    Input::setMousePosition(x, y);
}

void Events::onMouseMove(void (*onMouseMoveFunc)(float, float)){
    Events::onMouseMoveFunc = onMouseMoveFunc;
}

int Events::onMouseMove(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onMouseMoveLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onMouseMove(float x, float y){
    if (onMouseMoveFunc != NULL){
        onMouseMoveFunc(x, y);
    }
    if (onMouseMoveLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onMouseMoveLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), x);
        lua_pushnumber(LuaBind::getLuaState(), y);
        LuaBind::luaCallback(2, 0, 0);
    }
    Input::setMousePosition(x, y);
}

void Events::onKeyDown(void (*onKeyDownFunc)(int)){
    Events::onKeyDownFunc = onKeyDownFunc;
}

int Events::onKeyDown(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onKeyDownLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onKeyDown(int key){
    if (onKeyDownFunc != NULL){
        onKeyDownFunc(key);
    }
    if (onKeyDownLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onKeyDownLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), key);
        LuaBind::luaCallback(1, 0, 0);
    }
    Input::addKeyPressed(key);
}

void Events::onKeyUp(void (*onKeyUpFunc)(int)){
    Events::onKeyUpFunc = onKeyUpFunc;
}

int Events::onKeyUp(lua_State *L){
    
    if (lua_type(L, 2) == LUA_TFUNCTION){
        Events::onKeyUpLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Events::call_onKeyUp(int key){
    if (onKeyUpFunc != NULL){
        onKeyUpFunc(key);
    }
    if (onKeyUpLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, Events::onKeyUpLuaFunc);
        lua_pushnumber(LuaBind::getLuaState(), key);
        LuaBind::luaCallback(1, 0, 0);
    }
    Input::releaseKeyPressed(key);
}
