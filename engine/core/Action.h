#ifndef Action_h
#define Action_h

#define S_LINEAR 0
#define S_QUAD_EASEIN 1
#define S_QUAD_EASEOUT 2
#define S_QUAD_EASEINOUT 3
#define S_CUBIC_EASEIN 4
#define S_CUBIC_EASEOUT 5
#define S_CUBIC_EASEINOUT 6
#define S_QUART_EASEIN 7
#define S_QUART_EASEOUT 8
#define S_QUART_EASEINOUT 9
#define S_QUINT_EASEIN 10
#define S_QUINT_EASEOUT 11
#define S_QUINT_EASEINOUT 12
#define S_SINE_EASEIN 13
#define S_SINE_EASEOUT 14
#define S_SINE_EASEINOUT 15
#define S_EXPO_EASEIN 16
#define S_EXPO_EASEOUT 17
#define S_EXPO_EASEINOUT 18
#define S_ELASTIC_EASEIN 19
#define S_ELASTIC_EASEOUT 20
#define S_ELASTIC_EASEINOUT 21
#define S_BACK_EASEIN 22
#define S_BACK_EASEOUT 23
#define S_BACK_EASEINOUT 24
#define S_BOUNCE_EASEIN 25
#define S_BOUNCE_EASEOUT 26
#define S_BOUNCE_EASEINOUT 27

#include <functional>

typedef struct lua_State lua_State;

namespace Supernova{

    class Object;

    class Action{

        friend class Object;
        
    protected:
        
        float duration;
        bool loop;
        
        float (*function)(float);
        int functionLua;
        
        unsigned int timecount;
        unsigned int steptime;
        
        bool running;
        Object* object;
        
        float time;
        float value;
        
    public:
        Action();
        Action(float duration, bool loop);
        Action(float duration, bool loop, float (*function)(float));
        virtual ~Action();
        
        static float linear(float time);
        static float quadEaseIn(float time);
        static float quadEaseOut(float time);
        static float quadEaseInOut(float time);
        static float cubicEaseIn(float time);
        static float cubicEaseOut(float time);
        static float cubicEaseInOut(float time);
        static float quartEaseIn(float time);
        static float quartEaseOut(float time);
        static float quartEaseInOut(float time);
        static float quintEaseIn(float time);
        static float quintEaseOut(float time);
        static float quintEaseInOut(float time);
        static float sineEaseIn(float time);
        static float sineEaseOut(float time);
        static float sineEaseInOut(float time);
        static float expoEaseIn(float time);
        static float expoEaseOut(float time);
        static float expoEaseInOut(float time);
        static float circEaseIn(float time);
        static float circEaseOut(float time);
        static float circEaseInOut(float time);
        static float elasticEaseIn(float time);
        static float elasticEaseOut(float time);
        static float elasticEaseInOut(float time);
        static float backEaseIn(float time);
        static float backEaseOut(float time);
        static float backEaseInOut(float time);
        static float bounceEaseIn(float time);
        static float bounceEaseOut(float time);
        static float bounceEaseInOut(float time);
        
        void setFunction(float (*function)(float));
        int setFunction(lua_State* L);
        
        void setFunctionType(int functionType);
        
        bool isRunning();

        virtual void start();
        virtual void stop();
        virtual void reset();
        
        virtual void step();
    };
}

#endif /* Action_h */
