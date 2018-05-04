#include "Action.h"

#include "Engine.h"
#include "Object.h"
#include "Log.h"
#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaIntf.h"

using namespace Supernova;

Action::Action(){
    this->object = NULL;
    this->running = false;
    this->timecount = 0;
    this->steptime = 0;

    onStartFunc = NULL;
    onStartLuaFunc = 0;

    onRunFunc = NULL;
    onRunLuaFunc = 0;

    onPauseFunc = NULL;
    onPauseLuaFunc = 0;

    onStopFunc = NULL;
    onStopLuaFunc = 0;

    onFinishFunc = NULL;
    onFinishLuaFunc = 0;

    onStepFunc = NULL;
    onStepLuaFunc = 0;
}

Action::~Action(){
    if (object)
        object->removeAction(this);
}

void Action::onStart(void (*onStartFunc)(Object*)){
    this->onStartFunc = onStartFunc;
}

int Action::onStart(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onStartLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Action::call_onStart(){
    if (onStartFunc != NULL){
        onStartFunc(object);
    }
    if (onStartLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onStartLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), object);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void Action::onRun(void (*onRunFunc)(Object*)){
    this->onRunFunc = onRunFunc;
}

int Action::onRun(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onRunLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Action::call_onRun(){
    if (onRunFunc != NULL){
        onRunFunc(object);
    }
    if (onRunLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onRunLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), object);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void Action::onPause(void (*onPauseFunc)(Object*)){
    this->onPauseFunc = onPauseFunc;
}

int Action::onPause(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onPauseLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Action::call_onPause(){
    if (onPauseFunc != NULL){
        onPauseFunc(object);
    }
    if (onPauseLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onPauseLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), object);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void Action::onStop(void (*onStopFunc)(Object*)){
    this->onStopFunc = onStopFunc;
}

int Action::onStop(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onStopLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Action::call_onStop(){
    if (onStopFunc != NULL){
        onStopFunc(object);
    }
    if (onStopLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onStopLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), object);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void Action::onFinish(void (*onFinishFunc)(Object*)){
    this->onFinishFunc = onFinishFunc;
}

int Action::onFinish(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onFinishLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Action::call_onFinish(){
    if (onFinishFunc != NULL){
        onFinishFunc(object);
    }
    if (onFinishLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onFinishLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), object);
        LuaBind::luaCallback(1, 0, 0);
    }
}

void Action::onStep(void (*onStepFunc)(Object*)){
    this->onStepFunc = onStepFunc;
}

int Action::onStep(lua_State *L){
    if (lua_type(L, 2) == LUA_TFUNCTION){
        this->onStepLuaFunc = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        //TODO: return error in Lua
    }
    return 0;
}

void Action::call_onStep(){
    if (onStepFunc != NULL){
        onStepFunc(object);
    }
    if (onStepLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onStepLuaFunc);
        LuaIntf::Lua::push(LuaBind::getLuaState(), object);
        LuaBind::luaCallback(1, 0, 0);
    }
}

Object* Action::getObject(){
    return object;
}

bool Action::isRunning(){
    return running;
}

bool Action::run(){
    running = true;
    if (timecount == 0)
        call_onStart();
    call_onRun();

    return true;
}

bool Action::pause(){
    running = false;
    call_onPause();
    
    return true;
}

bool Action::stop(){
    running = false;
    timecount = 0;
    call_onStop();
    
    return true;
}

bool Action::step(){
    if (running){
        steptime = Engine::getDeltatime();
        timecount += steptime;
        call_onStep();
    }else{
        return false;
    }
    
    return true;
}
