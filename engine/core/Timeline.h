#ifndef Timeline_h
#define Timeline_h

#define S_TIMELINE_LINEAR 0
#define S_TIMELINE_EXPONENTIAL 1


namespace Supernova{

    class Timeline{
        
    private:
        
        int function;
        bool started;
        
        unsigned int timecount;
        
    protected:
        
        int duration;
        bool loop;
        
        float time;
        float value;
        
    public:
        Timeline();
        Timeline(int duration, bool loop);
        Timeline(int duration, bool loop, int function);
        virtual ~Timeline();
        
        bool isStarted();
        
        void start();
        void stop();
        
        virtual void step();
    };
}

#endif /* Timeline_h */
