#include "TimeAction.h"

#include "Engine.h"
#include "Object.h"
#include "platform/Log.h"
#include <math.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBind.h"

#include <stdio.h>

using namespace Supernova;


float TimeAction::linear(float time){
    return time;
}

float TimeAction::quadEaseIn(float time){
    return time * time;
}

float TimeAction::quadEaseOut(float time){
    return time * (2 - time);
}

float TimeAction::quadEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time;
    }
    
    time--;
    return - 0.5 * (time * (time - 2) - 1);
}

float TimeAction::cubicEaseIn(float time){
    return time * time * time;
}

float TimeAction::cubicEaseOut(float time){
    time--;
    return time * time * time + 1;
}

float TimeAction::cubicEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time;
    }
    time -= 2;
    return 0.5 * (time * time * time + 2);
}

float TimeAction::quartEaseIn(float time){
    return time * time * time * time;
}

float TimeAction::quartEaseOut(float time){
    time--;
    return 1 - (time * time * time * time);
}

float TimeAction::quartEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time;
    }
    time -= 2;
    return - 0.5 * (time * time * time * time - 2);
}

float TimeAction::quintEaseIn(float time){
    return time * time * time * time * time;
}

float TimeAction::quintEaseOut(float time){
    time--;
    return time * time * time * time * time + 1;
}

float TimeAction::quintEaseInOut(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time * time;
    }
    
    time -= 2;
    return 0.5 * (time * time * time * time * time + 2);
}

float TimeAction::sineEaseIn(float time){
    return 1 - cos(time * M_PI / 2);
}

float TimeAction::sineEaseOut(float time){
    return sin(time * M_PI / 2);
}

float TimeAction::sineEaseInOut(float time){
    return 0.5 * (1 - cos(M_PI * time));
}

float TimeAction::expoEaseIn(float time){
    return time == 0 ? 0 : pow(1024, time - 1);
}

float TimeAction::expoEaseOut(float time){
    return time == 1 ? 1 : 1 - pow(2, - 10 * time);
}

float TimeAction::expoEaseInOut(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    if ((time *= 2) < 1) {
        return 0.5 * pow(1024, time - 1);
    }
    
    return 0.5 * (- pow(2, - 10 * (time - 1)) + 2);
}

float TimeAction::circEaseIn(float time){
    return 1 - sqrt(1 - time * time);
}

float TimeAction::circEaseOut(float time){
    time--;
    return sqrt(1 - (time * time));
}

float TimeAction::circEaseInOut(float time){
    if ((time *= 2) < 1) {
        return - 0.5 * (sqrt(1 - time * time) - 1);
    }
    
    time -= 2;
    return 0.5 * (sqrt(1 - time * time) + 1);
}

float TimeAction::elasticEaseIn(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    return -pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
}

float TimeAction::elasticEaseOut(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    return pow(2, -10 * time) * sin((time - 0.1) * 5 * M_PI) + 1;
}

float TimeAction::elasticEaseInOut(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    time *= 2;
    
    if (time < 1) {
        return -0.5 * pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
    }
    
    return 0.5 * pow(2, -10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI) + 1;
}

float TimeAction::backEaseIn(float time){
    float s = 1.70158;
    
    return time * time * ((s + 1) * time - s);
}

float TimeAction::backEaseOut(float time){
    float s = 1.70158;
    
    time--;
    return time * time * ((s + 1) * time + s) + 1;
}

float TimeAction::backEaseInOut(float time){
    float s = 1.70158 * 1.525;
    
    if ((time *= 2) < 1) {
        return 0.5 * (time * time * ((s + 1) * time - s));
    }
    
    time -= 2;
    return 0.5 * (time * time * ((s + 1) * time + s) + 2);
}

float TimeAction::bounceEaseIn(float time){
    return 1 - bounceEaseOut(1 - time);
}

float TimeAction::bounceEaseOut(float time){
    if (time < (1 / 2.75)) {
        return 7.5625 * time * time;
    } else if (time < (2 / 2.75)) {
        time -= (1.5 / 2.75);
        return 7.5625 * time * time + 0.75;
    } else if (time < (2.5 / 2.75)) {
        time -= (2.25 / 2.75);
        return 7.5625 * time * time + 0.9375;
    } else {
        time -= (2.625 / 2.75);
        return 7.5625 * time * time + 0.984375;
    }
}

float TimeAction::bounceEaseInOut(float time){
    if (time < 0.5) {
        return bounceEaseIn(time * 2) * 0.5;
    }
    
    return bounceEaseOut(time * 2 - 1) * 0.5 + 0.5;
}


TimeAction::TimeAction(): Action(){
    this->function = TimeAction::linear;
    this->functionLua = 0;
    this->duration = 0;
    this->loop = false;
    this->time = 0;
    this->value = 0;
}

TimeAction::TimeAction(float duration, bool loop): Action(){
    this->function = TimeAction::linear;
    this->functionLua = 0;
    this->duration = duration;
    this->loop = loop;
    this->time = 0;
    this->value = 0;
}

TimeAction::TimeAction(float duration, bool loop, float (*function)(float)): Action(){
    this->function = function;
    this->functionLua = 0;
    this->duration = duration;
    this->loop = loop;
    this->time = 0;
    this->value = 0;
}


TimeAction::~TimeAction(){
}

void TimeAction::setFunction(float (*function)(float)){
    this->functionLua = 0;
    this->function = function;
}

int TimeAction::setFunction(lua_State* L){
    this->function = NULL;
    if (lua_type(L, 2) == LUA_TFUNCTION){
        functionLua = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error(LOG_TAG, "Lua Error: is not a function\n");
    }
    return 0;
}

void TimeAction::setFunctionType(int functionType){
    
    functionLua = 0;
    
    if (functionType == S_LINEAR){
        function = TimeAction::linear;
    }else if(functionType == S_QUAD_EASEIN){
        function = TimeAction::quadEaseIn;
    }else if(functionType == S_QUAD_EASEOUT){
        function = TimeAction::quadEaseOut;
    }else if(functionType == S_QUAD_EASEINOUT){
        function = TimeAction::quadEaseInOut;
    }else if(functionType == S_CUBIC_EASEIN){
        function = TimeAction::cubicEaseIn;
    }else if(functionType == S_CUBIC_EASEOUT){
        function = TimeAction::cubicEaseOut;
    }else if(functionType == S_CUBIC_EASEINOUT){
        function = TimeAction::cubicEaseInOut;
    }else if(functionType == S_QUART_EASEIN){
        function = TimeAction::quartEaseIn;
    }else if(functionType == S_QUART_EASEOUT){
        function = TimeAction::quartEaseOut;
    }else if(functionType == S_QUART_EASEINOUT){
        function = TimeAction::quartEaseInOut;
    }else if(functionType == S_QUINT_EASEIN){
        function = TimeAction::quintEaseIn;
    }else if(functionType == S_QUINT_EASEOUT){
        function = TimeAction::quintEaseOut;
    }else if(functionType == S_QUINT_EASEINOUT){
        function = TimeAction::quintEaseInOut;
    }else if(functionType == S_SINE_EASEIN){
        function = TimeAction::sineEaseIn;
    }else if(functionType == S_SINE_EASEOUT){
        function = TimeAction::sineEaseOut;
    }else if(functionType == S_SINE_EASEINOUT){
        function = TimeAction::sineEaseInOut;
    }else if(functionType == S_EXPO_EASEIN){
        function = TimeAction::expoEaseIn;
    }else if(functionType == S_EXPO_EASEOUT){
        function = TimeAction::expoEaseOut;
    }else if(functionType == S_EXPO_EASEINOUT){
        function = TimeAction::expoEaseInOut;
    }else if(functionType == S_ELASTIC_EASEIN){
        function = TimeAction::elasticEaseIn;
    }else if(functionType == S_ELASTIC_EASEOUT){
        function = TimeAction::elasticEaseOut;
    }else if(functionType == S_ELASTIC_EASEINOUT){
        function = TimeAction::elasticEaseInOut;
    }else if(functionType == S_BACK_EASEIN){
        function = TimeAction::backEaseIn;
    }else if(functionType == S_BACK_EASEOUT){
        function = TimeAction::backEaseOut;
    }else if(functionType == S_BACK_EASEINOUT){
        function = TimeAction::backEaseInOut;
    }else if(functionType == S_BOUNCE_EASEIN){
        function = TimeAction::bounceEaseIn;
    }else if(functionType == S_BOUNCE_EASEOUT){
        function = TimeAction::bounceEaseOut;
    }else if(functionType == S_BOUNCE_EASEINOUT){
        function = TimeAction::bounceEaseInOut;
    }
}

bool TimeAction::run(){
    return Action::run();
}

bool TimeAction::pause(){
    return Action::pause();
}

bool TimeAction::stop(){
    if (!Action::stop())
        return false;

    time = 0;
    value = 0;

    return true;
}

bool TimeAction::step(){
    
    if (!Action::step())
        return false;
    
    if ((time == 1) && !loop){
        stop();
        return false;
    }
    
    if (duration >= 0) {
        
        int durationms = (int)(duration * 1000);
        
        if (timecount >= durationms){
            if (!loop){
                timecount = durationms;
            }else{
                timecount -= durationms;
            }
        }
        
        time = (float) timecount / durationms;
    }
    
    
    if (function){
        value = function(time);
    }else if(functionLua != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, functionLua);
        lua_pushnumber(LuaBind::getLuaState(), time);
        int status = lua_pcall(LuaBind::getLuaState(), 1, 1, 0);
        if (status != 0){
            Log::Error(LOG_TAG, "Lua Error: %s\n", lua_tostring(LuaBind::getLuaState(),-1));
        }
        
        if (!lua_isnumber(LuaBind::getLuaState(), -1))
            Log::Error(LOG_TAG, "Lua Error: function in Action must return a number\n");
        value = lua_tonumber(LuaBind::getLuaState(), -1);
        lua_pop(LuaBind::getLuaState(), 1);
    }
    //Log::Debug(LOG_TAG, "step time %f value %f \n", time, value);
    
    return true;
}
