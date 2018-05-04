#include "Ease.h"

#include "Log.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBind.h"

using namespace Supernova;

Ease::Ease(){
    this->function = Ease::linear;
    this->functionLua = 0;
}

Ease::~Ease(){

}

float Ease::linear(float time){
    return time;
}

float Ease::easeInQuad(float time){
    return time * time;
}

float Ease::easeOutQuad(float time){
    return time * (2 - time);
}

float Ease::easeInOutQuad(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time;
    }

    time--;
    return - 0.5 * (time * (time - 2) - 1);
}

float Ease::easeInCubic(float time){
    return time * time * time;
}

float Ease::easeOutCubic(float time){
    time--;
    return time * time * time + 1;
}

float Ease::easeInOutCubic(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time;
    }
    time -= 2;
    return 0.5 * (time * time * time + 2);
}

float Ease::easeInQuart(float time){
    return time * time * time * time;
}

float Ease::easeOutQuart(float time){
    time--;
    return 1 - (time * time * time * time);
}

float Ease::easeInOutQuart(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time;
    }
    time -= 2;
    return - 0.5 * (time * time * time * time - 2);
}

float Ease::easeInQuint(float time){
    return time * time * time * time * time;
}

float Ease::easeOutQuint(float time){
    time--;
    return time * time * time * time * time + 1;
}

float Ease::easeInOutQuint(float time){
    if ((time *= 2) < 1) {
        return 0.5 * time * time * time * time * time;
    }

    time -= 2;
    return 0.5 * (time * time * time * time * time + 2);
}

float Ease::easeInSine(float time){
    return 1 - cos(time * M_PI / 2);
}

float Ease::easeOutSine(float time){
    return sin(time * M_PI / 2);
}

float Ease::easeInOutSine(float time){
    return 0.5 * (1 - cos(M_PI * time));
}

float Ease::easeInExpo(float time){
    return time == 0 ? 0 : pow(1024, time - 1);
}

float Ease::easeOutExpo(float time){
    return time == 1 ? 1 : 1 - pow(2, - 10 * time);
}

float Ease::easeInOutExpo(float time){
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

float Ease::easeInCirc(float time){
    return 1 - sqrt(1 - time * time);
}

float Ease::easeOutCirc(float time){
    time--;
    return sqrt(1 - (time * time));
}

float Ease::easeInOutCirc(float time){
    if ((time *= 2) < 1) {
        return - 0.5 * (sqrt(1 - time * time) - 1);
    }

    time -= 2;
    return 0.5 * (sqrt(1 - time * time) + 1);
}

float Ease::easeInElastic(float time){
    if (time == 0) {
        return 0;
    }

    if (time == 1) {
        return 1;
    }

    return -pow(2, 10 * (time - 1)) * sin((time - 1.1) * 5 * M_PI);
}

float Ease::easeOutElastic(float time){
    if (time == 0) {
        return 0;
    }

    if (time == 1) {
        return 1;
    }

    return pow(2, -10 * time) * sin((time - 0.1) * 5 * M_PI) + 1;
}

float Ease::easeInOutElastic(float time){
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

float Ease::easeInBack(float time){
    float s = 1.70158;

    return time * time * ((s + 1) * time - s);
}

float Ease::easeOutBack(float time){
    float s = 1.70158;

    time--;
    return time * time * ((s + 1) * time + s) + 1;
}

float Ease::easeInOutBack(float time){
    float s = 1.70158 * 1.525;

    if ((time *= 2) < 1) {
        return 0.5 * (time * time * ((s + 1) * time - s));
    }

    time -= 2;
    return 0.5 * (time * time * ((s + 1) * time + s) + 2);
}

float Ease::easeInBounce(float time){
    return 1 - easeOutBounce(1 - time);
}

float Ease::easeOutBounce(float time){
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

float Ease::easeInOutBounce(float time){
    if (time < 0.5) {
        return easeInBounce(time * 2) * 0.5;
    }

    return easeOutBounce(time * 2 - 1) * 0.5 + 0.5;
}

void Ease::setFunction(float (*function)(float)){
    this->functionLua = 0;
    this->function = function;
}

int Ease::setFunction(lua_State* L){
    this->function = NULL;
    if (lua_type(L, 2) == LUA_TFUNCTION){
        functionLua = luaL_ref(L, LUA_REGISTRYINDEX);
    }else{
        Log::Error("Lua Error: is not a function\n");
    }
    return 0;
}

void Ease::setFunctionType(int functionType){

    functionLua = 0;

    if (functionType == S_LINEAR){
        function = Ease::linear;
    }else if(functionType == S_EASE_QUAD_IN){
        function = Ease::easeInQuad;
    }else if(functionType == S_EASE_QUAD_OUT){
        function = Ease::easeOutQuad;
    }else if(functionType == S_EASE_QUAD_IN_OUT){
        function = Ease::easeInOutQuad;
    }else if(functionType == S_EASE_CUBIC_IN){
        function = Ease::easeInCubic;
    }else if(functionType == S_EASE_CUBIC_OUT){
        function = Ease::easeOutCubic;
    }else if(functionType == S_EASE_CUBIC_IN_OUT){
        function = Ease::easeInOutCubic;
    }else if(functionType == S_EASE_QUART_IN){
        function = Ease::easeInQuart;
    }else if(functionType == S_EASE_QUART_OUT){
        function = Ease::easeOutQuart;
    }else if(functionType == S_EASE_QUART_IN_OUT){
        function = Ease::easeInOutQuart;
    }else if(functionType == S_EASE_QUINT_IN){
        function = Ease::easeInQuint;
    }else if(functionType == S_EASE_QUINT_OUT){
        function = Ease::easeOutQuint;
    }else if(functionType == S_EASE_QUINT_IN_OUT){
        function = Ease::easeInOutQuint;
    }else if(functionType == S_EASE_SINE_IN){
        function = Ease::easeInSine;
    }else if(functionType == S_EASE_SINE_OUT){
        function = Ease::easeOutSine;
    }else if(functionType == S_EASE_SINE_IN_OUT){
        function = Ease::easeInOutSine;
    }else if(functionType == S_EASE_EXPO_IN){
        function = Ease::easeInExpo;
    }else if(functionType == S_EASE_EXPO_OUT){
        function = Ease::easeOutExpo;
    }else if(functionType == S_EASE_EXPO_IN_OUT){
        function = Ease::easeInOutExpo;
    }else if(functionType == S_EASE_CIRC_IN){
        function = Ease::easeInCirc;
    }else if(functionType == S_EASE_CIRC_OUT){
        function = Ease::easeOutCirc;
    }else if(functionType == S_EASE_CIRC_IN_OUT){
        function = Ease::easeInOutCirc;
    }else if(functionType == S_EASE_ELASTIC_IN){
        function = Ease::easeInElastic;
    }else if(functionType == S_EASE_ELASTIC_OUT){
        function = Ease::easeOutElastic;
    }else if(functionType == S_EASE_ELASTIC_IN_OUT){
        function = Ease::easeInOutElastic;
    }else if(functionType == S_EASE_BACK_IN){
        function = Ease::easeInBack;
    }else if(functionType == S_EASE_BACK_OUT){
        function = Ease::easeOutBack;
    }else if(functionType == S_EASE_BACK_IN_OUT){
        function = Ease::easeInOutBack;
    }else if(functionType == S_EASE_BOUNCE_IN){
        function = Ease::easeInBounce;
    }else if(functionType == S_EASE_BOUNCE_OUT){
        function = Ease::easeOutBounce;
    }else if(functionType == S_EASE_BOUNCE_IN_OUT){
        function = Ease::easeInOutBounce;
    }
}

float Ease::call_Function(float time){
    if (function){
        return function(time);
    }else if(functionLua != 0){
        lua_rawgeti(LuaBind::getLuaState(), LUA_REGISTRYINDEX, functionLua);
        lua_pushnumber(LuaBind::getLuaState(), time);
        int status = lua_pcall(LuaBind::getLuaState(), 1, 1, 0);
        if (status != 0){
            Log::Error("Lua Error: %s\n", lua_tostring(LuaBind::getLuaState(),-1));
        }

        if (!lua_isnumber(LuaBind::getLuaState(), -1))
            Log::Error("Lua Error: function in Action must return a number\n");
        float value = lua_tonumber(LuaBind::getLuaState(), -1);
        lua_pop(LuaBind::getLuaState(), 1);
        return value;
    }

    return 0;
}