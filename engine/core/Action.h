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
        static float elasticEaseIn(float time);
        static float elasticEaseOut(float time);
        static float elasticEaseInOut(float time);
        
        void setFunction(float (*function)(float));
        
        bool isStarted();

        virtual void start();
        virtual void stop();
        
        virtual void step();
    };
}

#endif /* Action_h */
