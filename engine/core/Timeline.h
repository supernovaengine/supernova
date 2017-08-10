#ifndef Timeline_h
#define Timeline_h

#define S_TIMELINE_LINEAR 0
#define S_TIMELINE_EXPONENTIAL 1


namespace Supernova{

    class Object;

    class Timeline{

        friend class Object;
        
    private:
        bool started;
        Object* parent;

        unsigned int timecount;
        
    protected:
        
        float duration;
        bool loop;
        int function;
        
        float time;
        float value;
        
    public:
        Timeline();
        Timeline(float duration, bool loop);
        Timeline(float duration, bool loop, int function);
        virtual ~Timeline();
        
        bool isStarted();

        void start();
        void stop();
        
        virtual void step();
    };
}

#endif /* Timeline_h */
