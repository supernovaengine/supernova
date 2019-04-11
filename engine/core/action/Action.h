#ifndef Action_h
#define Action_h

//
// (c) 2018 Eduardo Doria.
//

#include "util/FunctionCallback.h"

namespace Supernova{

    class Object;

    class Action{

        friend class Object;
        friend class Animation;
        
    protected:
        
        unsigned long timecount;
        unsigned int steptime;
        
        bool running;
        Object* object;
        
    public:
        Action();
        virtual ~Action();

        FunctionCallback<void(Object*)> onStart;
        FunctionCallback<void(Object*)> onRun;
        FunctionCallback<void(Object*)> onPause;
        FunctionCallback<void(Object*)> onStop;
        FunctionCallback<void(Object*)> onFinish;
        FunctionCallback<void(Object*)> onStep;

        Object* getObject();

        void setTimecount(unsigned long timecount);

        bool isRunning();

        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        
        virtual bool step();
    };
}

#endif /* Action_h */
