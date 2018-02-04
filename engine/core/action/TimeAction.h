#ifndef TimeAction_h
#define TimeAction_h

#include "Action.h"

#define S_LINEAR 0
#define S_EASE_QUAD_IN 1
#define S_EASE_QUAD_OUT 2
#define S_EASE_QUAD_IN_OUT 3
#define S_EASE_CUBIC_IN 4
#define S_EASE_CUBIC_OUT 5
#define S_EASE_CUBIC_IN_OUT 6
#define S_EASE_QUART_IN 7
#define S_EASE_QUART_OUT 8
#define S_EASE_QUART_IN_OUT 9
#define S_EASE_QUINT_IN 10
#define S_EASE_QUINT_OUT 11
#define S_EASE_QUINT_IN_OUT 12
#define S_EASE_SINE_IN 13
#define S_EASE_SINE_OUT 14
#define S_EASE_SINE_IN_OUT 15
#define S_EASE_EXPO_IN 16
#define S_EASE_EXPO_OUT 17
#define S_EASE_EXPO_IN_OUT 18
#define S_EASE_CIRC_IN 19
#define S_EASE_CIRC_OUT 20
#define S_EASE_CIRC_IN_OUT 21
#define S_EASE_ELASTIC_IN 22
#define S_EASE_ELASTIC_OUT 23
#define S_EASE_ELASTIC_IN_OUT 24
#define S_EASE_BACK_IN 25
#define S_EASE_BACK_OUT 26
#define S_EASE_BACK_IN_OUT 27
#define S_EASE_BOUNCE_IN 28
#define S_EASE_BOUNCE_OUT 29
#define S_EASE_BOUNCE_IN_OUT 30

typedef struct lua_State lua_State;

namespace Supernova{
    
    class Object;
    
    class TimeAction: public Action{
        
    protected:
        
        float duration;
        bool loop;
        
        float (*function)(float);
        int functionLua;
        
        float time;
        float value;
        
    public:
        TimeAction();
        TimeAction(float duration, bool loop);
        TimeAction(float duration, bool loop, float (*function)(float));
        virtual ~TimeAction();
        
        static float linear(float time);
        static float easeInQuad(float time);
        static float easeOutQuad(float time);
        static float easeInOutQuad(float time);
        static float easeInCubic(float time);
        static float easeOutCubic(float time);
        static float easeInOutCubic(float time);
        static float easeInQuart(float time);
        static float easeOutQuart(float time);
        static float easeInOutQuart(float time);
        static float easeInQuint(float time);
        static float easeOutQuint(float time);
        static float easeInOutQuint(float time);
        static float easeInSine(float time);
        static float easeOutSine(float time);
        static float easeInOutSine(float time);
        static float easeInExpo(float time);
        static float easeOutExpo(float time);
        static float easeInOutExpo(float time);
        static float easeInCirc(float time);
        static float easeOutCirc(float time);
        static float easeInOutCirc(float time);
        static float easeInElastic(float time);
        static float easeOutElastic(float time);
        static float easeInOutElastic(float time);
        static float easeInBack(float time);
        static float easeOutBack(float time);
        static float easeInOutBack(float time);
        static float easeInBounce(float time);
        static float easeOutBounce(float time);
        static float easeInOutBounce(float time);
        
        void setFunction(float (*function)(float));
        int setFunction(lua_State* L);
        
        void setFunctionType(int functionType);

        float getDuration();
        void setDuration(float duration);

        bool isLoop();
        void setLoop(bool loop);
        
        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        
        virtual bool step();
    };
}

#endif /* TimeAction_h */
