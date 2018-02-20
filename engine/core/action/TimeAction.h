#ifndef TimeAction_h
#define TimeAction_h

#include "Action.h"
#include "Ease.h"

namespace Supernova{
    
    class Object;
    
    class TimeAction: public Action, public Ease{
        
    protected:
        
        float duration;
        bool loop;
        
        float time;
        float value;
        
    public:
        TimeAction();
        TimeAction(float duration, bool loop);
        TimeAction(float duration, bool loop, float (*function)(float));
        virtual ~TimeAction();

        float getDuration();
        void setDuration(float duration);

        bool isLoop();
        void setLoop(bool loop);

        float getTime();
        float getValue();
        
        virtual bool run();
        virtual bool pause();
        virtual bool stop();
        
        virtual bool step();
    };
}

#endif /* TimeAction_h */
