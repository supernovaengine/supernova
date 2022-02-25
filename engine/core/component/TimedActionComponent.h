#ifndef TIMEDACTION_COMPONENT_H
#define TIMEDACTION_COMPONENT_H

#include "action/Ease.h"

namespace Supernova{

    struct TimedActionComponent{
        float timecount = 0;

        float time = 0;
        float value = 0;
        
        float duration = 0;
        bool loop = false;

        Function<float(float)> function = Supernova::Function<float(float)>(Ease::linear);
    };

}

#endif //TIMEDACTION_COMPONENT_H