#ifndef Action_h
#define Action_h

namespace Supernova{

    class Object;

    class Action{

        friend class Object;
        
    protected:
        
        unsigned long timecount;
        unsigned int steptime;
        
        bool running;
        Object* object;
        
    public:
        Action();
        virtual ~Action();
        
        bool isRunning();

        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        
        virtual bool step();
    };
}

#endif /* Action_h */
