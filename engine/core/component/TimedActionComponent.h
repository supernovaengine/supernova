#ifndef TIMEDACTION_COMPONENT_H
#define TIMEDACTION_COMPONENT_H

#include "action/Ease.h"

namespace Supernova{

    struct TimedActionComponent{
        float timecount;
        
        float duration;
        bool loop;
        
        float time;
        float value;

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

}

#endif //TIMEDACTION_COMPONENT_H