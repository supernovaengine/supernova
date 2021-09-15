#ifndef ACTION_COMPONENT_H
#define ACTION_COMPONENT_H

#include "Supernova.h"

namespace Supernova{

    enum class ActionState{
        Running,
        Paused,
        Stopped
    };

    struct ActionComponent{
        Entity target;
        ActionState state = ActionState::Stopped;

        bool startTrigger = false;
        bool stopTrigger = false;
        bool pauseTrigger = false;
    };
    
}

#endif //ACTION_COMPONENT_H