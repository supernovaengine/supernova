#ifndef Action_h
#define Action_h

//
// (c) 2018 Eduardo Doria.
//

#include "util/FunctionSubscribe.h"

namespace Supernova{

    class Object;

    class Action{

        friend class Object;
        friend class Animation;
        
    protected:
        
        float timecount;
        
        bool running;
        Object* object;
        
    public:
        Action();
        virtual ~Action();

        FunctionSubscribe<void(Object*)> onStart;
        FunctionSubscribe<void(Object*)> onRun;
        FunctionSubscribe<void(Object*)> onPause;
        FunctionSubscribe<void(Object*)> onStop;
        FunctionSubscribe<void(Object*)> onFinish;
        FunctionSubscribe<void(Object*,float)> onUpdate;

        Object* getObject();

        void setTimecount(float timecount);

        bool isRunning();

        virtual bool run();
        virtual bool pause();
        virtual bool stop();

        virtual bool update(float interval);
    };
}

#endif /* Action_h */
