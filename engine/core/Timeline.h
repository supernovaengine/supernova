#ifndef Timeline_h
#define Timeline_h

#define S_TIMELINE_LINEAR 0
#define S_TIMELINE_EXPONENTIAL 1


namespace Supernova{

    class Timeline{

        friend class Object;
        
    private:
        bool started;
        
        unsigned int timecount;

        void start();
        void stop();
        
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
        
        virtual void step();
    };
}

#endif /* Timeline_h */
