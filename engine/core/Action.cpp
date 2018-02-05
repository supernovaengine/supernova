#include "Action.h"

#include "Engine.h"
#include "Object.h"
#include "platform/Log.h"
#include "LuaBind.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

using namespace Supernova;

Action::Action(){
    this->object = NULL;
    this->running = false;
    this->timecount = 0;
    this->steptime = 0;
}

Action::~Action(){
    if (object)
        object->removeAction(this);
}

void Action::luaCallback(int nargs, int nresults, int msgh){
    int status = lua_pcall(LuaBind::getLuaState(), nargs, nresults, msgh);
    if (status != 0){
        Log::Error(LOG_TAG, "Lua Error: %s\n", lua_tostring(LuaBind::getLuaState(),-1));
    }
}

bool Action::isRunning(){
    return running;
}

void Action::onStart(void (*onStartFunc)()){
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
        onStartFunc();
    }
    if (onStartLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onStartLuaFunc);
        luaCallback(0, 0, 0);
    }
}

void Action::onRun(void (*onRunFunc)()){
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
        onRunFunc();
    }
    if (onRunLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onRunLuaFunc);
        luaCallback(0, 0, 0);
    }
}

void Action::onPause(void (*onPauseFunc)()){
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
        onPauseFunc();
    }
    if (onPauseLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onPauseLuaFunc);
        luaCallback(0, 0, 0);
    }
}

void Action::onStop(void (*onStopFunc)()){
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
        onStopFunc();
    }
    if (onStopLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onStopLuaFunc);
        luaCallback(0, 0, 0);
    }
}

void Action::onFinish(void (*onFinishFunc)()){
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
        onFinishFunc();
    }
    if (onFinishLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onFinishLuaFunc);
        luaCallback(0, 0, 0);
    }
}

void Action::onStep(void (*onStepFunc)()){
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
        onStepFunc();
    }
    if (onStepLuaFunc != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, onStepLuaFunc);
        luaCallback(0, 0, 0);
    }
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
