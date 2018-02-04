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

float TimeAction::easeInQuad(float time){
    return time * time;
}

float TimeAction::easeOutQuad(float time){
    return time * (2 - time);
}

float TimeAction::easeInOutQuad(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time;
    }
    
    time--;
    return - 0.5 * (time * (time - 2) - 1);
}

float TimeAction::easeInCubic(float time){
    return time * time * time;
}

float TimeAction::easeOutCubic(float time){
    time--;
    return time * time * time + 1;
}

float TimeAction::easeInOutCubic(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time;
    }
    time -= 2;
    return 0.5 * (time * time * time + 2);
}

float TimeAction::easeInQuart(float time){
    return time * time * time * time;
}

float TimeAction::easeOutQuart(float time){
    time--;
    return 1 - (time * time * time * time);
}

float TimeAction::easeInOutQuart(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time;
    }
    time -= 2;
    return - 0.5 * (time * time * time * time - 2);
}

float TimeAction::easeInQuint(float time){
    return time * time * time * time * time;
}

float TimeAction::easeOutQuint(float time){
    time--;
    return time * time * time * time * time + 1;
}

float TimeAction::easeInOutQuint(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time * time;
    }
    
    time -= 2;
    return 0.5 * (time * time * time * time * time + 2);
}

float TimeAction::easeInSine(float time){
    return 1 - cos(time * M_PI / 2);
}

float TimeAction::easeOutSine(float time){
    return sin(time * M_PI / 2);
}

float TimeAction::easeInOutSine(float time){
    return 0.5 * (1 - cos(M_PI * time));
}

float TimeAction::easeInExpo(float time){
    return time == 0 ? 0 : pow(1024, time - 1);
}

float TimeAction::easeOutExpo(float time){
    return time == 1 ? 1 : 1 - pow(2, - 10 * time);
}

float TimeAction::easeInOutExpo(float time){
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

float TimeAction::easeInCirc(float time){
    return 1 - sqrt(1 - time * time);
}

float TimeAction::easeOutCirc(float time){
    time--;
    return sqrt(1 - (time * time));
}

float TimeAction::easeInOutCirc(float time){
    if ((time *= 2) < 1) {
        return - 0.5 * (sqrt(1 - time * time) - 1);
    }
    
    time -= 2;
    return 0.5 * (sqrt(1 - time * time) + 1);
}

float TimeAction::easeInElastic(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    return -pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
}

float TimeAction::easeOutElastic(float time){
    if (time == 0) {
        return 0;
    }
    
    if (time == 1) {
        return 1;
    }
    
    return pow(2, -10 * time) * sin((time - 0.1) * 5 * M_PI) + 1;
}

float TimeAction::easeInOutElastic(float time){
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

float TimeAction::easeInBack(float time){
    float s = 1.70158;
    
    return time * time * ((s + 1) * time - s);
}

float TimeAction::easeOutBack(float time){
    float s = 1.70158;
    
    time--;
    return time * time * ((s + 1) * time + s) + 1;
}

float TimeAction::easeInOutBack(float time){
    float s = 1.70158 * 1.525;
    
    if ((time *= 2) < 1) {
        return 0.5 * (time * time * ((s + 1) * time - s));
    }
    
    time -= 2;
    return 0.5 * (time * time * ((s + 1) * time + s) + 2);
}

float TimeAction::easeInBounce(float time){
    return 1 - easeOutBounce(1 - time);
}

float TimeAction::easeOutBounce(float time){
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

float TimeAction::easeInOutBounce(float time){
    if (time < 0.5) {
        return easeInBounce(time * 2) * 0.5;
    }
    
    return easeOutBounce(time * 2 - 1) * 0.5 + 0.5;
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

float TimeAction::getDuration(){
    return duration;
};

void TimeAction::setDuration(float duration){
     this->duration = duration;
}

bool TimeAction::isLoop(){
    return loop;
}

void TimeAction::setLoop(bool loop){
    this->loop = loop;
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
    }else if(functionType == S_EASE_QUAD_IN){
        function = TimeAction::easeInQuad;
    }else if(functionType == S_EASE_QUAD_OUT){
        function = TimeAction::easeOutQuad;
    }else if(functionType == S_EASE_QUAD_IN_OUT){
        function = TimeAction::easeInOutQuad;
    }else if(functionType == S_EASE_CUBIC_IN){
        function = TimeAction::easeInCubic;
    }else if(functionType == S_EASE_CUBIC_OUT){
        function = TimeAction::easeOutCubic;
    }else if(functionType == S_EASE_CUBIC_IN_OUT){
        function = TimeAction::easeInOutCubic;
    }else if(functionType == S_EASE_QUART_IN){
        function = TimeAction::easeInQuart;
    }else if(functionType == S_EASE_QUART_OUT){
        function = TimeAction::easeOutQuart;
    }else if(functionType == S_EASE_QUART_IN_OUT){
        function = TimeAction::easeInOutQuart;
    }else if(functionType == S_EASE_QUINT_IN){
        function = TimeAction::easeInQuint;
    }else if(functionType == S_EASE_QUINT_OUT){
        function = TimeAction::easeOutQuint;
    }else if(functionType == S_EASE_QUINT_IN_OUT){
        function = TimeAction::easeInOutQuint;
    }else if(functionType == S_EASE_SINE_IN){
        function = TimeAction::easeInSine;
    }else if(functionType == S_EASE_SINE_OUT){
        function = TimeAction::easeOutSine;
    }else if(functionType == S_EASE_SINE_IN_OUT){
        function = TimeAction::easeInOutSine;
    }else if(functionType == S_EASE_EXPO_IN){
        function = TimeAction::easeInExpo;
    }else if(functionType == S_EASE_EXPO_OUT){
        function = TimeAction::easeOutExpo;
    }else if(functionType == S_EASE_EXPO_IN_OUT){
        function = TimeAction::easeInOutExpo;
    }else if(functionType == S_EASE_CIRC_IN){
        function = TimeAction::easeInCirc;
    }else if(functionType == S_EASE_CIRC_OUT){
        function = TimeAction::easeOutCirc;
    }else if(functionType == S_EASE_CIRC_IN_OUT){
        function = TimeAction::easeInOutCirc;
    }else if(functionType == S_EASE_ELASTIC_IN){
        function = TimeAction::easeInElastic;
    }else if(functionType == S_EASE_ELASTIC_OUT){
        function = TimeAction::easeOutElastic;
    }else if(functionType == S_EASE_ELASTIC_IN_OUT){
        function = TimeAction::easeInOutElastic;
    }else if(functionType == S_EASE_BACK_IN){
        function = TimeAction::easeInBack;
    }else if(functionType == S_EASE_BACK_OUT){
        function = TimeAction::easeOutBack;
    }else if(functionType == S_EASE_BACK_IN_OUT){
        function = TimeAction::easeInOutBack;
    }else if(functionType == S_EASE_BOUNCE_IN){
        function = TimeAction::easeInBounce;
    }else if(functionType == S_EASE_BOUNCE_OUT){
        function = TimeAction::easeOutBounce;
    }else if(functionType == S_EASE_BOUNCE_IN_OUT){
        function = TimeAction::easeInOutBounce;
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
