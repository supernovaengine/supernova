#ifndef TIMEDACTION_COMPONENT_H
#define TIMEDACTION_COMPONENT_H

namespace Supernova{

    struct TimedActionComponent{
        float timecount;
        
        float duration;
        bool loop;
        
        float time;
        float value;
    };

}

#endif //TIMEDACTION_COMPONENT_H