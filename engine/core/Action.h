#ifndef Action_h
#define Action_h


namespace Supernova{

    class Object;

    class Action{

        friend class Object;
        
    protected:
        
        float duration;
        bool loop;
        float (*function)(float);
        
        unsigned int timecount;
        
        bool started;
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
        
        bool isStarted();

        virtual void start();
        virtual void stop();
        
        virtual void step();
    };
}

#endif /* Action_h */
